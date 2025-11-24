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
#include <QFileInfo>
#include <QString>
#include <QStringList>
#include <QLocalSocket>
#include <QProcess>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QMessageBox>
#include "request.h"
#include "response.h"

const QString Compiler::minimalVersion = "2.2";

Compiler::Compiler(QObject *parent) : QObject(parent)
{
    serverProcess.setProcessChannelMode(QProcess::ForwardedChannels);

    connect(&socket, &QLocalSocket::connected, this, &Compiler::onConnected);
    // connect(&socket, &QLocalSocket::readyRead, this, &Compiler::onReadyRead);  // not used here
    connect(&socket, &QLocalSocket::disconnected, this, &Compiler::onDisconnected);
    connect(&socket, &QLocalSocket::errorOccurred, this, &Compiler::onErrorOccurred);
}

Compiler::~Compiler() {
  qDebug() << "compiler: done";
  stopServer();
}

void Compiler::startServer(const QString &serverPath, const QString &socketPath)
{
    this->socketPath = socketPath;
    QFile socketFile(socketPath);
    if ( socketFile.exists() ) {
      qDebug() << "Socket" << socketPath << "exists. Deleting";
      socketFile.remove();
      }
    QStringList serverArgs;
    serverArgs << "-server_mode" << "-socket_path" << socketPath;
    serverArgs << "-verbose"; 
    qDebug() << "compiler: launching:" << serverPath << serverArgs;
    serverProcess.start(serverPath, serverArgs);
    socketName = QFileInfo(socketPath).fileName();
    if ( serverProcess.waitForStarted(3000) ) {
      qDebug() << "compiler: server started in" << serverProcess.workingDirectory();
      //emit serverStarted();
      qDebug() << "compiler: connecting to socket " << socketName;
      QTimer::singleShot(300, this, [this]() { socket.connectToServer(socketName); });
      }
    else {
      qDebug() << "compiler: cannot launch server";
      emit serverError("Cannot launch compiler server");
      }
}


void Compiler::sendAsyncRequest(const QString &text)
{
    if (socket.state() == QLocalSocket::ConnectedState) {
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
  //if (socket.state() == QLocalSocket::ConnectedState) {
        socket.disconnectFromServer();
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

void Compiler::onErrorOccurred(QLocalSocket::LocalSocketError) {
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
    qDebug() << "Wrong response to check_fragment request: " << r.message();
    QMessageBox::critical(Globals::mainWindow, tr("Diagram checking"), r.message());
    return false;
    }
}

void Compiler::close(void)
{
  Request q = Request::Close();
  sendRequest(q.toString());
}
