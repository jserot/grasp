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

#include "iovPanel.h"

#include <QHBoxLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QRegularExpressionValidator>
#include <QtDebug>
#include <algorithm>

#include "model.h"
#include "diagram.h"
#include "transition.h"
#include "iov.h"
#include "rowDesc.h"
#include "globals.h"
#include "fieldValidation.h"

#define QCOMBOBOX_INDEX_CHANGED (&QComboBox::currentIndexChanged)

static DynamicPanel::Client mkCustomData(IoKind kind, IovPanel::Client client) 
{
  switch ( kind ) {
    case IoKind::IoIn: return { .ctTag = DynamicPanel::CtInputs, .ctData = { .model = client.icClient.model } };
    default: return { .ctTag = DynamicPanel::CtOther };
  }     
}

IovPanel::IovPanel(IoKind kind, QString title, QString rowPrefix, Client client)
  : DynamicPanel(title,mkCustomData(kind,client))
{
  this->kind = kind;
  this->client = client;
  this->rowPrefix = rowPrefix;

  name_validator = new QRegularExpressionValidator(Globals::re_lid, this);

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
    switch ( io->kind() ) {
      case IoKind::IoIn: 
      case IoKind::IoOut: 
        setComboBoxItemEnabled(type_selector, 0, true); // event
        setComboBoxItemEnabled(type_selector, 1, true); // int
        setComboBoxItemEnabled(type_selector, 2, true); // bool
        break;
      case IoKind::IoVar: 
        setComboBoxItemEnabled(type_selector, 0, false); // No event type for variables
        setComboBoxItemEnabled(type_selector, 1, true); // int
        setComboBoxItemEnabled(type_selector, 2, true); // bool
        break;
      }
    type_selector->setCurrentIndex(static_cast<int>(io->type));
    }
  else {
    setComboBoxItemEnabled(type_selector, 0, false); // event
    setComboBoxItemEnabled(type_selector, 1, false); // int
    setComboBoxItemEnabled(type_selector, 2, false); // int
    type_selector->setCurrentIndex(-1);
  }
}

void IovPanel::addRowFields(QHBoxLayout *row_layout, void *row_data)
{
  Iov *io;
  Model *model;
  Diagram *diagram;
  qDebug() << "IovPanel::addRowFields: " << row_data;
  if ( row_data )
    io = (Iov*)(row_data);
  else {
    switch ( client.icKind ) {
    case IcModel: 
      model = client.icClient.model;
      Q_ASSERT(model);
      switch ( kind ) {
      case IoKind::IoIn:
        io = model->addInput("", IoType::TyEvent);
        break;
      case IoKind::IoOut:
        io = model->addOutput("", IoType::TyEvent);
        break;
      case IoKind::IoVar:
        io = model->addGvar("", IoType::TyInt); // No event type for variables
        break;
      }
      break;
    case IcDiagram: 
      diagram = client.icClient.diagram;
      Q_ASSERT(diagram);
      io = diagram->addVar("", IoType::TyInt);
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
  type_selector->addItem("event", static_cast<int>(IoType::TyEvent));
  type_selector->addItem("int", static_cast<int>(IoType::TyInt));
  type_selector->addItem("bool", static_cast<int>(IoType::TyBool));
  setTypeSelector(type_selector, io->name != "", io);
  row_layout->addWidget(type_selector);
  connect(type_selector, QCOMBOBOX_INDEX_CHANGED, this, &IovPanel::typeEdited);

  rows.push_back(std::make_unique<RowDesc>(io, row_layout, name_selector, type_selector));
  RowDesc *row_desc = rows.back().get();

  if ( kind == IoKind::IoIn && client.icKind == IcModel ) { // Tells whether a stimulus is attached to the input
    QPushButton *stim_button = new QPushButton("Stim");
    stim_button->setFixedWidth(stim_button->sizeHint().width()); // Same width for all rows, whatever the text
    stim_button->setAutoDefault(false); // [Return] in the name field must not open the editor
    stim_button->setFocusPolicy(Qt::NoFocus);
    row_layout->addWidget(stim_button);
    connect(stim_button, &QPushButton::clicked, this, [this, io]() { editStim(static_cast<Input*>(io)); });
    row_desc->stim_button = stim_button;
    updateStimButton(row_desc);
    }
  
  widgetToRow.insert((QWidget*)name_selector, row_desc);
  widgetToRow.insert((QWidget*)type_selector, row_desc);
}

void IovPanel::deleteRowFields(QHBoxLayout *row_layout)
{
  Model *model;
  Diagram *diagram;
  QLineEdit *name_selector = qobject_cast<QLineEdit*>(row_layout->itemAt(0)->widget());
  Q_ASSERT(name_selector);
  QComboBox *type_selector = qobject_cast<QComboBox*>(row_layout->itemAt(1)->widget());
  Q_ASSERT(type_selector);
  RowDesc *row_desc = widgetToRow.value(name_selector);
  Q_ASSERT(row_desc);
  Iov* io = row_desc->io;
  qDebug() << "Removing Io/Var" << io->toString();
  switch ( client.icKind ) {
    case IcModel:
      model = client.icClient.model;
      Q_ASSERT(model);
      model->removeIo(io);
      break;
    case IcDiagram:
      diagram = client.icClient.diagram;
      Q_ASSERT(diagram);
      diagram->removeVar(io);
      break;
    }
  widgetToRow.remove(name_selector);
  widgetToRow.remove(type_selector);
  rows.erase(std::remove_if(rows.begin(), rows.end(),
                            [row_desc](const std::unique_ptr<RowDesc> &r) { return r.get() == row_desc; }),
             rows.end()); // Deletes the row descriptor
  emit modelModified();
}

void IovPanel::updateTypeChoices(RowDesc *row_desc)
{
  bool enabled = row_desc->name_selector->text().trimmed() != "";
  setTypeSelector(row_desc->type_selector, enabled, row_desc->io);
}

static int stimItemCount(const Stimulus &stim)
{
  switch ( stim.kind ) {
  case Stimulus::Sporadic: return stim.desc.sporadic.dates.size();
  case Stimulus::ValueChanges: return stim.desc.valueChanges.vcs.size();
  default: return 0;
  }
}

void IovPanel::updateStimButton(RowDesc *row_desc)
{
  QPushButton *button = row_desc->stim_button;
  if ( ! button ) return;
  const Stimulus &stim = static_cast<Input*>(row_desc->io)->stim;
  int n = stimItemCount(stim);
  if ( n == 0 ) {
    button->setText("-");
    button->setToolTip("No stimulus. Click to edit the stimuli");
    }
  else {
    button->setText("Stim");
    button->setToolTip(QString(stim.kind == Stimulus::Sporadic ? "%1 event(s)" : "%1 value change(s)").arg(n)
                       + ". Click to edit the stimuli");
    }
}

void IovPanel::stimuliEdited()
{
  for ( const auto &row: rows ) updateStimButton(row.get());
}

void IovPanel::nameChanged()
{
  QLineEdit* name_selector = qobject_cast<QLineEdit*>(sender());
  Q_ASSERT(name_selector);
  RowDesc *row_desc = widgetToRow.value(name_selector);
  Q_ASSERT(row_desc);
  // Note: the IO name itself is only updated when the edition is finished (see nameEdited). Operations using the
  // model (save, check, export, ...) validate the pending edition first (see MainWindow::beginUserOperation)
  updateTypeChoices(row_desc);
  QString name = name_selector->text().trimmed();
  QString error;
  if ( ! name.isEmpty() && isNameUsedByOther(row_desc->io, name) )
    error = "The name " + name + " is already used. Choose another one, or "
      + (row_desc->committedName.isEmpty() ? QString("the name will be cleared")
                                           : "the previous name (" + row_desc->committedName + ") will be restored");
  showFieldError(name_selector, error_label, error);
}

// Whether [name] is the name of an IO (model) or variable (diagram) other than [io]
bool IovPanel::isNameUsedByOther(const Iov *io, const QString &name) const
{
  QList<Iov*> others;
  switch ( client.icKind ) {
    case IcModel:
      Q_ASSERT(client.icClient.model);
      others = client.icClient.model->inputs() + client.icClient.model->outputs() + client.icClient.model->globalVars();
      break;
    case IcDiagram:
      Q_ASSERT(client.icClient.diagram);
      others = client.icClient.diagram->getVars();
      break;
    }
  for ( const Iov* other: others )
    if ( other != io && other->name == name ) return true;
  return false;
}

QStringList IovPanel::transitionsTriggeredBy(const QString &event)
{
  QStringList r;
  if ( client.icKind != IcModel || event.isEmpty() ) return r;
  for ( Diagram *d : client.icClient.model->getDiagrams() )
    for ( Transition *t : d->transitions() )
      if ( ! t->isInitial() && t->getEvent() == event )
        r.append(d->getName() + ": " + t->toString());
  return r;
}

// Returns a non-empty explanation if [event] triggers some transitions
QString IovPanel::usedEventError(const QString &event, const QString &what)
{
  QStringList uses = transitionsTriggeredBy(event);
  if ( uses.isEmpty() ) return QString();
  return "Cannot " + what + " " + event + ", which triggers the following transition(s):\n  "
    + uses.join("\n  ") + "\nPlease modify or delete these transitions first.";
}

QString IovPanel::rowDeletionError(QHBoxLayout *row_layout)
{
  QWidget *name_selector = row_layout->itemAt(0)->widget();
  RowDesc *row_desc = widgetToRow.value(name_selector);
  if ( ! row_desc || row_desc->io->type != IoType::TyEvent ) return QString();
  return usedEventError(row_desc->committedName, "delete");
}

void IovPanel::nameEdited()
{
  //qDebug() << "IovPanel::nameEdited called by" << sender();
  QLineEdit* name_selector = qobject_cast<QLineEdit*>(sender());
  Q_ASSERT(name_selector);
  RowDesc *row_desc = widgetToRow.value(name_selector);
  Q_ASSERT(row_desc);
  Iov* io = row_desc->io;
  qDebug() << "IovPanel: setting name for IOV" << io->name;
  QString name = name_selector->text().trimmed();
  showFieldError(name_selector, error_label, QString());
  if ( ! name.isEmpty() && isNameUsedByOther(io, name) ) {
    // A name already used is not applied: the previous one is restored (as announced while it was typed)
    qDebug() << "IovPanel: name" << name << "already used; restoring" << row_desc->committedName;
    name_selector->setText(row_desc->committedName);
    name_selector->setCursorPosition(0);
    updateTypeChoices(row_desc);
    if ( io->name == row_desc->committedName ) return; // Nothing changed
    name = row_desc->committedName;
    }
  io->name = name;
  // When an event is renamed, the transitions it triggers are updated accordingly
  QString old_name = row_desc->committedName;
  if ( ! name.isEmpty() ) {
    if ( io->type == IoType::TyEvent && ! old_name.isEmpty() && old_name != name && client.icKind == IcModel ) {
      for ( Diagram *d : client.icClient.model->getDiagrams() ) {
        for ( Transition *t : d->transitions() )
          if ( ! t->isInitial() && t->getEvent() == old_name ) {
            qDebug() << "IovPanel: renaming event of transition" << t->toString() << "to" << name;
            t->setEvent(name);
            t->update();
            }
        }
      }
    row_desc->committedName = name;
    }
  qDebug() << "IovPanel: updated IOV" << io->name;
  emit modelModified();
}

void IovPanel::typeEdited()
{
  //qDebug() << "IovPanel::typeEdited called by" << sender();
  QComboBox* type_selector = qobject_cast<QComboBox*>(sender());
  Q_ASSERT(type_selector);
  RowDesc *row_desc = widgetToRow.value(type_selector);
  Q_ASSERT(row_desc);
  Iov* io = row_desc->io;
  int index = type_selector->currentIndex();
  if ( index < 0 ) return; // No type selected (the name has been cleared) : the IO type is left unchanged
  IoType new_type = static_cast<IoType>(index);
  if ( io->type == IoType::TyEvent && new_type != IoType::TyEvent ) {
    QString error = usedEventError(row_desc->committedName, "change the type of");
    if ( ! error.isEmpty() ) {
      QMessageBox::warning(this, "Error", error);
      const QSignalBlocker blocker(type_selector);
      type_selector->setCurrentIndex(static_cast<int>(IoType::TyEvent));
      return;
      }
    }
  if ( new_type == io->type ) return;
  // The stimulus of an input is only meaningful for its type (event dates, integer or boolean values): it is
  // cleared when the type changes, after confirmation
  if ( row_desc->stim_button ) {
    Input *inp = static_cast<Input*>(io);
    int n = stimItemCount(inp->stim);
    if ( n > 0 ) {
      QString what = QString(inp->stim.kind == Stimulus::Sporadic ? "%1 event(s)" : "%1 value change(s)").arg(n);
      QMessageBox::StandardButton answer = QMessageBox::question(
        this, "Change input type",
        "Changing the type of input " + row_desc->committedName + " will delete its stimuli (" + what + ").\n"
        "Continue?");
      if ( answer != QMessageBox::Yes ) {
        const QSignalBlocker blocker(type_selector);
        type_selector->setCurrentIndex(static_cast<int>(io->type));
        return;
        }
      inp->stim = Stimulus(Stimulus::None);
      updateStimButton(row_desc);
      }
    }
  qDebug() << "IovPanel: setting type for IOV" << io->name;
  io->type = new_type;
  qDebug () << "Setting IO type: " << toString(io->type);
  qDebug() << "IovPanel: updated IOV:" << io->name;
  emit modelModified();
}

QStringList IovPanel::retrieve()
{
  return QStringList();
}

IovPanel::~IovPanel()
{
}
