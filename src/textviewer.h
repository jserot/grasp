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

#include <QPlainTextEdit>
#include "syntaxHighlighters.h"

class TextViewer : public QPlainTextEdit
{
  Q_OBJECT

public:
  TextViewer(QFile& file, const QFont& font, QWidget *parent = 0);
  ~TextViewer();

private:
  SyntaxHighlighter* highlighter;
};

