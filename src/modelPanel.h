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

#include <QComboBox>
#include <QFrame>
#include "iovPanel.h"

class MainWindow;
class State;
class Transition;
class Model;
class ModelIovs;
class QLineEdit;
class QGroupBox;

class ModelPanel : public QFrame
{
  Q_OBJECT
private:
  MainWindow* main_window;
  Model *model;

  QGroupBox* name_panel;
  QLineEdit* model_name_field;
  IovPanel *inps_panel;
  IovPanel *outps_panel;
  IovPanel *vars_panel;
  
public:
  explicit ModelPanel(Model *model, MainWindow* parent);
  ~ModelPanel();

  QSize sizeHint() const { return QSize(275,300); }; 

signals:
  void modelModified();
  
public slots:
  void setModelName();
  void fillModelName();
  void clearModelName();
  void fillIovPanel();
  void clearIovPanel();
  void clear();
  void fill();
  void update();

  void modelUpdated();
  void dumpModel(); // for debug only

private:
  static QRegularExpressionValidator *io_name_validator;
};
