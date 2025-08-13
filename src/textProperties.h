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

#include <QDialog>

class TextViewer;
class QFontComboBox;
class QComboBox;

class TextProperties : public QDialog
{
  Q_OBJECT

public:
  TextProperties(TextViewer *viewer, QWidget *parent = NULL);
  ~TextProperties();

private:
  TextViewer *viewer;
  QFontComboBox *fontCombo;
  QComboBox *fontSizeCombo;
  QPushButton *close_button;
  QFont font;
  int fontSize;
                           
protected slots:
  void fontChanged();
  void close();
  // void accept();
  // void cancel();
};
