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
#include <QRegularExpression>

class State;
class Diagram;
class QLineEdit;
class QLabel;
class QPushButton;
class StateValuations;
class Compiler;

class StateProperties : public QDialog
{
  Q_OBJECT

  static const QRegularExpression re_uid;

public:
  StateProperties(State *state, Diagram *diagram, QWidget *parent);
  ~StateProperties();

private:
  State *state;
  Diagram *diagram;

  QLineEdit* state_name_field;
  QLabel* name_error; // Error of the name field, shown under it
  QPushButton* accept_button;

  QString nameError(const QString &id) const; // Empty if [id] is a valid name for the state
  StateValuations *valuations_panel;

signals:
    void modelModified(void);
    
protected slots:
  void accept();
  void cancel();
  void validateName(); // Updates the name error and the [Done] button as the name is typed
};
