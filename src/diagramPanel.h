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

#include <QWidget>
#include <QPointer>

#include "diagram.h" // Needed by QPointer<Diagram>
class QLineEdit;
class QLabel;
class QVBoxLayout;
class IovPanel;

// Properties of the current diagram (name, local variables), shown in the "Diagram" dock window.
// The panel follows the current diagram tab (see MainWindow::currentDiagramChanged); its fields are applied as
// soon as they are edited (as for the model IOs).
class DiagramPanel : public QWidget
{
  Q_OBJECT

public:
  explicit DiagramPanel(QWidget *parent = nullptr);

  void setDiagram(Diagram *diagram); // nullptr if there's no current diagram
  Diagram *getDiagram() const { return diagram; }
  void focusName(); // Gives the focus to the name field (to rename a new diagram, for ex.)

private:
  QPointer<Diagram> diagram;
  QVBoxLayout *layout;
  QLabel *no_diagram_label;
  QWidget *content = nullptr; // Fields of the current diagram, rebuilt for each diagram
  QLineEdit *name_field = nullptr;
  QLabel *name_error = nullptr;
  IovPanel *vars_panel = nullptr;

  void clearContent(bool validatePendingEdit);
  QString nameError(const QString &name) const;
  void validateName();
  void applyName();
};
