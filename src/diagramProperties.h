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

class Diagram;
class QLineEdit;
class QTabWidget;
class QComboBox;
class IovPanel;

class DiagramProperties : public QDialog
{
  Q_OBJECT
public:
  DiagramProperties(Diagram *diagram, QWidget *parent);
  ~DiagramProperties();

private:
  QTabWidget *diagrams;
  Diagram *diagram;
  QLineEdit* name_field;
  QComboBox *scaleCombo;
  IovPanel *vars_panel;
    
protected slots:
  void fill();
  void changeScale(const QString& scale);
  void accept();
  void cancel();
};
