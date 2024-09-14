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

#include "actionValidator.h"

ActionValidator::ActionValidator()
{
}

QValidator::State ActionValidator::validate(QString &input, int &pos) const
{
  Q_UNUSED(input);
  Q_UNUSED(pos);
  return QValidator::Acceptable;
}

void ActionValidator::fixup(QString &input) const
{
  Q_UNUSED(input);
  // Nothing
}
