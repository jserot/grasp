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

#include "iov.h"
#include <optional>

QString toString(IoKind k)
{
  switch ( k ) {
  case IoKind::IoIn: return QString("in");
  case IoKind::IoOut: return QString("out");
  case IoKind::IoVar: return QString("var");
  }
}

std::optional<IoKind> ioKindFromString(const QString &s)
{
  if ( s == "in" ) return IoKind::IoIn;
  else if ( s == "out" ) return IoKind::IoOut;
  else if ( s == "var" ) return IoKind::IoVar;
  else return std::nullopt;
}

QString toString(IoType t)
{
  switch ( t ) {
  case IoType::TyEvent: return QString("event");
  case IoType::TyInt: return QString("int");
  case IoType::TyBool: return QString("bool");
  }
}

std::optional<IoType> ioTypeFromString(QString s)
{
  if ( s == "event" ) return IoType::TyEvent;
  else if ( s == "int" ) return IoType::TyInt;
  else if ( s == "bool" ) return IoType::TyBool;
  else return std::nullopt;
}

QString Iov::toString(void)
{
  QString r;
  r = ::toString(this->kind()) + " " + name + " : " + ::toString(type);
  return r;
}

QString toString(QList<Iov*> vs, QString sep, bool withType) 
{
  QStringList rs;
  for (const auto& v : vs) {
    if ( withType )
      rs << v->name + ": " + toString(v->type);
    else
      rs << v->name;
    }
  return rs.join(sep);
}

