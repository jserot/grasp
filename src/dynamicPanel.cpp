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

#include "dynamicPanel.h"

#include <QApplication>
#include <QGroupBox>
#include <QPushButton>
#include <QBoxLayout>
#include <QHBoxLayout>
#include <QtDebug>

DynamicPanel::DynamicPanel(QString title) : QGroupBox(title)
{
  layout = new QVBoxLayout();
  layout->setSpacing(0); // Minimize space between rows
  QHBoxLayout *top_row_layout = new QHBoxLayout();
  top_row_layout->setObjectName("top row");
  add_button = new QPushButton("Add");
  clear_button = new QPushButton("Clear");
  top_row_layout->addWidget(add_button);
  top_row_layout->addWidget(clear_button);
  layout->addLayout(top_row_layout);
  setLayout(layout);
  connect(add_button, &QPushButton::clicked, this, &DynamicPanel::addNewRow);
  connect(clear_button, &QPushButton::clicked, this, &DynamicPanel::clear);
  add_button->setDefault(false);
  clear_button->setDefault(false);
}

QHBoxLayout* DynamicPanel::addRow(void *row_data) {
  QHBoxLayout* row_layout = new QHBoxLayout();
  int nb_rows = layout->count();
  row_layout->setObjectName("row" + QString::number(nb_rows));
  QString name(QString(tr("#%1:").arg(nb_rows)));
  addRowFields(row_layout, row_data); // Derived row fields added here
  QPushButton *delButton = new QPushButton();
  row_layout->addWidget(delButton);
  delButton->setIcon(QIcon(":/images/delete.png"));
  connect(delButton, &QPushButton::clicked, this, &DynamicPanel::deleteRow);
  mButtonToLayoutMap.insert(delButton, row_layout);
  layout->insertLayout(layout->count(), row_layout); // Add to bottom
  return row_layout;
}

void DynamicPanel::addNewRow()
{
  QHBoxLayout *row_layout = addRow(nullptr);
  QWidget* first = qobject_cast<QWidget*>(row_layout->itemAt(0)->widget());
  Q_ASSERT(first);
  first->setFocus();
}

void DynamicPanel::delete_row(QHBoxLayout *row_layout)
{
  deleteRowFields(row_layout);
  while (row_layout->count() != 0) {
    QLayoutItem* item = row_layout->takeAt(0);
    delete item->widget();
    delete item;
  }
  delete row_layout;
}

void DynamicPanel::deleteRow()
{
  QPushButton* button = qobject_cast<QPushButton*>(sender());
  QHBoxLayout* row_layout = static_cast<QHBoxLayout*>(mButtonToLayoutMap.take(button));
  DynamicPanel::delete_row(row_layout);
}

void DynamicPanel::clear()
{
  while ( layout->count() > 1 ) { // Do not erase first row !
    QHBoxLayout* row = static_cast<QHBoxLayout*>(layout->takeAt(1));
    Q_ASSERT(row);
    qDebug() << "DynamicPanel: deleting row" << row->objectName();
    delete_row(row);
  }
}

DynamicPanel::~DynamicPanel()
{
  // TBC
}
