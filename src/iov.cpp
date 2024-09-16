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

Iov::IoKind Iov::ioKindOfString(QString s)
{
  if ( s == "in" ) return Iov::IoIn;
  else if ( s == "out" ) return Iov::IoOut;
  else if ( s == "var" ) return Iov::IoVar;
  else Q_ASSERT(false);
}

Iov::IoType Iov::ioTypeOfString(QString s)
{
  if ( s == "event" ) return Iov::TyEvent;
  else if ( s == "int" ) return Iov::TyInt;
  else if ( s == "bool" ) return Iov::TyBool;
  else Q_ASSERT(false);
}

QString Iov::stringOfKind(IoKind k)
{
  switch ( k ) {
  case IoIn: return QString("in");
  case IoOut: return QString("out");
  case IoVar: return QString("var");
  }
}

QString Iov::stringOfType(IoType t)
{
  switch ( t ) {
  case TyEvent: return QString("event");
  case TyInt: return QString("int");
  case TyBool: return QString("bool");
  }
}

QString Iov::toString(bool withStim)
{
  QString r;
  r = stringOfKind(kind) + " " + name + " : " + stringOfType(type);
  if ( kind == IoIn && withStim ) r += " = " + stim.toString(); 
  return r;
}

QString Iov::stringOfList(QList<Iov*> ios)
{
  QString r;
  foreach ( Iov* io, ios)
    r += io->toString() + "\\l";
  return r;
}

