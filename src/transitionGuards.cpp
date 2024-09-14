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

#include "transitionGuards.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QtDebug>

TransitionGuards::TransitionGuards(QString title, QStringList& guards) : DynamicPanel(title)
{
  foreach ( QString v, guards )
    addRow((void *)(&v));
}

void TransitionGuards::addRowFields(QHBoxLayout *row_layout, void *row_data)
{
  int nb_rows = row_layout->count();
  QString name(QString(tr("guard #%1").arg(nb_rows)));
  QLineEdit *valuation = new QLineEdit();
  valuation->setObjectName(name);
  valuation->setPlaceholderText("<bool expr>");
  valuation->setMinimumSize(120,valuation->minimumHeight());
  valuation->setFrame(true);
  valuation->setText(row_data ? *(QString *)row_data : "");
  valuation->setCursorPosition(0);
  row_layout->addWidget(valuation);
}

void TransitionGuards::deleteRowFields(QHBoxLayout *row_layout)
{
  Q_UNUSED(row_layout);
  // Nothing
}

QStringList TransitionGuards::retrieve()
{
  QStringList guards;
  for ( int i=1; i<layout->count(); i++ ) { // Exclude first row, carrying panel buttons
    QHBoxLayout* row = static_cast<QHBoxLayout*>(layout->itemAt(i));
    Q_ASSERT(row);
    QLineEdit* ledit = qobject_cast<QLineEdit*>(row->itemAt(0)->widget());
    Q_ASSERT(ledit);
    QString guard = ledit->text().trimmed();
    guards << guard;
    }
  qDebug () << "TransitionGuards: guards=" << guards;
  return guards;
}

TransitionGuards::~TransitionGuards()
{
}
