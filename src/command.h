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

#include <QString>

class CommandLine
{
public:
  QString cmd;
  QString args;

  CommandLine(QString cmd_, QString args_ = "") : cmd(cmd_), args(args_) { }
  CommandLine() : cmd(""), args("") { }
  ~CommandLine() { }

  QString toString(void);
};

