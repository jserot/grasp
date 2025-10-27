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

// Model fragments (state valuations, guards, actions) sent to the compiler server for checking

#pragma once

#include <QString>
#include <QPair>
#include <QList>
#include <QJsonObject>
#include <QJsonArray>

class Fragment
{
public:
  struct Context {
    QMap<QString, QString> inps;
    QMap<QString, QString> outps;
    QMap<QString, QString> vars;
    };

private:
    Context ctx;
    QString obj;

public:
    Fragment() = default;

    Fragment(const Context& ctx, const QString &obj) : ctx(ctx), obj(obj) {}
    Fragment(const QMap<QString, QString> &inps, // TO BE REMOVED ?
             const QMap<QString, QString> &outps,
             const QMap<QString, QString> &vars,
             const QString &obj) : obj(obj) { ctx.inps = inps; ctx.outps = outps; ctx.vars = vars; }

  QJsonObject toJson() const;

  static Fragment fromJson(const QJsonObject &json);

private:
  static QJsonArray mapToJsonArray(const QMap<QString,QString> &list);
  static QMap<QString,QString> jsonArrayToMap(const QJsonArray &arr);
};
