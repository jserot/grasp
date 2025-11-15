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

    // Ajuste la largeur au contenu
    QFontMetrics fm(font());
    QStringList lines = toPlainText().split('\n');
    int maxWidth = 0;
    for (const QString &line : lines) {
        maxWidth = std::max(maxWidth, fm.horizontalAdvance(line));
    }
    maxWidth += 20;

    // Largeur auto, hauteur variable (scroll activé si nécessaire)
    setMinimumWidth(maxWidth);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
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
