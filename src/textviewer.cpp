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


#include <QtWidgets>
#include "textviewer.h"

SyntaxHighlighter* makeSyntaxHighlighter(QString suffix, QTextDocument* doc)
{
    if ( suffix == "fsm" ) return new FsmSyntaxHighlighter(doc);
    if ( suffix == "c" || suffix == "h" || suffix == "cpp" ) return new CTaskSyntaxHighlighter(doc);
    return NULL;
}

TextViewer::TextViewer(QFile& f, const QFont& font, QWidget *parent) : QPlainTextEdit(parent)
{
  QFileInfo fi(f);
  setFont(font);
  setPlainText(QString::fromUtf8(f.readAll()));
  setReadOnly(true);
  highlighter = makeSyntaxHighlighter(fi.suffix(), document());
  setProperty("attachedSyntaxHighlighter", QVariant::fromValue(static_cast<void*>(highlighter)));
}

TextViewer::~TextViewer()
{
  if ( highlighter ) delete highlighter;
}
