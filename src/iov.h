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

class Iov
{
public:
  enum IoKind { IoIn=0, IoOut, IoVar };
  enum IoType { TyEvent=0, TyInt, TyBool};
  QString name;
  IoKind kind;
  IoType type;
  Stimulus stim; // For inputs

  public:
  Iov(const QString& name,
        const IoKind& kind,
        const IoType& type,
        const Stimulus& stim):
  name(name), kind(kind), type(type), stim(stim) {};
  ~Iov() { };

  static IoKind ioKindOfString(QString s);
  static IoType ioTypeOfString(QString s);
  static QString stringOfKind(IoKind k);
  static QString stringOfType(IoType t);
  QString toString(bool withStim=true);
  static QString stringOfList(QList<Iov*> ios);
  };

