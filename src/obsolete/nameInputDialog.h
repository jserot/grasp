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

class QLineEdit;

class NameInputDialog : public QDialog
{
  Q_OBJECT
public:
  NameInputDialog(QString initText, QWidget *parent);
  ~NameInputDialog();

  QString getResult() { return result; }

private:
  QLineEdit* text;
  QString result;
    
protected slots:
  void accept();
  void cancel();
};
