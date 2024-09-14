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

#include "iovPanel.h"
#include "qt_compat.h"

#include <QHBoxLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QRegularExpressionValidator>
#include <QtDebug>

#include "model.h"
#include "iov.h"
#include "rowDesc.h"
#include "stimuli.h"
#include "globals.h"

IovPanel::IovPanel(Iov::IoKind kind, QString title, QString rowPrefix, Client& client, QRegularExpressionValidator *name_validator)
  : DynamicPanel(title)
{
  this->kind = kind;
  this->client = client;
  this->name_validator = name_validator;
  this->rowPrefix = rowPrefix;

  Q_ASSERT(Globals::mainWindow);
  connect(this, SIGNAL(modelModified()), Globals::mainWindow, SLOT(modelModified()));
}

static void setComboBoxItemEnabled(QComboBox * comboBox, int index, bool enabled)
{
    QStandardItemModel *m = qobject_cast<QStandardItemModel*>(comboBox->model());
    Q_ASSERT(m);
    if(!m) return;
    QStandardItem *item = m->item(index);
    Q_ASSERT(item);
    if(!item) return;
    item->setEnabled(enabled);
}

void setTypeSelector(QComboBox *type_selector, bool enabled, Iov* io)
{
  if ( enabled ) {
    switch ( io->kind ) {
      case Iov::IoIn: 
      case Iov::IoOut: 
        setComboBoxItemEnabled(type_selector, 0, true); // event
        setComboBoxItemEnabled(type_selector, 1, true); // int
        setComboBoxItemEnabled(type_selector, 2, true); // bool
        break;
      case Iov::IoVar: 
        setComboBoxItemEnabled(type_selector, 0, false); // No event type for variables
        setComboBoxItemEnabled(type_selector, 1, true); // int
        setComboBoxItemEnabled(type_selector, 2, true); // bool
        break;
      }
    type_selector->setCurrentIndex(io->type);
    }
  else {
    setComboBoxItemEnabled(type_selector, 0, false); // event
    setComboBoxItemEnabled(type_selector, 1, false); // int
    setComboBoxItemEnabled(type_selector, 2, false); // int
    type_selector->setCurrentIndex(-1);
  }
}

void setStimSelector(QComboBox *stim_selector, bool enabled, Iov* io)
{
  if ( enabled ) {
    switch ( io->type ) {
      case Iov::TyEvent: 
        setComboBoxItemEnabled(stim_selector, 0, true); // None
        setComboBoxItemEnabled(stim_selector, 1, true); // Periodic
        setComboBoxItemEnabled(stim_selector, 2, true); // Sporadic
        setComboBoxItemEnabled(stim_selector, 3, false); // ValueChanges
        break;
      case Iov::TyInt: 
      case Iov::TyBool: 
        setComboBoxItemEnabled(stim_selector, 0, true); // None
        setComboBoxItemEnabled(stim_selector, 1, false); // Periodic
        setComboBoxItemEnabled(stim_selector, 2, false); // Sporadic
        setComboBoxItemEnabled(stim_selector, 3, true); // ValueChanges
        break;
      }
    stim_selector->setCurrentIndex(io->stim.kind);
    }
  else {
    setComboBoxItemEnabled(stim_selector, 0, false); // None
    setComboBoxItemEnabled(stim_selector, 1, false); // Periodic
    setComboBoxItemEnabled(stim_selector, 2, false); // Sporadic
    setComboBoxItemEnabled(stim_selector, 3, false); // ValueChanges
    stim_selector->setCurrentIndex(-1);
  }
}

void IovPanel::addRowFields(QHBoxLayout *row_layout, void *row_data)
{
  Iov *io;
  Model *model;
  Automaton *automaton;
  if ( row_data )
    io = (Iov*)(row_data);
  else {
    switch ( client.icKind ) {
    case IcModel: 
      model = client.icClient.model;
      Q_ASSERT(model);
      io = model->addIo("", kind, Iov::TyEvent, Stimulus(Stimulus::None));
      break;
    case IcAutomaton: 
      automaton = client.icClient.automaton;
      Q_ASSERT(automaton);
      io = automaton->addVar("", Iov::TyInt);
      break;
      }
    }
  int nb_rows = row_layout->count();
  QString row_name(QString(tr("%1 #%2").arg(rowPrefix).arg(nb_rows)));

  QLineEdit *name_selector = new QLineEdit();
  name_selector->setObjectName(row_name);
  name_selector->setMinimumSize(80,name_selector->minimumHeight());
  name_selector->setText(io->name);
  name_selector->setPlaceholderText("<name>");
  name_selector->setFrame(true);
  name_selector->setCursorPosition(0);
  name_selector->setValidator(name_validator);
  name_selector->setFocus();
  row_layout->addWidget(name_selector);
  connect(name_selector, &QLineEdit::editingFinished, this, &IovPanel::nameEdited);
  connect(name_selector, &QLineEdit::textEdited, this, &IovPanel::nameChanged);

  QComboBox *type_selector = new QComboBox();
  type_selector->addItem("event", QVariant(Iov::TyEvent));
  type_selector->addItem("int", QVariant(Iov::TyInt));
  type_selector->addItem("bool", QVariant(Iov::TyBool));
  setTypeSelector(type_selector, io->name != "", io);
  row_layout->addWidget(type_selector);
  connect(type_selector, QCOMBOBOX_INDEX_CHANGED, this, &IovPanel::typeEdited);

  QComboBox *stim_selector;
  if ( kind == Iov::IoIn ) {
    stim_selector = new QComboBox();
    stim_selector->addItem("None", QVariant(Stimulus::None));
    stim_selector->addItem("Periodic", QVariant(Stimulus::Periodic));
    stim_selector->addItem("Sporadic", QVariant(Stimulus::Sporadic));
    stim_selector->addItem("ValueChanges", QVariant(Stimulus::ValueChanges));
    setStimSelector(stim_selector, io->name != "", io);
    row_layout->addWidget(stim_selector);
    connect(stim_selector, QCOMBOBOX_ACTIVATED, this, &IovPanel::stimEdited);
    }
  else
    stim_selector = nullptr;

  RowDesc *row_desc = new RowDesc(io, row_layout, name_selector, type_selector, stim_selector);
  
  widgetToRow.insert((QWidget*)name_selector, row_desc);
  widgetToRow.insert((QWidget*)type_selector, row_desc);
  if ( kind == Iov::IoIn ) 
    widgetToRow.insert((QWidget*)stim_selector, row_desc);
}

void IovPanel::deleteRowFields(QHBoxLayout *row_layout)
{
  QLineEdit *name_selector = qobject_cast<QLineEdit*>(row_layout->itemAt(0)->widget());
  Q_ASSERT(name_selector);
  QComboBox *type_selector = qobject_cast<QComboBox*>(row_layout->itemAt(1)->widget());
  Q_ASSERT(type_selector);
  QComboBox *stim_selector;
  stim_selector = qobject_cast<QComboBox*>(row_layout->itemAt(2)->widget()); // Will be NULL for inputs
  RowDesc *row_desc = widgetToRow.value(name_selector);
  Q_ASSERT(row_desc);
  Iov* io = row_desc->io;
  qDebug() << "Removing Io/Var" << io->toString();
  switch ( client.icKind ) {
    case IcModel: client.icClient.model->removeIo(io); break;
    case IcAutomaton: client.icClient.automaton->removeVar(io); break;
    }
  widgetToRow.remove(name_selector);
  widgetToRow.remove(type_selector);
  if ( stim_selector ) 
    widgetToRow.remove(stim_selector);
  emit modelModified();
}

void IovPanel::updateTypeChoices(RowDesc *row_desc)
{
  bool enabled = row_desc->name_selector->text().trimmed() != "";
  setTypeSelector(row_desc->type_selector, enabled, row_desc->io);
}

void IovPanel::updateStimChoices(RowDesc *row_desc) // For inputs only
{
  bool enabled = row_desc->name_selector->text().trimmed() != "";
  setStimSelector(row_desc->stim_selector, enabled, row_desc->io);
}

void IovPanel::nameChanged()
{
  QLineEdit* name_selector = qobject_cast<QLineEdit*>(sender());
  Q_ASSERT(name_selector);
  RowDesc *row_desc = widgetToRow.value(name_selector);
  Q_ASSERT(row_desc);
  updateTypeChoices(row_desc);
  if ( row_desc->io->kind == Iov::IoIn ) 
    updateStimChoices(row_desc); 
}

void IovPanel::nameEdited()
{
  qDebug() << "** Iov::nameEdited called by" << sender();
   // TOFIX : this may be called spuriously when the automaton scene is scrolled and a new automaton tab is added...
  QLineEdit* name_selector = qobject_cast<QLineEdit*>(sender());
  Q_ASSERT(name_selector);
  RowDesc *row_desc = widgetToRow.value(name_selector);
  Q_ASSERT(row_desc);
  QString name = name_selector->text().trimmed();
  QStringList defined;
  Model *model;
  Automaton *automaton;
  switch ( client.icKind ) {
    case IcModel:
      model = client.icClient.model;
      defined = model->getInputs() + model->getOutputs() + model->getShared();
      break;
    case IcAutomaton:
      automaton = client.icClient.automaton;
      defined = automaton->getVarNames();
      break;
    }
  if ( defined.contains(name) ) {
    QMessageBox::warning( this, "Error", "The name " + name + " is already used. Please choose another none");
    //name_selector->setText("");
    }
  // else {
    Iov* io = row_desc->io;
    qDebug() << "Setting input name to" << name;
    io->name = name;
  //   }
  emit modelModified();
}

void IovPanel::typeEdited()
{
  QComboBox* type_selector = qobject_cast<QComboBox*>(sender());
  Q_ASSERT(type_selector);
  RowDesc *row_desc = widgetToRow.value(type_selector);
  Q_ASSERT(row_desc);
  Iov* io = row_desc->io;
  io->type = (Iov::IoType)(type_selector->currentIndex());
  qDebug () << "Setting IO type: " << io->type;
  if ( row_desc->io->kind == Iov::IoIn ) 
    updateStimChoices(row_desc);
  emit modelModified();
}

void IovPanel::stimEdited()
{
  QComboBox* stim_selector = qobject_cast<QComboBox*>(sender());
  Q_ASSERT(stim_selector);
  RowDesc *row_desc = widgetToRow.value(stim_selector);
  Q_ASSERT(row_desc);
  Iov* io = row_desc->io;
  QString io_name = io->name;
  Stimulus::Kind kind = (Stimulus::Kind)(stim_selector->currentIndex()); 
  switch ( kind ) {
  case Stimulus::None:
    io->stim = Stimulus(Stimulus::None);
    break;
  default:
    Stimuli* stimDialog = new Stimuli(kind,io,this);
    stimDialog->exec();
    //fillIoList();
    delete stimDialog;
    break;
    }
  emit modelModified();
}

QStringList IovPanel::retrieve()
{
  return QStringList();
}

IovPanel::~IovPanel()
{
}
