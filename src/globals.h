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

#include <QWidget>
#include <QString>
#include <QStringList>

class CompilerPaths;
class CompilerOptions;
class Compiler;
class CommandExec;

class Globals
{
public:
    enum Mode {
      InsertState,
      InsertPseudoState,
      InsertTransition,
      InsertLoopTransition,
      SelectItem,
      DeleteItem
      };
    static Mode mode;
    static CompilerPaths *compilerPaths;
    static CompilerOptions *compilerOptions;
    static Compiler *compiler; // For calling the rfsmc compiler
    static CommandExec *executor; // For calling externals commands
    static QString initDir;
    const static QString version;
    const static QStringList guiOnlyOpts;
    static QWidget *mainWindow;
    // const static QString defaultModelName;
    static const QRegularExpression re_lid;
};

