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

#include "response.h"

// Public ctors
Response Response::Version(const QString &v) { return Response(Kind::Version,v); }
Response Response::Error(const QString &msg) { return Response(Kind::Error, msg); }
Response Response::CompilationFailed(const QString &msg) { return Response(Kind::Compiled, msg); }
Response Response::CheckingFailed(const QString &msg) { return Response(Kind::Checked, msg); }
Response Response::CompilationOk(const QStringList &files) { return Response(Kind::Compiled, files); }
Response Response::CheckingOk(const QStringList& rds, const QStringList& wrs) { return Response(Kind::Checked,rds,wrs); }
Response Response::None() { return Response(Kind::None); }

// Private ctors
Response::Response(Kind kind) : m_kind(kind) // None
{
  switch ( kind ) {
  case Kind::Checked: m_result = true; break;
  default: break;
  }
}

Response::Response(Kind kind, const QString &s) : m_kind(kind) // Version, CompilationFailed, CheckingFailed, Error
{
  switch ( kind ) {
  case Kind::Version: m_version = s; break;
  case Kind::Compiled: m_result = false; m_message = s; break;
  case Kind::Checked: m_result = false; m_message = s; break;
  case Kind::Error: m_message = s; break;
  default: break; // Should not happen
  }
}

Response::Response(Kind kind, const QStringList &ss) // CompilationOk
  : m_kind(kind), m_result(true), m_files(ss) {}

Response::Response(Kind kind, const QStringList &rds, const QStringList &wrs) // CheckingOk
  : m_kind(kind), m_result(true), m_rds(rds), m_wrs(wrs) {}

Response::Kind Response::kind() const { return m_kind; }
QString Response::version() const { return m_version; }
bool Response::result() const { return m_result; }
QStringList Response::files() const { return m_files; }
QStringList Response::rds() const { return m_rds; }
QStringList Response::wrs() const { return m_wrs; }
QString Response::message() const { return m_message; }
//QString Response::error() const { return m_error; }

QJsonObject Response::toJson() const {
    QJsonObject obj;
    switch (m_kind) {
    case Kind::Version:
        obj["kind"] = "version";
        obj["version"] = m_version;
        break;
    case Kind::Compiled: {
        obj["kind"] = "compiled";
        obj["result"] = m_result;
        QJsonArray arr;
        for (const auto &f : m_files) arr.append(f);
        obj["files"] = arr;
        break;
    }
    case Kind::Checked:
        obj["kind"] = "checked";
        obj["result"] = m_result;
        obj["message"] = m_message;
        break;
    case Kind::Error:
        obj["kind"] = "error";
        obj["message"] = m_message;
        break;
    case Kind::None:
        obj["kind"] = "none";
        break;
    }
    return obj;
}

Response Response::fromJson(const QJsonObject &obj) {
    QString kind = obj["kind"].toString();
    if (kind == "version") {
        return Version(obj["version"].toString());
    } else if (kind == "compiled") {
        bool result = obj["result"].toBool();
        if ( result ) {
          QStringList files;
          for (const auto &v : obj["files"].toArray()) files.append(v.toString());
          return CompilationOk(files);
          }
        else {
          QString msg = obj["message"].toString();
          return CompilationFailed(msg);
          }
    } else if (kind == "checked") {
        bool result = obj["result"].toBool();
        if ( result ) {
          QStringList rds, wrs;
          for (const auto &v : obj["rds"].toArray()) rds.append(v.toString());
          for (const auto &v : obj["wrs"].toArray()) wrs.append(v.toString());
          return CheckingOk(rds,wrs);
          }
        else {
          QString msg = obj["message"].toString();
          return CheckingFailed(msg);
          }
    } else if (kind == "error") {
        return Error(obj["message"].toString());
    } else {
        return None();
    }
}

Response Response::fromString(const QString &jsonStr)
{
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8(), &err);
    if (err.error != QJsonParseError::NoError) {
        return Response::None(); // default fallback
    }
    return fromJson(doc.object());
}

QString Response::toString() const
{
    QJsonObject json = toJson();
    QJsonDocument doc(json);
    QByteArray data = doc.toJson(QJsonDocument::Compact);
    return QString::fromUtf8(data);
}
