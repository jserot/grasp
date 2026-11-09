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

#include "stateProperties.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QRegularExpressionValidator>
#include <QMessageBox>
#include <QtDebug>

#include "globals.h"
#include "state.h"
#include "model.h"
#include "stateValuations.h"
#include "compiler.h"
#include "fieldValidation.h"

const QRegularExpression StateProperties::re_uid("[A-Z][A-Za-z0-9_]*");

StateProperties::StateProperties(State *state, Diagram *diagram, QWidget *parent)
  : QDialog(parent)
{
  QString id = state->getId();
  setWindowTitle("State " + id);
  QVBoxLayout *layout = new QVBoxLayout();

  QHBoxLayout *row_layout1 = new QHBoxLayout();
  QLabel* name_label = new QLabel("Name");
  state_name_field = new QLineEdit();
  // state_name_field->setPlaceholderText("State identifier");
  state_name_field->setText(id);
  QRegularExpressionValidator *state_name_validator = new QRegularExpressionValidator(re_uid, this);
  state_name_field->setValidator(state_name_validator);
  row_layout1->addWidget(name_label);
  row_layout1->addWidget(state_name_field);
  layout->addLayout(row_layout1);
  name_error = newFieldErrorLabel();
  layout->addWidget(name_error);

  QStringList valuations = state->getAttrs();
  valuations_panel = new StateValuations("Valuations", valuations);
  layout->addWidget(valuations_panel);
  
  QHBoxLayout *row_layout2 = new QHBoxLayout();
  QPushButton *cancel_button = new QPushButton("Cancel");
  accept_button = new QPushButton("Done");
  cancel_button->setDefault(false);
  accept_button->setDefault(true);
  row_layout2->addWidget(cancel_button);
  row_layout2->addWidget(accept_button);
  layout->addLayout(row_layout2);

  setLayout(layout);

  connect(cancel_button, &QPushButton::clicked, this, &StateProperties::cancel);
  connect(accept_button, &QPushButton::clicked, this, &StateProperties::accept);

  Q_ASSERT(Globals::mainWindow);
  connect(this, SIGNAL(modelModified()), Globals::mainWindow, SLOT(modelModified()));

  this->state = state;
  this->diagram = diagram;

  connect(state_name_field, &QLineEdit::textChanged, this, &StateProperties::validateName);
  validateName();
}

QString StateProperties::nameError(const QString &id) const
{
  if ( id.isEmpty() || ! re_uid.match(id).hasMatch() || re_uid.match(id).capturedLength() != id.length() )
    return "Invalid state name. It must start with an uppercase letter, followed by letters, digits or _";
  State *other = diagram->getState(id);
  if ( other && other != state )
    return "The name " + id + " is already used by another state of this diagram";
  return QString();
}

void StateProperties::validateName()
{
  QString error = nameError(state_name_field->text().trimmed());
  showFieldError(state_name_field, name_error, error);
  accept_button->setEnabled(error.isEmpty());
}

void StateProperties::accept()
{
  qDebug() << "StateProperties::accept";
  Globals::compiler->resetReports();
  QString id = state_name_field->text().trimmed();
  // The state is left unchanged (and the dialog opened) if its new name or valuations are invalid
  if ( ! nameError(id).isEmpty() ) { // Normally not reached: [Done] is disabled while the name is invalid
    validateName();
    return;
    }
  QStringList valuations = valuations_panel->retrieve();
  bool ok = Globals::check_model ? state->check_valuations(valuations) : true;
  if ( ok ) {
    state->setId(id);
    state->setAttrs(valuations);
    qDebug() << "StateProperties::accept(ok)";
    update();
    emit modelModified(); // To main window
    QDialog::done(Accepted);
    }
  else {
    qDebug() << "StateProperties::accept(nok)";
    // Do not accept and leave dialog opened
    // TODO: check this ! 
  }
}

void StateProperties::cancel()
{
  qDebug() << "StateProperties::cancel";
  QDialog::done(Rejected);
}

StateProperties::~StateProperties()
{
}
