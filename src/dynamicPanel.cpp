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
  for ( auto it = mButtonToLayoutMap.begin(); it != mButtonToLayoutMap.end(); )
    if ( it.value() == row_layout ) it = mButtonToLayoutMap.erase(it); else ++it;
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
  QHBoxLayout* row_layout = static_cast<QHBoxLayout*>(mButtonToLayoutMap.value(button));
  Q_ASSERT(row_layout);
  QString error = rowDeletionError(row_layout);
  if ( ! error.isEmpty() ) {
    QMessageBox::warning(this, "Error", error);
    return;
    }
  DynamicPanel::delete_row(row_layout);
}

void DynamicPanel::clear()
{
  // Nothing is deleted if any of the rows cannot be
  QStringList errors;
  for ( int i = 1; i < layout->count(); i++ ) { // First row holds the buttons
    QHBoxLayout* row = static_cast<QHBoxLayout*>(layout->itemAt(i)->layout());
    QString error = row ? rowDeletionError(row) : QString();
    if ( ! error.isEmpty() ) errors.append(error);
    }
  if ( ! errors.isEmpty() ) {
    QMessageBox::warning(this, "Error", errors.join("\n\n"));
    return;
    }
  while ( layout->count() > 1 ) { // Do not erase first row !
    QHBoxLayout* row = static_cast<QHBoxLayout*>(layout->takeAt(1));
    Q_ASSERT(row);
    qDebug() << "DynamicPanel: deleting row" << row->objectName();
    delete_row(row);
  }
}

// Builds the model stimulus corresponding to an edited track.
// Dates are rounded to the nearest integer (model stimuli are integer-timed), events falling at the same
// date are merged, and values of boolean inputs are normalized to 0/1. Each adjustment is described in
// [adjustments].
static Stimulus trackToStimulus(const StimulusTrack *track, const Input *inp, QStringList &adjustments)
{
  if ( track->count() == 0 ) return Stimulus(Stimulus::None);
  QString name = inp->name;
  bool rounded = false;
  if ( track->type() == TrackStimulusType::Event ) {
    QList<int> dates;
    for ( int i = 0; i<track->count(); i++ ) {
      const TrackStimulus *st = track->at(i);
      if ( st->type() != TrackStimulusType::Event ) continue;
      int t = qRound(st->date());
      if ( t != st->date() ) rounded = true;
      if ( dates.isEmpty() || dates.last() != t ) // Track is sorted by date
        dates.append(t);
      else
        adjustments.append(name + QString(": events at t=%1 merged").arg(t));
      }
    if ( rounded ) adjustments.append(name + ": event dates rounded to integers");
    if ( dates.isEmpty() ) return Stimulus(Stimulus::None);
    return Stimulus(Stimulus::Sporadic, dates);
    }
  else { // ValueChange
    QList<QPair<int,int>> vcs;
    bool normalized = false;
    for ( int i = 0; i<track->count(); i++ ) {
      const TrackStimulus *st = track->at(i);
      if ( st->type() != TrackStimulusType::ValueChange ) continue;
      const ValueChangeStimulus *vc = static_cast<const ValueChangeStimulus *>(st);
      int t = qRound(vc->date());
      if ( t != vc->date() ) rounded = true;
      int v = vc->value().toInt();
      if ( inp->type == IoType::TyBool && v != 0 && v != 1 ) { v = 1; normalized = true; }
      if ( ! vcs.isEmpty() && vcs.last().first == t ) { // Track is sorted by date: keep the last value
        vcs.last().second = v;
        adjustments.append(name + QString(": value changes at t=%1 merged (last value kept)").arg(t));
        }
      else
        vcs.append(QPair<int,int>(t, v));
      }
    if ( rounded ) adjustments.append(name + ": value change dates rounded to integers");
    if ( normalized ) adjustments.append(name + ": non-zero boolean values set to 1");
    if ( vcs.isEmpty() ) return Stimulus(Stimulus::None);
    QList<int> params;
    for ( const auto &vc : vcs ) params << vc.first << vc.second;
    return Stimulus(Stimulus::ValueChanges, params);
    }
}

void DynamicPanel::editStim()
// TODO : this should not be in DynamicPanel but in function defined in the StimEditor class and
// passed as a parameter to the DynamicPanel class (and called when the "Custom" button is clicked
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
    foreach (Iov *io, model->inputs()) {
        Q_ASSERT(io->kind() == IoKind::IoIn);
        auto track = std::make_unique<StimulusTrack>(static_cast<Input*>(io));
        scenario.addTrack(std::move(track));
    }
    editor->rebuildPanels();
    if (dialog.exec() == QDialog::Accepted) {
      // Report the edited tracks back to the model inputs
      // TODO : get rid of all this forth-and-back conversion work by directly storing _tracks_ in model inputs !
      qDebug() << "DynamicPanel:: updating model with edited stimuli";
      bool modified = false;
      QStringList adjustments;
      foreach ( StimulusTrack* track, scenario.tracks() ) {
        qDebug() << track->count() << "stimuli registred in track" << track->name();
        Input *inp = track->io();
        Q_ASSERT(inp);
        Stimulus new_stim = trackToStimulus(track, inp, adjustments);
        if ( new_stim.toString() != inp->stim.toString() ) {
          inp->stim = new_stim;
          modified = true;
          }
        }
      if ( ! adjustments.isEmpty() )
        QMessageBox::warning(Globals::mainWindow, tr("Stimuli editor"),
                             tr("Some stimuli have been adjusted:\n") + adjustments.join("\n"));
      if ( modified ) emit modelModified();
   }
}

DynamicPanel::~DynamicPanel()
{
  // TBC
}
