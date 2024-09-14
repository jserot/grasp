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

#ifndef _GUARD_VALIDATOR
#define _GUARD_VALIDATOR

#include <QValidator>
#include "commandExec.h"


class GuardValidator : public QValidator
{
 public:
  GuardValidator(CommandExec *executor, QString syntaxChecker);
  QValidator::State validate(QString &input, int &pos) const;
  void fixup(QString &input) const;
private:
  CommandExec *executor;
  QString checker;
};
#endif
