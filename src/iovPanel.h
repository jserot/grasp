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
#include <memory>
#include <vector>

class Model;
class Diagram;
class QRegularExpressionValidator;

class IovPanel : public DynamicPanel
{
  Q_OBJECT

public:

  enum ClientKind { IcModel, IcDiagram };// Iov panels are used both for models and model diagrams
  typedef struct {
    ClientKind icKind;
    union { Model *model; Diagram *diagram; } icClient;
    } Client;

  IovPanel(IoKind kind, QString title, QString rowPrefix, Client client);
  ~IovPanel();

  QStringList retrieve() override; // Not used here

protected:
  QString rowDeletionError(QHBoxLayout *row_layout) override;
  void stimuliEdited() override;

private:
  IoKind kind; // Input, output or variable
  Client client;
  std::vector<std::unique_ptr<RowDesc>> rows; // Owns the row descriptors
  QHash<QWidget*,RowDesc*> widgetToRow; 
  QString rowPrefix;

  void updateTypeChoices(RowDesc *row_desc);
  void updateStimButton(RowDesc *row_desc);
  QStringList transitionsTriggeredBy(const QString &event); // In all the diagrams of the model
  QString usedEventError(const QString &event, const QString &what);

protected slots:
  void addRowFields(QHBoxLayout *row_layout, void *data) override;
  void deleteRowFields(QHBoxLayout *row_layout) override;
  void nameChanged();
  void nameEdited();
  void typeEdited();

private:
  QRegularExpressionValidator *name_validator;
};
