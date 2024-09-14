/***********************************************************************/
/*                                                                     */
/*       This file is part of the RFSM Light software package          */
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

class QPushButton;
class QBoxLayout;
class QVBoxLayout;
class QHBoxLayout;

class DynamicPanel : public QGroupBox
{
  Q_OBJECT

protected:
  QVBoxLayout *layout;
  QPushButton *add_button;
  QPushButton *clear_button;
  QHash<QPushButton*, QBoxLayout*> mButtonToLayoutMap;

  void delete_row(QHBoxLayout *row_layout);

public:
  DynamicPanel(QString title);
  ~DynamicPanel();

public slots:
  void clear();
  QHBoxLayout *addRow(void *row_data);

signals:
    
protected slots:
  void addNewRow();
  void deleteRow();
  // The following methods must be defined in the derived classes 
protected:
  virtual void addRowFields(QHBoxLayout *row_layout, void *row_data) = 0;
    // Fields shown _before_ the [Delete] button on the row
    // [row_data] must point to the data used to populate the added row fields
  virtual void deleteRowFields(QHBoxLayout *row_layout) = 0; 
public:
  virtual QStringList retrieve() = 0;
};
