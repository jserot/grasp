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

#ifndef _SYNTAX_CHECKER
#define _SYNTAX_CHECKER

#include "commandExec.h"

struct SyntaxCheckerResult
{
  bool ok;
  QStringList lhs_vars;
  QStringList rhs_vars;
  QString msg;
  SyntaxCheckerResult(bool ok) : ok(ok) { }
  SyntaxCheckerResult(bool ok, QStringList& lhs_vars, QStringList& rhs_vars)
    : ok(ok), lhs_vars(lhs_vars), rhs_vars(rhs_vars) { }
};

class SyntaxChecker
{
 public:
  SyntaxChecker(CommandExec *executor, QString syntaxChecker);
  SyntaxCheckerResult check_valuation(QStringList outps, QString valuation);
  SyntaxCheckerResult check_guard(QStringList inps, QStringList outps, QStringList vars, QString guard);
  SyntaxCheckerResult check_action(QStringList inps, QStringList outps, QStringList vars, QString action);
private:
  CommandExec *executor;
  QString checker;
  SyntaxCheckerResult check(QStringList args, QString input);
};
#endif
