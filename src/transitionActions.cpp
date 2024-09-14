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

#include "transitionActions.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QtDebug>

TransitionActions::TransitionActions(QString title, QStringList& actions) : DynamicPanel(title)
{
  foreach ( QString v, actions )
    addRow((void *)(&v));
}

void TransitionActions::addRowFields(QHBoxLayout *row_layout, void *row_data)
{
  int nb_rows = row_layout->count();
  QString name(QString(tr("action #%1").arg(nb_rows)));
  QLineEdit *valuation = new QLineEdit();
  valuation->setObjectName(name);
  valuation->setPlaceholderText("<o/v>:=<expr> | o");
  valuation->setMinimumSize(120,valuation->minimumHeight());
  valuation->setFrame(true);
  valuation->setText(row_data ? *(QString *)row_data : "");
  valuation->setCursorPosition(0);
  row_layout->addWidget(valuation);
}

void TransitionActions::deleteRowFields(QHBoxLayout *row_layout)
{
  Q_UNUSED(row_layout);
  // Nothing
}

QStringList TransitionActions::retrieve()
{
  QStringList actions;
  for ( int i=1; i<layout->count(); i++ ) { // Exclude first row, carrying panel buttons
    QHBoxLayout* row = static_cast<QHBoxLayout*>(layout->itemAt(i));
    Q_ASSERT(row);
    QLineEdit* ledit = qobject_cast<QLineEdit*>(row->itemAt(0)->widget());
    Q_ASSERT(ledit);
    QString action = ledit->text().trimmed();
    actions << action;
    }
  qDebug () << "TransitionActions: actions=" << actions;
  return actions;
}

TransitionActions::~TransitionActions()
{
}
