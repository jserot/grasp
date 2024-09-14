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

#include "fragmentChecker.h"
#include "model.h"
#include "automaton.h"
#include "compiler.h"
#include <QTemporaryFile>
#include <QMessageBox>
#include <QtDebug>
#include "qt_compat.h"

FragmentChecker::FragmentChecker(Compiler *compiler, Automaton *automaton, QWidget *parent)
{
  this->parent = parent;
  this->compiler = compiler;
  this->automaton = automaton;
}

bool FragmentChecker::check(QString kind, QString frag)
{
  // Build fragment file
  QTemporaryFile file;
  //file.setAutoRemove(false); // For debug only
  if ( ! file.open() ) return false;
  QString fname = file.fileName();
  //qDebug() << "Temporary file name is" << fname;
  QTextStream os(&file);
  os << "-- context" << QT_ENDL;
  foreach ( Iov* iov, automaton->enclosingModel()->getIos() ) {
    os << Iov::stringOfKind(iov->kind) << " " << iov->name << ": " << Iov::stringOfType(iov->type) << ";" << QT_ENDL;
  }
  foreach ( Iov* iov, automaton->getVars() ) {
    os << Iov::stringOfKind(iov->kind) << " " << iov->name << ": " << Iov::stringOfType(iov->type) << ";" << QT_ENDL;
  }
  os << "-- fragment" << QT_ENDL;
  os << kind<< " " << frag << ";" << QT_ENDL;
  file.close();
  QStringList args = { "-check_fragment" };
  // Call compiler
  return compiler->run(fname, args, ".");
  // TODO : add a "transition" fragment class and let the compiler return (on stdout ?), in this case,
  // the list of of IOs and variables read (resp. written) by the guards and actions
  // More generally, implement a "server" mode for the rfsmc compiler, allowing it to be used for
  // checking / extracting semantic informations from syntax fragments 
}

bool FragmentChecker::check_state_valuation(QString valuation)
{
  return check("sval", valuation);
}

bool FragmentChecker::check_guard(QString guard)
{
  return check("guard", guard);
}

bool FragmentChecker::check_action(QString action)
{
  return check("action", action);
}

QStringList FragmentChecker::getErrors() 
{
  return compiler->getErrors();
}
  
