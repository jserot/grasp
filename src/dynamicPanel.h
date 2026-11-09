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

#include <QGroupBox>
#include <QMap>
#include "iov.h"

class QPushButton;
class QBoxLayout;
class QVBoxLayout;
class QHBoxLayout;
class QLabel;
class QLineEdit;
class Model;

class DynamicPanel : public QGroupBox
{
  Q_OBJECT

  // This is used to pass client-specific data to the [DynamicPanel] object.
  // For ex, when creating a panel display global inputs, we want to add a button for editing the associated stimuli
public:
  enum CustomTag { CtOther, CtInputs }; // To be extended 
  typedef struct {
    CustomTag ctTag;
    union { Model *model; } ctData; // For inputs, pass the enclosing model
    } Client;

protected:
  QVBoxLayout *layout; // Top row (buttons), then one row per item
  QLabel *error_label; // Under the rows: error of the field being edited, if any (see fieldValidation.h)
  QPushButton *add_button;
  QPushButton *clear_button;
  QHash<QPushButton*, QBoxLayout*> mButtonToLayoutMap;
  Client client;

  void delete_row(QHBoxLayout *row_layout);

public:
  DynamicPanel(QString title, Client client = { .ctTag = CtOther });
  ~DynamicPanel();

  // For panels whose rows begin with a text field (guards, actions, valuations, ...)
  QList<QLineEdit*> rowFields() const; // The text field of each row
  void showRowErrors(const QList<QLineEdit*> &fields, const QStringList &errors);
    // Marks [fields] (red frame) and shows [errors] under the rows; the previous errors are cleared

public slots:
  void clear();
  QHBoxLayout *addRow(void *row_data);

signals:
  void modelModified();

protected slots:
  void addNewRow();
  void deleteRow();
  void editStim(Input *focus = nullptr); // The track of input [focus], if any, is highlighted in the editor
  // The following methods must be defined in the derived classes 
protected:
  virtual void addRowFields(QHBoxLayout *row_layout, void *row_data) = 0;
    // Fields shown _before_ the [Delete] button on the row
    // [row_data] must point to the data used to populate the added row fields
  virtual void deleteRowFields(QHBoxLayout *row_layout) = 0; 
  // Returns a non-empty explanation if the row cannot be deleted (default: always possible)
  virtual QString rowDeletionError(QHBoxLayout *) { return QString(); }
  // Called when the stimuli of the model inputs have been changed by the stimuli editor (default: nothing)
  virtual void stimuliEdited() {}
public:
  virtual QStringList retrieve() = 0;
};
