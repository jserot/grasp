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

class ModelProperties : public QFrame
{
  Q_OBJECT
private:
  Model *model; // The edited model

  IovPanel *inps_panel;
  IovPanel *outps_panel;
  IovPanel *vars_panel;
  
public:
  explicit ModelProperties(Model *model, QWidget* parent);
  ~ModelProperties();

  QSize sizeHint() const { return QSize(275,300); }; 

signals:
  void modelModified();
  
public slots:
  void clear();
  void fill();
  void update();
};
