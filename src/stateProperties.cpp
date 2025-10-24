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
  QRegularExpressionValidator *state_name_validator = new QRegularExpressionValidator(re_uid);
  state_name_field->setValidator(state_name_validator);
  row_layout1->addWidget(name_label);
  row_layout1->addWidget(state_name_field);
  layout->addLayout(row_layout1);

  QStringList valuations = state->getAttrs();
  valuations_panel = new StateValuations("Valuations", valuations);
  layout->addWidget(valuations_panel);
  
  QHBoxLayout *row_layout2 = new QHBoxLayout();
  QPushButton *cancel_button = new QPushButton("Cancel");
  QPushButton *accept_button = new QPushButton("Done");
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
}

void StateProperties::accept()
{
  qDebug() << "StateProperties::accept";
  QString id = state_name_field->text();
  state->setId(id);
  QStringList valuations = valuations_panel->retrieve();
  bool ok = Globals::check_model ? state->check_valuations(valuations) : true;
  if ( ok ) {
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
