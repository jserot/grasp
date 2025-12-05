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
#include "request.h"
#include "response.h"

const QString Compiler::minimalVersion = "2.3"; // Rfsm 2.2 used Unix-based sockets

Compiler::Compiler(QObject *parent) : QObject(parent)
{
    serverProcess.setProcessChannelMode(QProcess::ForwardedChannels);

    connect(&socket, &QTcpSocket::connected, this, &Compiler::onConnected);
    // connect(&socket, &QTcpSocket::readyRead, this, &Compiler::onReadyRead);  // not used here
    connect(&socket, &QTcpSocket::disconnected, this, &Compiler::onDisconnected);
    connect(&socket, &QTcpSocket::errorOccurred, this, &Compiler::onErrorOccurred);
}

Compiler::~Compiler() {
  qDebug() << "compiler: done";
  stopServer();
}

void Compiler::startServer(const QString &serverPath, const int socketPort)
{
    QStringList serverArgs;
    serverArgs << "-server_mode" << "-socket_port" << QString::number(socketPort) << "-verbose";
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
    // Wait for message from server telling it's listening for connection
    if (serverProcess.waitForReadyRead(5000)) {
      QByteArray firstOutput = serverProcess.readAll();
      qDebug() << firstOutput;
      if ( firstOutput.contains("listening") ) {
        qDebug() << "compiler: server now listening"; 
        connectToServer(socketPort);
        }
      else
        qDebug() << "compiler: server is not listening";
      }
    else
      qDebug() << "compiler: server gave no output";
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

void Compiler::sendAsyncRequest(const QString &text)
{
    if (socket.state() == QTcpSocket::ConnectedState) {
        QByteArray data = text.trimmed().toUtf8() + '\n';
        //qDebug() << "compiler: sending: " << data;
        socket.write(data);
        socket.flush();
        //qDebug() << "compiler: sent";
        }
    else {
        qDebug() << "compiler: server error: no active connexion";
        }
}

QString Compiler::sendRequest(const QString &text)
{
  sendAsyncRequest(text);
  QString response = readAnswer();
  return response;
}

QString Compiler::readAnswer()
{
  if (socket.waitForReadyRead(3000)) {
    QByteArray line = socket.readAll();
    line.chop(1);
    return line;
    }
  else {
    qDebug() << "compiler: timeout when waiting for response";
    return "<no response>";
    }
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
  QString r = sendRequest(q.toString());
  Response s = Response::fromString(r);
  return s.kind() == Response::Kind::Version ? s : Response::Error("Invalid response from compiler");
}

Response Compiler::compile(const QStringList &args)
{
  Request q = Request::Compile(args);
  QString r = sendRequest(q.toString());
  Response s = Response::fromString(r);
  return s;
}

Response Compiler::checkFragment(const Fragment &fragment)
{
  Request q = Request::CheckFragment(fragment);
  QString r = sendRequest(q.toString());
  Response s = Response::fromString(r);
  return s;
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
    QMessageBox::critical(Globals::mainWindow, tr("Diagram checking"), r.message());
    return false;
    }
}

void Compiler::close(void)
{
  Request q = Request::Close();
  sendRequest(q.toString());
}
