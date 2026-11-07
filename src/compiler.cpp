/***********************************************************************/
/*                                                                     */
/*       This file is part of the Grasp software package               */
/*                                                                     */
/*  Copyright (c) 2019-present, Jocelyn SEROT (jocelyn.serot@uca.fr)   */
/*                       All rights reserved.                          */
/*                                                                     */
/*    This source code is licensed under the license found in the      */
/*      LICENSE file in the root directory of this source tree.        */
/*                                                                     */
/***********************************************************************/

#include "compiler.h"
#include "globals.h"
#include "appFiles.h"

#include <QFile>
#include <QThread>
#include <QFileInfo>
#include <QString>
#include <QStringList>
#include <QTcpSocket>
#include <QProcess>
#include <QCoreApplication>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QMessageBox>
#include <QElapsedTimer>
#include "request.h"
#include "response.h"
#include "globals.h"
#include "compilerPaths.h"

const bool Compiler::printServerMessages = false;
const bool Compiler::logClientServerMessages = false;

const QVersionNumber& Compiler::minimalVersion()
{
    static const QVersionNumber v = QVersionNumber::fromString("2.3.1");
    return v;
} 

Compiler::Compiler(QWidget *parent)
{
    this->parent = parent;
    serverPath = Globals::compilerPaths->getPath("COMPILER");
    if ( serverPath.isNull() || serverPath.isEmpty() ) {
      QMessageBox::critical(parent, "", "Invalid path for RFSMC compiler ("+ serverPath +")\nCheck file " + AppFiles::configFile());
      exit(1);
      }
    QString socketPortSpec = Globals::compilerPaths->getPath("SOCKETPORT");
    bool ok; 
    socketPort = socketPortSpec.toInt(&ok);
    if ( ! ok ) {
      QMessageBox::critical(parent, "", "Invalid socket port number (" + socketPortSpec + ")\nCheck file " + AppFiles::configFile());
      exit(1);
      }

    //serverProcess.setProcessChannelMode(QProcess::ForwardedChannels);
    //serverProcess.setProcessChannelMode(QProcess::MergedChannels); // Redirect both server's stdout and stderr to client
    // connect(&serverProcess, &QProcess::readyReadStandardOutput, [&]() {
    //   QByteArray msg = serverProcess.readAllStandardOutput();
    //   qDebug() << "**** message from server:" << msg ;
    //   });

    connect(&socket, &QTcpSocket::connected, this, &Compiler::onConnected);
    // connect(&socket, &QTcpSocket::readyRead, this, &Compiler::onReadyRead);  // not used here
    connect(&socket, &QTcpSocket::disconnected, this, &Compiler::onDisconnected);
    connect(&socket, &QTcpSocket::errorOccurred, this, &Compiler::onErrorOccurred);

}

Compiler::~Compiler() {
  qDebug() << "compiler: done";
  stopServer();
}

void Compiler::startServer()
{
    QStringList serverArgs;

    serverArgs << "-server_mode" << "-socket_port" << QString::number(socketPort);
    if ( printServerMessages ) serverArgs = serverArgs << "-verbose"; 
    qDebug() << "compiler: launching server:" << serverPath << serverArgs;

    serverReady = false;
    serverProcess.setProcessChannelMode(QProcess::MergedChannels); // catch stdout and stderr
    serverProcess.setWorkingDirectory(QCoreApplication::applicationDirPath());
    serverProcess.start(serverPath, serverArgs);
    // Wait for server to start (max 5 sec)
    if (!serverProcess.waitForStarted(5000)) { // 5 secondes max
        qDebug() << "compiler: cannot launch server";
        emit serverError("Cannot launch compiler server");
        return;
    }
    qDebug() << "compiler: server started in" << serverProcess.workingDirectory();
    // Wait for _first_ message from server telling it's listening for connection
    // Note: this is crucial that this message is emitted even if the server is launched without the -verbose option
    if (serverProcess.waitForReadyRead(5000)) {
      QByteArray firstOutput = serverProcess.readAll();
      qDebug() << firstOutput;
      if ( firstOutput.contains("listening") ) {
        qDebug() << "compiler: server now listening"; 
        connectToServer(socketPort);
      if ( printServerMessages ) {
        // If set, just catch and print all messages from server on stdout/stderr
        // Note: app-level messages are going through the established socket connexion !
        connect(&serverProcess, &QProcess::readyReadStandardOutput, [&]() {
          QByteArray msg = serverProcess.readAllStandardOutput();
          qDebug() << "**" << msg ;
          });
        }
      }
      else {
        qDebug() << "compiler: server is not listening";
        QString msg = "Cannot start the compiler server:\n" + QString::fromUtf8(firstOutput).trimmed();
        if ( firstOutput.contains("EADDRINUSE") )
          msg += "\n\nThe socket port " + QString::number(socketPort) + " is already used. "
            "Another instance of Grasp may be running.";
        emit serverError(msg);
        }
      }
    else {
      qDebug() << "compiler: server gave no output";
      emit serverError("Cannot start the compiler server (no answer from " + serverPath + ")");
      }
}

void Compiler::connectToServer(const int socketPort)
{
    bool connected = false;
    qDebug() << "compiler: attempting socket connection";
    for ( int i=0; i<50; ++i ) { // Retry loop
      socket.connectToHost("127.0.0.1", socketPort);
      if (socket.waitForConnected(100)) {  // 100 ms max
        connected = true;
        break;
        }
      QThread::msleep(50);
      }
    if ( connected )
      qDebug() << "compiler: connected to compiler server!";
    else {
      qDebug() << "compiler: failed to connect to server";
      emit serverError("Cannot connect to compiler server");
      return;
      }
}

bool Compiler::serverAvailable()
{
  return socket.state() == QTcpSocket::ConnectedState && serverProcess.state() == QProcess::Running;
}

void Compiler::sendAsyncRequest(const QString &text)
{
    if (socket.state() == QTcpSocket::ConnectedState) {
        // The protocol does not number the requests : an answer arriving after its timeout would be taken as
        // the answer to the next request. Any such stale data (already buffered by the event loop, which runs
        // between requests) is discarded before sending a new request.
        if ( socket.bytesAvailable() > 0 )
          qDebug() << "compiler: discarding stale data:" << socket.readAll();
        QByteArray data = text.trimmed().toUtf8() + '\n';
        if ( logClientServerMessages ) qDebug() << "compiler: sending: " << data;
        socket.write(data);
        socket.flush();
        if ( logClientServerMessages ) qDebug() << "compiler: sent";
        }
    else {
        qDebug() << "compiler: server error: no active connexion";
        }
}

bool Compiler::sendRequest(const QString &text, QString &answer, int timeoutMs)
{
  if ( ! serverAvailable() ) {
    qDebug() << "compiler: server not available";
    return false;
    }
  sendAsyncRequest(text);
  bool ok = readAnswer(timeoutMs, answer);
  if ( logClientServerMessages ) qDebug() << "compiler: received: " << answer;
  return ok;
}

bool Compiler::readAnswer(int timeoutMs, QString &answer)
{
  // An answer is a single line, which may arrive in several parts
  QElapsedTimer timer;
  timer.start();
  while ( ! socket.canReadLine() ) {
    qint64 remaining = timeoutMs - timer.elapsed();
    if ( remaining <= 0 || socket.state() != QTcpSocket::ConnectedState ) {
      qDebug() << "compiler: timeout or disconnection when waiting for response";
      return false;
      }
    socket.waitForReadyRead(static_cast<int>(remaining));
    }
  answer = QString::fromUtf8(socket.readLine()).trimmed();
  return true;
}

Response Compiler::request(const QString &text, int timeoutMs)
{
  if ( ! serverAvailable() )
    return Response::Error("The compiler server is not running (please restart the application)");
  QString answer;
  if ( ! sendRequest(text, answer, timeoutMs) )
    return Response::Error("No response from the compiler server");
  return Response::fromString(answer);
}

void Compiler::stopServer() {
  qDebug() << "compiler: stopping server: socket.state=" << socket.state() << " process state=" << serverProcess.state();
  //if (socket.state() == QTcpSocket::ConnectedState) {
        socket.disconnectFromHost();
      //}
      //if (serverProcess.state() == QProcess::Running) {
        serverProcess.terminate();
        serverProcess.waitForFinished();
        qDebug() << "compiler: server terminated";
      //}
        // TO FIX : socket is not deleted 
}

void Compiler::onConnected() {
  qDebug() << "compiler: connected to server";
    //emit connected();
}

void Compiler::onDisconnected() {
    // emit disconnected();
  qDebug() << "compiler: disconnected from server";
}

void Compiler::onErrorOccurred(QTcpSocket::SocketError) {
    qDebug() << "compiler: got error:" << socket.errorString();
    //emit serverError(socket.errorString());
}

// High-level interface

Response Compiler::getVersion(void)
{
  Request q = Request::GetVersion();
  Response s = request(q.toString(), CheckTimeOutMs);
  return s.kind() == Response::Kind::Version ? s : Response::Error("Invalid response from compiler");
}

Response Compiler::compile(const QStringList &args)
{
  Request q = Request::Compile(args);
  return request(q.toString(), CompileTimeOutMs);
}

Response Compiler::checkFragment(const Fragment &fragment)
{
  Request q = Request::CheckFragment(fragment);
  return request(q.toString(), CheckTimeOutMs);
}

void Compiler::report_error(QString ctx, QString loc, QString msg)
{
  QMessageBox::warning(Globals::mainWindow, ctx, loc + "\n\n" + msg);
}


bool Compiler::handle_response(QString ctx, QString loc, Response r)
{
  if ( r.kind() == Response::Kind::Checked ) {
    if ( r.result() == true )
      return true;
    else { 
      report_error(ctx, loc, r.message());
      return false;
      }
    }
  else {
    qDebug() << "compiler: wrong response to check_fragment request: " << r.message();
    if ( ! serverAvailable() ) {
      // A model check sends many requests : the user is only told once that the server is not available
      if ( unavailableReported ) return false;
      unavailableReported = true;
      }
    QMessageBox::critical(Globals::mainWindow, tr("Diagram checking"), r.message());
    return false;
    }
}

void Compiler::close(void)
{
  Request q = Request::Close();
  QString answer;
  sendRequest(q.toString(), answer); // Ignored (the server may already be gone)
}
