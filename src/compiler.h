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
#include "fragment.h"
#include "request.h"
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

    bool handle_response(QString ctx, QString loc, Response r);
    void report_error(QString ctx, QString loc, QString msg);

    // Low-level requests
    void sendAsyncRequest(const QString &text);
    QString sendRequest(const QString &text);

    // High-level requests
    Response getVersion(void);
    Response compile(const QStringList &args);
    Response checkFragment(const Fragment &fragment);
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

    void tryConnect(int retries = 20, int intervalMs = 100);
    QString readAnswer();
    QProcess serverProcess;
    QLocalSocket socket;
    QString socketPath;
    QString socketName;
};
