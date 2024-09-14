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

#include "guardValidator.h"
#include "commandExec.h"

GuardValidator::GuardValidator(CommandExec *executor, QString checker)
{
  this->executor = executor;
  this->checker = checker;
}

QValidator::State GuardValidator::validate(QString &input, int &pos) const
{
  Q_UNUSED(pos);
  QString wDir = ".";
  QStringList args = { "-guard", input };
  assert(executor);

  if ( ! executor->execute(wDir, checker, args) ) return QValidator::Intermediate;
  QStringList outps = executor->getOutputs();
  qDebug() << "Checker output=" << outps; 
  if ( outps.length() < 1 ) return QValidator::Intermediate;
  QStringList res = outps.at(0).split(";", Qt::KeepEmptyParts);
  if ( res.length() != 3 ) return QValidator::Intermediate;
  if ( res.at(0) != "1" ) return QValidator::Intermediate;
  QStringList lhs_vars = res.at(1).split(",",Qt::SkipEmptyParts);
  QStringList rhs_vars = res.at(2).split(",",Qt::SkipEmptyParts);
  qDebug() << "Guard validated. LHS vars=" << lhs_vars << "RHS vars" << rhs_vars;
  return QValidator::Acceptable;
}

void GuardValidator::fixup(QString &input) const
{
  Q_UNUSED(input);
  // Nothing
}
