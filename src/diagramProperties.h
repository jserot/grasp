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
class QLabel;
class QPushButton;
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
  static const QString fitScale; // Scale entry for which the whole diagram is visible
  QTabWidget *diagrams;
  Diagram *diagram;
  QLineEdit* name_field;
  QLabel* name_error; // Error of the name field, shown under it
  QPushButton* accept_button;

  QString nameError(const QString &id) const; // Empty if [id] is a valid name for the diagram
  QComboBox *scaleCombo;
  IovPanel *vars_panel;
    
protected slots:
  void fill();
  void changeScale(const QString& scale);
  void accept();
  void cancel();
  void validateName(); // Updates the name error and the [Done] button as the name is typed
};
