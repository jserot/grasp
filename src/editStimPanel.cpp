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

#include <QHBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QTabWidget>
#include <QMessageBox>
#include <QGraphicsView>
#include <QtDebug>

#include "editStimPanel.h"
#include "model.h"
#include "iov.h"
#include "iovPanel.h"
#include "globals.h"

EditStimPanel::EditStimPanel(Model *model, QWidget *parent)
  : QDialog(parent)
{
  this->model = model;

  setWindowTitle("Input stimuli");
  QVBoxLayout *layout = new QVBoxLayout();

  IovPanel::Client client = { IovPanel::IcModel, { .model = this->model } }; 
  stim_panel = new IovPanel(IoKind::IoIn, "Inputs", "Inputs", client);
  fill();
  layout->addWidget(stim_panel);

  QHBoxLayout *row_layout2 = new QHBoxLayout();
  QPushButton *cancel_button = new QPushButton("Cancel");
  QPushButton *accept_button = new QPushButton("Done");
  cancel_button->setDefault(false);
  accept_button->setDefault(true);
  row_layout2->addWidget(cancel_button);
  row_layout2->addWidget(accept_button);
  layout->addLayout(row_layout2);

  setLayout(layout);

  connect(cancel_button, &QPushButton::clicked, this, &EditStimPanel::cancel);
  connect(accept_button, &QPushButton::clicked, this, &EditStimPanel::accept);
}

void EditStimPanel::fill()
{
  Q_ASSERT(model);
  QList<Iov*> inps = model->inputs();
  foreach (Iov *inp, inps)
      stim_panel->addRow((void *)(inp));
}

void EditStimPanel::accept()
{
  qDebug() << "EditStimPanel::accept";
  QDialog::done(Accepted);
}

void EditStimPanel::cancel()
{
  qDebug() << "EditStimPanel::cancel";
  QDialog::done(Rejected);
}

EditStimPanel::~EditStimPanel()
{
}
