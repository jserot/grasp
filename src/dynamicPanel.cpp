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
#include <QMessageBox>
#include "editStimPanel.h"
#include "globals.h"
#include "model.h"
#include "aux/ChronogramEditorWidget.h"
#include <QDialog>
#include <QVBoxLayout>

DynamicPanel::DynamicPanel(QString title, Client client) : QGroupBox(title)
{
  layout = new QVBoxLayout();
  layout->setSpacing(0); // Minimize space between rows
  QHBoxLayout *top_row_layout = new QHBoxLayout();
  top_row_layout->setObjectName("top row");
  add_button = new QPushButton("Add");
  clear_button = new QPushButton("Clear");
  top_row_layout->addWidget(add_button);
  top_row_layout->addWidget(clear_button);
  this->client = client;
  if ( client.ctTag == CtInputs ) {  // When displaying input rows, add a button to show the "edit stimuli" dialog
    // TODO : the button and the associated callback should be passed as the custom data
    custom_button = new QPushButton("STIM");
    top_row_layout->addWidget(custom_button);
    }
  layout->addLayout(top_row_layout);
  setLayout(layout);
  connect(add_button, &QPushButton::clicked, this, &DynamicPanel::addNewRow);
  connect(clear_button, &QPushButton::clicked, this, &DynamicPanel::clear);
  add_button->setDefault(false);
  clear_button->setDefault(false);
  if ( client.ctTag == CtInputs ) {
    connect(custom_button, &QPushButton::clicked, this, &DynamicPanel::editStim);
    // TODO : the button and the associated callback should be passed as the custom data
    custom_button->setDefault(false);
    }
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

void DynamicPanel::editStim()
{
    Model *model = client.ctData.model;
    Q_ASSERT(model);
    Q_ASSERT(Globals::mainWindow);
    qDebug() << "DynamicPanel:: editing stimuli";

    QDialog dialog(Globals::mainWindow);
    dialog.setWindowTitle(tr("Stimuli editor"));
    dialog.resize(900, 500);

    auto *editor = new ChronogramEditorWidget(&dialog);
    auto *layout = new QVBoxLayout(&dialog);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(editor);

    connect(editor, &ChronogramEditorWidget::closeRequested, &dialog, &QDialog::accept);
    connect(editor, &ChronogramEditorWidget::cancelRequested, &dialog, &QDialog::reject);

    StimulusScenario &scenario = editor->scenario();
    foreach (Iov *io, model->getIos()) { // TODO: model->getIns()
      if (io->kind == Iov::IoIn) {
        auto track = std::make_unique<StimulusTrack>(io);
        scenario.addTrack(std::move(track));
      }
    }
    editor->rebuildPanels();
    if (dialog.exec() == QDialog::Accepted) {
    // --- 2) Report des modifications de "scenario" vers "model" ---
    // TODO : relire scenario.tracks() et mettre à jour les signaux de "model"
      qDebug() << "DynamicPanel:: updating model with edited stimuli";
      foreach ( StimulusTrack* track, scenario.tracks() ) {
        qDebug() << track->count() << "stimuli registred in track" << track->name();
        for ( int i = 0; i<track->count(); i++ ) {
          }
        }
    }
}

DynamicPanel::~DynamicPanel()
{
  // TBC
}
