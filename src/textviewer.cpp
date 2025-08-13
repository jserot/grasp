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


#include <QtWidgets>
#include "textViewer.h"
#include "textProperties.h"

QFont createDefaultFont(void)
{
 QFont font;
 font.setFamily("Monaco");
 font.setFixedPitch(true);
 font.setPointSize(12);
 return font;
}

QFont TextViewer::defaultFont = createDefaultFont();

SyntaxHighlighter* makeSyntaxHighlighter(QString suffix, QTextDocument* doc)
{
    if ( suffix == "fsm" ) return new FsmSyntaxHighlighter(doc);
    if ( suffix == "c" || suffix == "h" || suffix == "cpp" ) return new CTaskSyntaxHighlighter(doc);
    return NULL;
}

TextViewer::TextViewer(QString fname) : QPlainTextEdit()
{
  QFile file(fname);
  Q_ASSERT(file.open(QIODevice::ReadOnly | QIODevice::Text)); // This is supposed to have been checked by the caller
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  // setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  // setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setFont(defaultFont);
  QFileInfo fi(file);
  setWindowTitle(fi.fileName());
  setPlainText(QString::fromUtf8(file.readAll()));
  setReadOnly(true);
  highlighter = makeSyntaxHighlighter(fi.suffix(), document());
  setProperty("attachedSyntaxHighlighter", QVariant::fromValue(static_cast<void*>(highlighter)));
  setAttribute(::Qt::WA_DeleteOnClose);
  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu()));
}

void TextViewer::contextMenu()
{
  TextProperties *dialog = new TextProperties(this, this);
  dialog->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint | Qt::WindowSystemMenuHint);
  dialog->setAttribute(Qt::WA_DeleteOnClose);
  dialog->show();
}

QFont TextViewer::getFont()
{
  return currentFont;
}

void TextViewer::setFont(QFont font)
{
  currentFont = font;
  QPlainTextEdit::setFont(currentFont);
}

QSize TextViewer::sizeHint () const
{
  // TODO: compute size from the document contents
  // QSize s(this->document()->size().toSize());
  // qDebug() << "TextViewer::sizeHint:" << s;
  // s.rwidth() = std::max(100, s.width());
  // s.rheight() = std::max(100, s.height());
  QSize s(400, 600);
  return s;
}


TextViewer::~TextViewer()
{
  qDebug() << "TextViewer::delete";
  if ( highlighter ) delete highlighter;
}
