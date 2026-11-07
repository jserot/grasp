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
#include <optional>
#include "stimulus.h"

enum class IoKind {
  IoIn=0,
  IoOut,
  IoVar
};

QString toString(IoKind k);
std::optional<IoKind> ioKindFromString(const QString &s);

enum class IoType {
  TyEvent=0,
  TyInt,
  TyBool
};

QString toString(IoType t);
std::optional<IoType> ioTypeFromString(QString s);

/* Base abstract class for model inputs, outputs and variables */

class Iov
{
public:
  QString name;
  IoType type;

public:
  Iov(const QString& name, const IoType& type)
    : name(name), type(type) {};
  virtual ~Iov() = default;

  virtual IoKind kind() const = 0;
  virtual Iov* clone() const = 0;

  virtual QString toString(void);
  };

class Input : public Iov
{
public:
  Input(const QString& name, const IoType& type, const Stimulus& stim)
    : Iov(name, type), stim(stim) {}

  IoKind kind() const override { return IoKind::IoIn; }
  Input* clone() const override { return new Input(*this); }
public:
  Stimulus stim;
};

class Output : public Iov
{
public:
  Output(const QString& name, const IoType& type)
    : Iov(name, type) {}
  IoKind kind() const override { return IoKind::IoOut; }
  Output* clone() const override { return new Output(*this); }
private:
};

class Variable : public Iov
{
public:
  Variable(const QString& name, const IoType& type)
    : Iov(name, type) {}
  IoKind kind() const override { return IoKind::IoVar; }
  Variable* clone() const override { return new Variable(*this); }
private:
};

QString toString(QList<Iov*> vs, QString sep=", ", bool withType=true);
