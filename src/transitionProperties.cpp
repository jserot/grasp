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

#include "transitionProperties.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QMessageBox>

#include "globals.h"
#include "transition.h"
#include "state.h"
#include "model.h"
#include "transitionGuards.h"
#include "transitionActions.h"
#include "compiler.h"

TransitionProperties::TransitionProperties(
  Transition *transition, Diagram *diagram, bool isInitial, QWidget *parent)
  : QDialog(parent)
{
  setWindowTitle("Transition");

  QVBoxLayout *layout = new QVBoxLayout();

  QGridLayout* layout1 = new QGridLayout();

  QLabel *start_state_label = new QLabel("Start State");
  start_state_field = new QComboBox();
  layout1->addWidget(start_state_label, 0, 0, 1, 3);
  layout1->addWidget(start_state_field, 1, 0, 1, 3);

  QLabel *end_state_label = new QLabel("End State");
  end_state_field = new QComboBox();
  layout1->addWidget(end_state_label, 0, 3, 1, 3);
  layout1->addWidget(end_state_field, 1, 3, 1, 3);

  QLabel *event_label = new QLabel("Event");
  event_field = new QComboBox();
  layout1->addWidget(event_label, 2, 0, 1, 2);
  layout1->addWidget(event_field, 2, 2, 1, 4);

  layout->addLayout(layout1);

  QStringList guards = transition->getGuards();
  guards_panel = new TransitionGuards("Guards", guards);
  layout->addWidget(guards_panel);
  
  QStringList actions = transition->getActions();
  actions_panel = new TransitionActions("Actions", actions);
  layout->addWidget(actions_panel);
  
  QHBoxLayout *layout2 = new QHBoxLayout();
  QPushButton *cancel_button = new QPushButton("Cancel");
  QPushButton *accept_button = new QPushButton("Done");
  layout2->addWidget(cancel_button);
  layout2->addWidget(accept_button);
  layout->addLayout(layout2);
  cancel_button->setDefault(false);
  accept_button->setDefault(true);

  if ( isInitial ) {
    start_state_label->hide();
    start_state_field->hide();
    event_label->hide();
    event_field->hide();
    guards_panel->hide();
    }

  // Fill fields
  start_state_field->clear();
  end_state_field->clear();
  if ( isInitial ) {
    foreach ( State* state, diagram->states() ) {
      if ( ! state->isPseudo() ) {
          QString id = state->getId();
          end_state_field->addItem(id, QVariant(id));
          if ( transition->getDstState()->getId() == id ) 
            end_state_field->setCurrentIndex(end_state_field->count()-1);
          }
        }
      }
  else {
    foreach ( State* state, diagram->states() ) {
      if ( ! state->isPseudo() ) {
        QString id = state->getId();
        start_state_field->addItem(id, QVariant(id));
        end_state_field->addItem(id, QVariant(id));
        if ( transition->getSrcState()->getId() == id )
          start_state_field->setCurrentIndex(start_state_field->count()-1);
        if ( transition->getDstState()->getId() == id ) 
          end_state_field->setCurrentIndex(end_state_field->count()-1);
      }
    }
  }
  event_field->clear();
  QStringList inpEvents = diagram->enclosingModel()->getInpEvents();
  QStringList sharedEvents = diagram->enclosingModel()->getSharedEvents();
  for ( auto ev: inpEvents + sharedEvents ) 
    event_field->addItem(ev, QVariant(ev));
  QString event = transition->getEvent();
  if ( event == "" ) {
    event_field->setCurrentIndex(0);
    transition->setEvent(event);
    }
  else {
    if ( inpEvents.contains(event) || sharedEvents.contains(event) ) 
      event_field->setCurrentText(event);
    else
      QMessageBox::warning( this, "Error", "The triggering event for this transition is not listed in the model inputs");
      }

  setLayout(layout);

  connect(cancel_button, &QPushButton::clicked, this, &TransitionProperties::cancel);
  connect(accept_button, &QPushButton::clicked, this, &TransitionProperties::accept);

  Q_ASSERT(Globals::mainWindow);
  connect(this, SIGNAL(modelModified()), Globals::mainWindow, SLOT(modelModified()));

  this->transition = transition;
  this->diagram = diagram;
}

void TransitionProperties::accept()
{
  State *srcState, *dstState;
  QString event;
  bool isInitial = transition->isInitial();
  
  if ( ! isInitial ) {
    srcState = diagram->getState(start_state_field->currentText());
    Q_ASSERT(srcState);
    }

  dstState = diagram->getState(end_state_field->currentText());
  Q_ASSERT(dstState);

  if ( ! isInitial ) 
    event = event_field->currentText();

  QStringList guards = guards_panel->retrieve();
  QStringList actions = actions_panel->retrieve();

  bool ok;
  if ( Globals::check_model ) {
    ok = isInitial ? true : transition->check_guards(guards);
    ok = ok && transition->check_actions(actions);
    }
  else
    ok = true;

  if ( ok  ) {
    transition->setDstState(dstState);
    transition->setActions(actions);
    if ( ! isInitial ) {
      transition->setSrcState(srcState);
      transition->setEvent(event);
      transition->setGuards(guards);
    }
    qDebug() << "TransitionProperties::accept(ok)";
    update();
    emit modelModified(); // To main window
    QDialog::done(Accepted);
    }
  else {
    qDebug() << "TransitionProperties::accept(nok)";
    // Do not accept and leave dialog opened
    // TODO: check this ! 
  }
}

void TransitionProperties::cancel()
{
  QDialog::done(Rejected);
}

TransitionProperties::~TransitionProperties()
{
}
