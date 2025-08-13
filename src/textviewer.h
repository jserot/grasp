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

#include <QPlainTextEdit>
#include "syntaxHighlighters.h"

class TextViewer : public QPlainTextEdit
{
  Q_OBJECT
public:
  TextViewer(QString fname);
  ~TextViewer();

  static QFont defaultFont;
  void setFont(QFont font);
  QFont getFont();

protected:
  virtual QSize sizeHint () const override;

protected slots:
  void contextMenu();
private:
  QFont currentFont;
  SyntaxHighlighter* highlighter;
};

