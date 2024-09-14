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

#include "syntaxChecker.h"
#include "commandExec.h"
#include <QtDebug>

SyntaxChecker::SyntaxChecker(CommandExec *executor, QString checker)
{
  this->executor = executor;
  this->checker = checker;
}

SyntaxCheckerResult SyntaxChecker::check(QStringList args, QString input)
{
  QString wDir = ".";
  args.append(input);
  assert(executor);
  if ( ! executor->execute(wDir, checker, args) ) return false;
  QStringList outps = executor->getOutputs();
  qDebug() << "Checker output=" << outps; 
  // TODO: use regexp to parse output
  if ( outps.length() < 1 ) return SyntaxCheckerResult(false);
  QStringList res = outps.at(0).split(";", Qt::KeepEmptyParts);
  if ( res.length() != 3 ) return false;
  if ( res.at(0) != "1" ) return false;
  QStringList lhs_vars = res.at(1).split(",",Qt::SkipEmptyParts);
  QStringList rhs_vars = res.at(2).split(",",Qt::SkipEmptyParts);
  qDebug() << "Guard/action validated. LHS vars=" << lhs_vars << "RHS vars" << rhs_vars;
  return SyntaxCheckerResult(true, lhs_vars, rhs_vars);
}

SyntaxCheckerResult SyntaxChecker::check_guard(QStringList inps, QStringList outps, QStringList vars, QString guard)
{
  Q_UNUSED(outps);
  QStringList lhs_vars, rhs_vars;
  SyntaxCheckerResult r = check(QStringList("-guard"), guard);
  if ( ! r.ok ) { r.msg = "Invalid guard: \"" + guard + "\""; return r; }
  foreach ( QString rhs, r.rhs_vars ) {
    if ( ! inps.contains(rhs) && ! vars.contains(rhs) ) {
      r.msg = "The symbol \"" + rhs + "\" occuring in the guard \"" + guard + "\" is neither an input nor a variable";
      return r;
      }
    }
  return r;
}

SyntaxCheckerResult SyntaxChecker::check_action(QStringList inps, QStringList outps, QStringList vars, QString action)
{
  SyntaxCheckerResult r = check(QStringList("-action"), action);
  if ( ! r.ok ) { r.msg = "Invalid action: \"" + action + "\""; return r; }
  foreach ( QString rhs, r.rhs_vars ) {
    if ( ! inps.contains(rhs) && ! vars.contains(rhs) ) {
      r.msg = "The symbol \"" + rhs + "\" occuring in the action \"" + action + "\" is neither an input nor a variable";
      return r;
      }
    }
  foreach ( QString lhs, r.lhs_vars ) {
    if ( ! outps.contains(lhs) && ! vars.contains(lhs) ) {
      r.msg = "The symbol \"" + lhs + "\" modified in the action \"" + action + "\" is neither an output nor a variable";
      return r;
      }
    }
  return r;
}

SyntaxCheckerResult SyntaxChecker::check_valuation(QStringList outps, QString valuation)
{
  SyntaxCheckerResult r = check(QStringList("-sval"), valuation);
  if ( ! r.ok ) { r.msg = "Invalid valuation: \"" + valuation + "\""; return r; }
  foreach ( QString lhs, r.lhs_vars ) {
    if ( ! outps.contains(lhs) )  {
      r.msg = "The symbol \"" + lhs + "\" set in the valuation \"" + valuation + "\" is not an output";
      return r;
      }
    }
  return r;
}
