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

#ifndef _ACTION_VALIDATOR
#define _ACTION_VALIDATOR

#include <QValidator>

class ActionValidator : public QValidator
{
 public:
  ActionValidator();
  QValidator::State validate(QString &input, int &pos) const;
  void fixup(QString &input) const;
};
#endif
