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

#include "stateValuations.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QtDebug>

StateValuations::StateValuations(QString title, QStringList& valuations) : DynamicPanel(title)
{
  qDebug() << "StateValuations::StateValuations: valuations=" << valuations;
  foreach ( QString v, valuations )
    addRow((void *)(&v));
}

void StateValuations::addRowFields(QHBoxLayout *row_layout, void *row_data)
{
  int nb_rows = row_layout->count();
  QString name(QString(tr("valuation #%1").arg(nb_rows)));
  QLineEdit *valuation = new QLineEdit();
  valuation->setObjectName(name);
  valuation->setPlaceholderText("<output>=<value>");
  valuation->setMinimumSize(120,valuation->minimumHeight());
  valuation->setFrame(true);
  valuation->setText(row_data ? *(QString *)row_data : "");
  valuation->setCursorPosition(0);
  row_layout->addWidget(valuation);
}

void StateValuations::deleteRowFields(QHBoxLayout *row_layout)
{
  Q_UNUSED(row_layout);
  // Nothing
}

QStringList StateValuations::retrieve()
{
  QStringList valuations;
  for ( int i=1; i<layout->count(); i++ ) { // Exclude first row, carrying panel buttons
    QHBoxLayout* row = static_cast<QHBoxLayout*>(layout->itemAt(i));
    Q_ASSERT(row);
    QLineEdit* ledit = qobject_cast<QLineEdit*>(row->itemAt(0)->widget());
    Q_ASSERT(ledit);
    QString valuation = ledit->text().trimmed();
    valuations << valuation;
    }
  qDebug () << "StateValuationPanel: valuations=" << valuations;
  return valuations;
}

StateValuations::~StateValuations()
{
}
