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
#include "fragmentChecker.h"

TransitionProperties::TransitionProperties(
  Transition *transition, Automaton *automaton, bool isInitial, QWidget *parent)
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
    foreach ( State* state, automaton->states() ) {
      if ( ! state->isPseudo() ) {
          QString id = state->getId();
          end_state_field->addItem(id, QVariant(id));
          if ( transition->getDstState()->getId() == id ) 
            end_state_field->setCurrentIndex(end_state_field->count()-1);
          }
        }
      }
  else {
    foreach ( State* state, automaton->states() ) {
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
  QStringList inpEvents = automaton->enclosingModel()->getInpEvents();
  QStringList sharedEvents = automaton->enclosingModel()->getSharedEvents();
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

  this->transition = transition;
  this->automaton = automaton;
}

void TransitionProperties::accept()
{
  State *srcState, *dstState;
  QString event;
  QStringList guards;
  QStringList actions;
  
  bool isInitial = transition->isInitial();
  
  if ( ! isInitial ) {
    srcState = automaton->getState(start_state_field->currentText());
    Q_ASSERT(srcState);
    }

  dstState = automaton->getState(end_state_field->currentText());
  Q_ASSERT(dstState);

  if ( ! isInitial ) 
    event = event_field->currentText();

  FragmentChecker checker(Globals::compiler,automaton,this);

  bool guards_ok = true;
  if ( ! isInitial ) {
    guards = guards_panel->retrieve();
    foreach ( QString guard, guards) {
      if ( ! checker.check_guard(guard) ) {
        QStringList errors = checker.getErrors();
        QMessageBox::warning(this, "", "Illegal guard: \"" + guard + "\"\n" + errors.join("\n"));
        guards_ok = false;
        }
      }
  // if ( guards.length() >= 2 ) {
  //   for ( int i = 0; i<guards.length(); i++ ) {
  //     QString guard = guards.at(i);
  //     guards.replace(i, "(" + guard + ")");
  //   }
  // }
    }
 
  bool actions_ok = true;
  actions = actions_panel->retrieve();
  foreach ( QString action, actions) {
    // First check action is well-formed and typed
    if ( ! checker.check_action(action) ) {
      QStringList errors = checker.getErrors();
      QMessageBox::warning(this, "", "Illegal action: \"" + action + "\"\n" + errors.join("\n"));
      actions_ok = false;
      continue;
      }
    // Then, if the above succeeded, test that an output modified by an action is not assigned in the target state 
      QString lhs = action.split(":=").at(0);
      QStringList ovs = dstState->getAttrs();
      for ( int i = 0; i<ovs.length(); i++ ) { // It's a pity QList does not have a [map] operator ..
        QString lhs = ovs.at(i).split("=").at(0);
        ovs.replace(i, lhs);
        }
      if ( ovs.contains(lhs) ) {
        QMessageBox::warning(this, "", "Action \"" + action + "\" sets output \"" + lhs + "\", which is already assigned in the target state"); 
        actions_ok = false;
        break;
        }
    }
    // foreach ( QString v, r.lhs_vars ) {
    //   if ( lhss.contains(v) ) { // Assignation of an already assigned output/var 
    //     QMessageBox::warning(this, "Error", "The output/variable " + v + " is assigned several times by the actions");
    //     actions_ok = false;
    //     }
    //   else
    //     lhss.insert(v);
    //   }
    // }
  if ( guards_ok && actions_ok ) {
    transition->setDstState(dstState);
    transition->setActions(actions);
    if ( ! isInitial ) {
      transition->setSrcState(srcState);
      transition->setEvent(event);
      transition->setGuards(guards);
    }
    QDialog::done(Accepted);
    }
  else {
    // Do not accept and leave dialog opened
  }
}

void TransitionProperties::cancel()
{
  QDialog::done(Rejected);
}

TransitionProperties::~TransitionProperties()
{
}
