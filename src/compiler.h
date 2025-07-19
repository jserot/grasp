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
#pragma once

#include <QString>
#include <QProcess>

class CommandExec;

class Compiler : QObject
{
  Q_OBJECT

  // static const QString name;
  
public:
  Compiler(QString path);
  ~Compiler();

  void setPath(QString path);
  
  bool run(QString srcFile, QStringList args, QString wDir);
  QStringList getOutputs();
  QStringList getErrors();
  QStringList getOutputFiles(QString target, QString wDir, QString modelName);
private:
  QString path;
  CommandExec* executor;
};
