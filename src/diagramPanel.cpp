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

#include "diagramPanel.h"

#include <QApplication>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QRegularExpression>
#include <QSignalBlocker>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QtDebug>

#include "diagram.h"
#include "fieldValidation.h"
#include "globals.h"
#include "iov.h"
#include "iovPanel.h"
#include "model.h"

DiagramPanel::DiagramPanel(QWidget *parent) : QWidget(parent)
{
  layout = new QVBoxLayout(this);
  layout->setAlignment(Qt::AlignTop);
  no_diagram_label = new QLabel("<i>No diagram</i>");
  layout->addWidget(no_diagram_label);
}

void DiagramPanel::clearContent(bool validatePendingEdit)
{
  if ( ! content ) return;
  // A name being edited is applied when its field loses the focus (editingFinished) : this must be done while the
  // diagram still exists, before the field is deleted
  if ( validatePendingEdit )
    if ( QWidget *w = QApplication::focusWidget(); w && content->isAncestorOf(w) ) w->clearFocus();
  delete content; // Also deletes the fields and the variable panel
  content = nullptr;
  name_field = nullptr;
  name_error = nullptr;
  vars_panel = nullptr;
}

void DiagramPanel::setDiagram(Diagram *new_diagram)
{
  if ( new_diagram == diagram && content ) return;
  if ( diagram ) disconnect(diagram, &QObject::destroyed, this, nullptr);
  clearContent(true);
  diagram = new_diagram;
  no_diagram_label->setVisible(diagram == nullptr);
  if ( ! diagram ) return;

  // Should the diagram be deleted while being shown, its fields (which refer to its variables) are removed at once
  connect(diagram, &QObject::destroyed, this, [this]() { clearContent(false); no_diagram_label->show(); });

  content = new QWidget();
  QVBoxLayout *content_layout = new QVBoxLayout(content);
  content_layout->setContentsMargins(0, 0, 0, 0);

  QFormLayout *form = new QFormLayout();
  name_field = new QLineEdit(diagram->getName());
  form->addRow("Name", name_field);
  content_layout->addLayout(form);
  name_error = newFieldErrorLabel();
  content_layout->addWidget(name_error);

  IovPanel::Client client = { IovPanel::IcDiagram, { .diagram = diagram } };
  vars_panel = new IovPanel(IoKind::IoVar, "Local variables", "Local variable", client);
  for ( Iov *var: diagram->getVars() ) vars_panel->addRow((void *)(var));
  content_layout->addWidget(vars_panel);

  layout->addWidget(content);

  connect(name_field, &QLineEdit::textEdited, this, &DiagramPanel::validateName);
  connect(name_field, &QLineEdit::editingFinished, this, &DiagramPanel::applyName);
}

void DiagramPanel::focusName()
{
  if ( ! name_field ) return;
  name_field->setFocus();
  name_field->selectAll();
}

QString DiagramPanel::nameError(const QString &name) const
{
  // Diagram names are used as identifiers in the generated RFSM and DOT code
  static const QRegularExpression re_id("^[A-Za-z][A-Za-z0-9_]*$");
  QString error;
  if ( ! re_id.match(name).hasMatch() )
    error = "Invalid diagram name. It must start with a letter, followed by letters, digits or _";
  else if ( diagram->enclosingModel()->isDiagramNameUsed(name, diagram) )
    error = "The name " + name + " is already used by another diagram";
  if ( ! error.isEmpty() )
    error += ". If it is not changed, the previous name (" + diagram->getName() + ") will be restored";
  return error;
}

void DiagramPanel::validateName()
{
  if ( ! diagram || ! name_field ) return;
  showFieldError(name_field, name_error, nameError(name_field->text().trimmed()));
}

void DiagramPanel::applyName()
{
  if ( ! diagram || ! name_field ) return;
  QString name = name_field->text().trimmed();
  showFieldError(name_field, name_error, QString());
  if ( ! nameError(name).isEmpty() ) { // An invalid name is not applied : the previous one is restored
    name_field->setText(diagram->getName());
    return;
    }
  if ( name == diagram->getName() ) return;
  qDebug() << "DiagramPanel: renaming diagram" << diagram->getName() << "to" << name;
  diagram->setName(name);
  int index = Globals::diagrams->indexOf(diagram->getView());
  if ( index >= 0 ) Globals::diagrams->setTabText(index, name);
  emit diagram->modelModified(); // To main window
}
