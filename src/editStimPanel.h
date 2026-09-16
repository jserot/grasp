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

class Model;
class IovPanel;

class EditStimPanel : public QDialog
{
  Q_OBJECT
public:
  EditStimPanel(Model *model, QWidget *parent);
  ~EditStimPanel();

private:
  Model *model;
  //QLineEdit* name_field;
  //QComboBox *scaleCombo;
  IovPanel *stim_panel;
    
protected slots:
  void fill();
  void accept();
  void cancel();
};
