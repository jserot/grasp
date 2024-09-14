#include "modelInps.h"

#include <QHBoxLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QRegularExpressionValidator>

#include "model.h"
#include "iov.h"
#include "rowDesc.h"
#include "stimuli.h"

ModelInps::ModelInps(QString title, Model *model, QRegularExpressionValidator *name_validator) : DynamicPanel(title)
{
  this->model = model;
  this->name_validator = name_validator;
}

static void setComboBoxItemEnabled(QComboBox * comboBox, int index, bool enabled)
{
    auto * model = qobject_cast<QStandardItemModel*>(comboBox->model());
    assert(model);
    if(!model) return;
    auto * item = model->item(index);
    assert(item);
    if(!item) return;
    item->setEnabled(enabled);
}

void ModelInps::addRowFields(QHBoxLayout *row_layout, QString& v)
{
  Q_UNUSED(v); // Here
  int nb_rows = row_layout->count();
  QString row_name(QString(tr("inp #%1").arg(nb_rows)));

  assert(model);
  Iov* io = model->addIo("", Iov::IoIn, Iov::TyEvent, Stimulus(Stimulus::None));

  QLineEdit *name_selector = new QLineEdit();
  name_selector->setObjectName(row_name);
  name_selector->setMinimumSize(80,name_selector->minimumHeight());
  name_selector->setPlaceholderText("<name>");
  name_selector->setFrame(true);
  name_selector->setCursorPosition(0);
  name_selector->setValidator(name_validator);
  row_layout->addWidget(name_selector);
  connect(name_selector, &QLineEdit::editingFinished, this, &ModelInps::nameEdited);
  connect(name_selector, &QLineEdit::textEdited, this, &ModelInps::nameChanged);

  QComboBox *type_selector = new QComboBox();
  type_selector->addItem("event", QVariant(Iov::TyEvent));
  type_selector->addItem("int", QVariant(Iov::TyInt));
  type_selector->addItem("bool", QVariant(Iov::TyBool));
  for ( int i=0; i<type_selector->count(); i++ ) 
    setComboBoxItemEnabled(type_selector, i, false); // Nothing enabled 
  type_selector->setCurrentIndex(-1);
  row_layout->addWidget(type_selector);
#if QT_VERSION >= 0x060000
  connect(type_selector, &QComboBox::currentIndexChanged, this, &ModelInps::typeEdited);
#else
  connect(type_selector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ModelInps::typeEdited);
#endif

  QComboBox *stim_selector = new QComboBox();
  stim_selector->addItem("None", QVariant(Stimulus::None));
  stim_selector->addItem("Periodic", QVariant(Stimulus::Periodic));
  stim_selector->addItem("Sporadic", QVariant(Stimulus::Sporadic));
  stim_selector->addItem("ValueChanges", QVariant(Stimulus::ValueChanges));
  for ( int i=0; i<stim_selector->count(); i++ ) 
    setComboBoxItemEnabled(stim_selector, i, false); // Nothing enabled 
  stim_selector->setCurrentIndex(-1);
  row_layout->addWidget(stim_selector);
#if QT_VERSION >= 0x060000
  connect(stim_selector, &QComboBox::activated, this, &ModelInps::stimEdited);
#else
  connect(stim_selector, QOverload<int>::of(&QComboBox::activated), this, &ModelInps::stimEdited);
#endif

  RowDesc *row_desc = new RowDesc(io, row_layout, name_selector, type_selector, stim_selector);
  
  widgetToRow.insert((QWidget*)name_selector, row_desc);
  widgetToRow.insert((QWidget*)type_selector, row_desc);
  widgetToRow.insert((QWidget*)stim_selector, row_desc);
}

void ModelInps::deleteRowFields(QHBoxLayout *row_layout)
{
  QLineEdit *name_selector = qobject_cast<QLineEdit*>(row_layout->itemAt(0)->widget());
  QComboBox *type_selector = qobject_cast<QComboBox*>(row_layout->itemAt(1)->widget());
  QComboBox *stim_selector = qobject_cast<QComboBox*>(row_layout->itemAt(2)->widget());
  assert(name_selector);
  assert(type_selector);
  assert(stim_selector);
  RowDesc *row_desc = widgetToRow.value(name_selector);
  assert(row_desc);
  Iov* io = row_desc->io;
  qDebug() << "Removing IO" << io->toString();
  model->removeIo(io);
  widgetToRow.remove(name_selector);
  widgetToRow.remove(type_selector);
  widgetToRow.remove(stim_selector);
  emit modelModified();
}

void ModelInps::updateTypeChoices(RowDesc *row_desc)
{
  qDebug() << "** updateTypeChoices";
  QString name = row_desc->name_selector->text().trimmed();
  bool enabled = name != "";
  QComboBox *type_selector = row_desc->type_selector;
  setComboBoxItemEnabled(type_selector, 0, enabled);  // TO FIX: we should not use hardcoded index here 
  setComboBoxItemEnabled(type_selector, 1, enabled); 
  setComboBoxItemEnabled(type_selector, 2, enabled);
  type_selector->setCurrentIndex(enabled ? 1 : -1);
}

void ModelInps::updateStimChoices(RowDesc *row_desc)
{
  qDebug() << "** updateStimChoices";
  QComboBox *stim_selector = row_desc->stim_selector;
  QString name = row_desc->name_selector->text().trimmed();
  bool enabled = name != "";
  if ( enabled ) {
    Iov* io = row_desc->io;
    assert(io);
    QComboBox *stim_selector = row_desc->stim_selector;
    qDebug() << "Setting stim choices for io" << io->toString();
    switch ( io->type ) {
    case Iov::TyEvent:
      setComboBoxItemEnabled(stim_selector, 0, true); // TO FIX: we should not use hardcoded index here 
      setComboBoxItemEnabled(stim_selector, 1, true);
      setComboBoxItemEnabled(stim_selector, 2, true);
      setComboBoxItemEnabled(stim_selector, 3, false);
      break;
    case Iov::TyInt:
    case Iov::TyBool:
      setComboBoxItemEnabled(stim_selector, 0, true);
      setComboBoxItemEnabled(stim_selector, 1, false);
      setComboBoxItemEnabled(stim_selector, 2, false);
      setComboBoxItemEnabled(stim_selector, 3, true);
      break;
    }
    stim_selector->setCurrentIndex(0); // None. 
  } else {
  setComboBoxItemEnabled(stim_selector, 0, false); 
  setComboBoxItemEnabled(stim_selector, 1, false);
  setComboBoxItemEnabled(stim_selector, 2, false);
  setComboBoxItemEnabled(stim_selector, 2, false);
  stim_selector->setCurrentIndex(-1);
  }
}

void ModelInps::nameChanged()
{
  qDebug() << "** Name changed !";
  QLineEdit* name_selector = qobject_cast<QLineEdit*>(sender());
  assert(name_selector);
  RowDesc *row_desc = widgetToRow.value(name_selector);
  assert(row_desc);
  updateTypeChoices(row_desc);
  updateStimChoices(row_desc); 
}

void ModelInps::nameEdited()
{
  QLineEdit* name_selector = qobject_cast<QLineEdit*>(sender());
  assert(name_selector);
  RowDesc *row_desc = widgetToRow.value(name_selector);
  assert(row_desc);
  QString name = name_selector->text().trimmed();
  Iov* io = row_desc->io;
  qDebug() << "Setting input name to" << name;
  io->name = name;
  emit modelModified();
}

void ModelInps::typeEdited()
{
  QComboBox* type_selector = qobject_cast<QComboBox*>(sender());
  assert(type_selector);
  RowDesc *row_desc = widgetToRow.value(type_selector);
  assert(row_desc);
  Iov* io = row_desc->io;
  io->type = (Iov::IoType)(type_selector->currentIndex());
  qDebug () << "Setting IO type: " << io->type;
  updateStimChoices(row_desc);
  emit modelModified();
}

void ModelInps::stimEdited()
{
  QComboBox* stim_selector = qobject_cast<QComboBox*>(sender());
  assert(stim_selector);
  RowDesc *row_desc = widgetToRow.value(stim_selector);
  assert(row_desc);
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

QStringList ModelInps::retrieve()
{
  return QStringList();
}

ModelInps::~ModelInps()
{
}
