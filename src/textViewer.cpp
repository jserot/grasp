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
#include "viewerSize.h"

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

TextViewer::TextViewer(QString fname, QWidget *parent) : QPlainTextEdit(parent)
{
    QFile file(fname);
    if ( file.open(QIODevice::ReadOnly | QIODevice::Text) )
      setPlainText(QString::fromUtf8(file.readAll()));
    else
      setPlainText("Cannot open file " + fname);
    setFont(defaultFont);
    QFileInfo fi(file);
    setWindowFlag(Qt::Window, true);  // Required to make the window toplevel
    setWindowTitle(fi.fileName());
    setReadOnly(true);

    highlighter = makeSyntaxHighlighter(fi.suffix(), document());
    setProperty("attachedSyntaxHighlighter", QVariant::fromValue(static_cast<void*>(highlighter)));

    setAttribute(Qt::WA_DeleteOnClose);
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu()));

    // The window is sized after its content, within limits (scroll bars are shown beyond)
    setLineWrapMode(QPlainTextEdit::NoWrap);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    resize(boundedViewerSize(contentSize()));
}

QSize TextViewer::contentSize()
{
    QFontMetrics fm(font());
    int maxWidth = 0;
    for ( const QString &line : toPlainText().split('\n') )
      maxWidth = std::max(maxWidth, fm.horizontalAdvance(line));
    int margins = 2 * int(document()->documentMargin()) + 2 * frameWidth();
    // Room is left for the scroll bars, which may be needed when the size is bounded
    int width = maxWidth + margins + verticalScrollBar()->sizeHint().width() + fm.averageCharWidth();
    int height = document()->blockCount() * fm.lineSpacing() + margins + horizontalScrollBar()->sizeHint().height();
    return QSize(width, height);
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

TextViewer::~TextViewer()
{
  qDebug() << "TextViewer::delete";
  if ( highlighter ) delete highlighter;
}
