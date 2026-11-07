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

#include "compilerOption.h"

CompilerOption::CompilerOption(QString category, QString name, CompilerOption::Kind kind, QString val, QString desc)
  : category(category), name(name), kind(kind), desc(desc)
{
  Q_UNUSED(val); 
}

CompilerOption::CompilerOption(QString category, QString name, CompilerOption::Kind kind, QString val)
   : category(category), name(name), kind(Kind::StringOpt)
{
  switch ( kind ) {
  case CompilerOption::UnitOpt: this->val = QVariant(val=="on"); break;
  case CompilerOption::StringOpt: this->val = QVariant(val); break;
  case CompilerOption::IntOpt: this->val = QVariant(val.toInt()); break;
  }
}

CompilerOption::CompilerOption(QString category, QString name, QString val)
   : category(category), name(name), kind(Kind::StringOpt), val(QVariant(val))
{
}

CompilerOption::CompilerOption(QString category, QString name, int val)
   : category(category), name(name), kind(Kind::IntOpt), val(QVariant(val))
{
}

CompilerOption::CompilerOption(QString category, QString name, bool val)
   : category(category), name(name), kind(Kind::UnitOpt), val(QVariant(val))
{
}

QStringList CompilerOption::getArgs()
{
  // Note: the name and the value must be passed as separate arguments ("-stop_time 100" is rejected by rfsmc)
  switch ( kind ) {
  case CompilerOption::UnitOpt: return val.toBool() ? QStringList{name} : QStringList();
  case CompilerOption::StringOpt:
  case CompilerOption::IntOpt: return val.toString().isEmpty() ? QStringList() : QStringList{name, val.toString()};
  }
  return QStringList();
}

// QString const CompilerOption::toString()
// {
//   return name + "(" + category + "," + kind + "," + desc + ") = " + val.toString();
// }
