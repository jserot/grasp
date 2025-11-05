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
#include <QStringList>
#include <QJsonObject>
#include <QJsonArray>

class Response
{
public:
    enum class Kind {
        Version,
        Compiled,
        Checked,
        Error,
        None
    };

    // Public static ctors
    static Response Version(const QString &v);
    static Response CompilationOk(const QStringList &files);
    static Response CompilationFailed(const QString &msg);
    static Response CheckingOk(const QStringList& rds, const QStringList& wrs);
    static Response CheckingFailed(const QString &msg);
    static Response Error(const QString &msg);
    static Response None();

    // Accessors
    Kind kind() const;
    QString version() const;
    bool result() const;
    QStringList files() const;
    QStringList rds() const;
    QStringList wrs() const;
    QString message() const;

    QJsonObject toJson() const;
    QString toString() const;
    static Response fromJson(const QJsonObject &obj);
    static Response fromString(const QString &s);

private:
    Kind m_kind;
    QString m_version;
    bool m_result = false;
    QStringList m_files;
    QStringList m_rds;
    QStringList m_wrs;
    QString m_message;

    // Private constructors
    explicit Response(Kind kind);
    Response(Kind kind, const QString &err);
    Response(Kind kind, const QStringList &files);
    Response(Kind kind, const QStringList &rds, const QStringList &wrs);
};
