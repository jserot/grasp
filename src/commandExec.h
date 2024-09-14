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
#include <QProcess>

class CommandExec : QObject
{
  Q_OBJECT
public:
  CommandExec();
  ~CommandExec();

  bool execute(QString wDir, QString cmd, QStringList args, bool detach = false);
  QStringList getOutputs();
  QStringList getErrors();
private slots:
  void readStdout();
  void readStderr();
private:
  QProcess proc; 
  QString outputs;
  QString errors;
};

