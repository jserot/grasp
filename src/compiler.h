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
#pragma once

#include <QString>
#include <QProcess>
#include <QLocalSocket>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include "diagram.h"
#include "response.h"

// Interface to the RFSM compiler in server mode

class Compiler : public QObject {
    Q_OBJECT

public:
    explicit Compiler(QObject *parent = nullptr);
    ~Compiler();

    static const QString minimalVersion;

    void startServer(const QString &serverPath, const QString &socketPath);
    void stopServer();

    // Low-level requests
    void sendRequest(const QString &text);
    QString sendRequestAndReadResponse(const QString &text);

    // High-level requests
    Response getVersion(void);
    Response compile(const QStringList &args);
    void close(void);

signals:
    void serverError(const QString &error);
    //void serverStarted(); // Not used
    // void connected();
    //void messageReceived(const QString &message); // Not used (synchronous mode)
    // void disconnected();

private slots:
    void onConnected();
    // void onReadyRead();  // For asynchronous reception of responses; not used here
    void onDisconnected();
    void onErrorOccurred(QLocalSocket::LocalSocketError socketError);

private:
    static const int TimeOutMs = 2000;  // Timeout when waiting for a response after sending a request (synchronous mode)

    QString readAnswer();
    QProcess serverProcess;
    QLocalSocket socket;
    QString socketPath;
};
