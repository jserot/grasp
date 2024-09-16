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

#include "dynamicPanel.h"
#include "iov.h"
#include "rowDesc.h"

class Model;
class Automaton;
class QRegularExpressionValidator;

class IovPanel : public DynamicPanel
{
  Q_OBJECT

public:

  enum ClientKind { IcModel, IcAutomaton };
  typedef struct { // Iov panels are used both for models and model automatons
    ClientKind icKind;
    union { Model *model; Automaton *automaton; } icClient;
    } Client;

  IovPanel(Iov::IoKind kind, QString title, QString rowPrefix, Client& client, QRegularExpressionValidator *name_validator);
  ~IovPanel();


  QStringList retrieve(); // Not used here

signals:
  void modelModified();
  
private:
  Iov::IoKind kind; // Input, output or variable
  Client client;
  QRegularExpressionValidator *name_validator;
  QHash<QWidget*,RowDesc*> widgetToRow; 
  QString rowPrefix;

  void updateTypeChoices(RowDesc *row_desc);
  void updateStimChoices(RowDesc *row_desc);  // For inputs only

protected slots:
  void addRowFields(QHBoxLayout *row_layout, void *data);
  void deleteRowFields(QHBoxLayout *row_layout);
  void nameChanged();
  void nameEdited();
  void typeEdited();
  void stimEdited();
};
