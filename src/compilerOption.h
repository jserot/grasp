/***********************************************************************/
/*                                                                     */
/*       This file is part of the RFSM Light software package          */
/*                                                                     */
/*  Copyright (c) 2019-present, Jocelyn SEROT (jocelyn.serot@uca.fr)   */
/*                       All rights reserved.                          */
/*                                                                     */
/*    This source code is licensed under the license found in the      */
/*      LICENSE file in the root directory of this source tree.        */
/*                                                                     */
/***********************************************************************/

#pragma once

#include <QCheckBox>
#include <QLineEdit>
#include <QVariant>

class CompilerOption 
{
 public:
  typedef enum { UnitOpt, StringOpt, IntOpt } Kind;

  CompilerOption() : category(""), name("") { }
  CompilerOption(QString category, QString name, CompilerOption::Kind kind, QString ignored, QString desc);
  CompilerOption(QString category, QString name, CompilerOption::Kind kind, QString val);
  CompilerOption(QString category, QString name, QString val);
  CompilerOption(QString category, QString name, int val);
  CompilerOption(QString category, QString name, bool val);

  QString getValue();
  //QString const toString();
  
 public:
  QString category;
  QString name;
  Kind kind;
  QString desc;
  QVariant val;
};
