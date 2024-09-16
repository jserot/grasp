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

#include "compiler.h"

#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QMessageBox>
#include "commandExec.h"

// static const Compiler::QString name = "rfsmc";

Compiler::Compiler(QString path)
{
  this->path = path;
  executor = new CommandExec();
}

Compiler::~Compiler()
{
  delete executor;
}

void Compiler::setPath(QString path)
{
  this->path = path;
}

bool Compiler::run(QString sFname, QStringList args, QString wDir)
{
  return executor->execute(wDir, path, args << "-gui" << sFname);
}

QStringList Compiler::getOutputs()
{
  return executor->getOutputs();
}

QStringList Compiler::getErrors()
{
  return executor->getErrors();
}

QStringList Compiler::getOutputFiles(QString target, QString wDir, QString modelName)
{
  QString rfile = wDir + "/rfsm.output";
  QFile ff(rfile);
  QStringList res;
  qDebug() << "Output files: rfile=" << rfile;
  if ( ! ff.exists() ) {
      QMessageBox::warning(NULL, "", "Compiler cannot open file " + rfile);
      return res;
    }
  ff.open(QIODevice::ReadOnly | QIODevice::Text);
  if ( target == "sim" )
    res.append(wDir+"/"+modelName+".vcd");  
  else {
    QTextStream is(&ff);
    while( ! is.atEnd() ) {
      QString of = is.readLine(); // One file per line
      QFileInfo f(of);
      if (  (target == "systemc" && (f.suffix() == "cpp" || f.suffix() == "h"))
            || (target == "vhdl" && f.suffix() == "vhd")
            || (target == "ctask" && f.suffix() == "c")
            || (target == "dot" && f.suffix() == "dot") 
            || (target == "sim" && f.suffix() == "vcd") )
        res.append(wDir+"/"+of);
      }
    }
  ff.close();
  qDebug() << "Output files: " << res;
  return res;
}
