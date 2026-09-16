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
#include "stimulus.h"

enum class IoKind {
  IoIn=0,
  IoOut,
  IoVar
};

enum class IoType {
  TyEvent=0,
  TyInt,
  TyBool
};

/* Base abstract class for model inputs, outputs and variables */

class Iov
{
public:
  QString name;
  IoKind kind;
  IoType type;

  public:
  Iov(const QString& name, const IoKind& kind, const IoType& type)
    : name(name), kind(kind), type(type {};
  ~Iov() { };

  static IoKind ioKindOfString(QString s);
  static IoType ioTypeOfString(QString s);
  static QString stringOfKind(IoKind k);
  static QString stringOfType(IoType t);
  QString toString(bool withStim=true);
  static QString stringOfList(QList<Iov*> ios, bool withStim);
  };

class Input : public Iov
{
public:
  Input(const QString& name, const IoType& type /*, const Stimulus& stim*/)
    : Iov(name, IoIn, type) {}
private:
  StimulusTrack stim;
}

class Output : public Iov
{
public:
  Input(const QString& name, const IoType& type)
    : Iov(name, IoOut, type) {}
private:
}

class Variable : public Iov
{
public:
  Input(const QString& name, const IoType& type)
    : Iov(name, IoVar, type) {}
private:
}
