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
#include <QRegularExpression>

const QString Globals::version = "2.0.0"; 
const QStringList Globals::guiOnlyOpts = { "-dot_external_viewer", "-sync_externals" };
CompilerPaths *Globals::compilerPaths = NULL;
CompilerOptions *Globals::compilerOptions = NULL;
Compiler *Globals::compiler = NULL;
CommandExec *Globals::executor = NULL;
Globals::Mode Globals::mode = SelectItem;
QString Globals::initDir = ".";
QWidget *Globals::mainWindow = NULL;
//const QString Globals::defaultModelName = "main";
const QRegularExpression Globals::re_lid("[a-z][A-Za-z0-9_]*");
