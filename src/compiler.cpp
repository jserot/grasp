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

#include <QString>
#include <QStringList>
#include <QLocalSocket>
#include <QProcess>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
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
    QStringList serverArgs;
    serverArgs << "-server_mode" << "-socket_path" << socketPath << "-verbose";
    qDebug() << "compiler: launching:" << serverPath << serverArgs;
    serverProcess.start(serverPath, serverArgs);
    if ( serverProcess.waitForStarted(3000) ) {
      qDebug() << "compiler: server started in" << serverProcess.workingDirectory();
      //emit serverStarted();
      QTimer::singleShot(300, this, [this]() { socket.connectToServer(this->socketPath); });
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
        qDebug() << "compiler: sending: " << data;
        socket.write(data);
        socket.flush();
        qDebug() << "compiler: sent";
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
  else
    qDebug() << "compiler: timeout when waiting for response";
    return "<no response>";
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
  return s.kind() == Response::Kind::Version ?
    s 
    : Response::Error("Invalid response from compiler");
}

Response Compiler::compile(const QStringList &args)
{
  Request q = Request::Compile(args);
  QString r = sendRequest(q.toString());
  Response s = Response::fromString(r);
  return s.kind() == Response::Kind::Compiled ? s : Response::Error("Invalid response from compiler");
}

Response Compiler::checkFragment(const Fragment &fragment)
{
  Request q = Request::CheckFragment(fragment);
  QString r = sendRequest(q.toString());
  Response s = Response::fromString(r);
  return s.kind() == Response::Kind::Checked ? s : Response::Error("Invalid response from compiler");
}

void Compiler::close(void)
{
  Request q = Request::Close();
  sendRequest(q.toString());
}
