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

#include "globals.h"
#include "compilerPaths.h"
#include "compilerOptions.h"
#include "compiler.h"
#include "commandExec.h"
#include "model.h"
#include <QRegularExpression>

const QString Globals::version = "1.0.1"; 
const QStringList Globals::guiOnlyOpts = { "-dot_external_viewer", "-no_model_check" };
CompilerPaths *Globals::compilerPaths = NULL;
CompilerOptions *Globals::compilerOptions = NULL;
Compiler *Globals::compiler = NULL;
CommandExec *Globals::executor = NULL;
Globals::Mode Globals::mode = SelectItem;
QString Globals::initDir = ".";
QWidget *Globals::mainWindow = NULL;
QTabWidget* Globals::diagrams = NULL; 
const QRegularExpression Globals::re_lid("[a-z][A-Za-z0-9_]*");
bool Globals::check_model = true; 
bool Globals::traceMode = true;
const QString Globals::traceFileName = "grasp.log";
QFile *Globals::traceFile = NULL;
