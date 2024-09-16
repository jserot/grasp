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

#include "commandExec.h"
#include "qt_compat.h"
#include <QDebug>

CommandExec::CommandExec()
{
  connect(&proc, SIGNAL(readyReadStandardOutput()), this, SLOT(readStdout()));
  connect(&proc, SIGNAL(readyReadStandardError()), this, SLOT(readStderr()));
}

bool CommandExec::execute(QString wDir, QString cmd, QStringList args, bool detach)
{
  qDebug() << "CommandExec::execute: wDir=" << wDir << " cmd=" << cmd << " args=" << args;
  outputs.clear();
  errors.clear();
  proc.setWorkingDirectory(wDir);
  if ( detach )
    proc.startDetached(cmd,args);
  else
    proc.start(cmd,args);
  if ( proc.error() == QProcess::FailedToStart ) {
    qDebug() << "CommandExec: failed to start" << QT_ENDL;
    return false;
    }
  // qDebug() << "CommandExec: sync process launched. Waiting for termination";
  if ( detach ) 
    return true;
  else {
    bool r = proc.waitForFinished(); 
    QProcess::ExitStatus s = proc.exitStatus();
    int o = proc.exitCode();
    qDebug() << "CommandExec: exit status/code=" << s << "," << o;
    // qDebug() << "CommandExec: stdout=" << outputs;
    // qDebug() << "CommandExec: stderr=" << errors;
    proc.kill();
    proc.close();
    proc.terminate();
    return r && s == QProcess::NormalExit && o == 0;
    }
}

QStringList CommandExec::getOutputs()
{
  return outputs.split("\n", SKIP_EMPTY_PARTS);
}

QStringList CommandExec::getErrors()
{
  return errors.split("\n", SKIP_EMPTY_PARTS);
}

void CommandExec::readStdout()
{
  proc.setReadChannel(QProcess::StandardOutput);
  outputs = "";
  while (proc.canReadLine ()) {
    outputs += QString(proc.readLine());
      }
}

void CommandExec::readStderr()
{
  proc.setReadChannel ( QProcess::StandardError );
  errors = "";
  while (proc.canReadLine ()) 
    errors += QString(proc.readLine());
}

CommandExec::~CommandExec()
{
}
