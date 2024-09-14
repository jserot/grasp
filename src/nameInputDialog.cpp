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

#include "nameInputDialog.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QtDebug>

NameInputDialog::NameInputDialog(QString initText, QWidget* parent) : QDialog(parent)
{
  setWindowTitle("Automaton name");
  QVBoxLayout *layout = new QVBoxLayout();
  QHBoxLayout *row_layout1 = new QHBoxLayout();
  text = new QLineEdit();
  text->setPlaceholderText(initText);
  row_layout1->addWidget(text);
  layout->addLayout(row_layout1);
  QHBoxLayout *row_layout2 = new QHBoxLayout();
  QPushButton *cancel_button = new QPushButton("Cancel");
  QPushButton *accept_button = new QPushButton("Done");
  cancel_button->setDefault(false);
  accept_button->setDefault(true);
  row_layout2->addWidget(cancel_button);
  row_layout2->addWidget(accept_button);
  layout->addLayout(row_layout2);
  setLayout(layout);
  connect(cancel_button, &QPushButton::clicked, this, &NameInputDialog::cancel);
  connect(accept_button, &QPushButton::clicked, this, &NameInputDialog::accept);
}

void NameInputDialog::accept()
{
  result = text->text();
  QDialog::done(Accepted);
}

void NameInputDialog::cancel()
{
  QDialog::done(Rejected);
}

NameInputDialog::~NameInputDialog()
{
}
