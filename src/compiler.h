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
#include <QTcpSocket>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVersionNumber>
#include "diagram.h"
#include "fragment.h"
#include "request.h"
#include "response.h"

// Interface to the RFSM compiler in server mode

class Compiler : public QObject {
    Q_OBJECT

public:
    explicit Compiler(QWidget *parent = 0);
    ~Compiler();

    static const QVersionNumber& minimalVersion();
    static const bool printServerMessages;
    static const bool logClientServerMessages;

    void startServer();
    void stopServer();

    bool handle_response(QString ctx, QString loc, Response r);
    void report_error(QString ctx, QString loc, QString msg);

    // Low-level requests
    void sendAsyncRequest(const QString &text);
    // Sends a request and waits (at most [timeoutMs]) for the answer. Returns false if no answer could be obtained
    bool sendRequest(const QString &text, QString &answer, int timeoutMs = CheckTimeOutMs);
    bool serverAvailable(); // true if the server is running and connected

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
    void onErrorOccurred(QTcpSocket::SocketError socketError);

private:
    QWidget *parent;
    // Timeouts when waiting for a response after sending a request (synchronous mode)
    static const int CheckTimeOutMs = 3000;    // For fragment checks and version requests
    static const int CompileTimeOutMs = 60000; // For compilation and simulation requests, which may take longer
    bool readAnswer(int timeoutMs, QString &answer);
    Response request(const QString &text, int timeoutMs);
    bool unavailableReported = false; // Set once the user has been told that the server is not available
    void connectToServer(const int socketPort);
    bool serverReady;
    QProcess serverProcess;
    QString serverPath;
    int socketPort;
    QTcpSocket socket;
};
