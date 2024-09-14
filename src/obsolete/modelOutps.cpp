#include "modelOutps.h"

#include <QHBoxLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QStandardItemModel>
#include <QRegularExpressionValidator>

#include "model.h"
#include "rowDesc.h"
#include "iov.h"

ModelOutps::ModelOutps(QString title, Model *model, QRegularExpressionValidator *name_validator) : DynamicPanel(title)
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

void ModelOutps::addRowFields(QHBoxLayout *row_layout, QString& v)
{
  Q_UNUSED(v); // Here
  int nb_rows = row_layout->count();
  QString row_name(QString(tr("outp #%1").arg(nb_rows)));

  assert(model);
  Iov* io = model->addIo("", Iov::IoOut, Iov::TyInt, Stimulus(Stimulus::None));

  QLineEdit *name_selector = new QLineEdit();
  name_selector->setObjectName(row_name);
  name_selector->setMinimumSize(80,name_selector->minimumHeight());
  name_selector->setPlaceholderText("<name>");
  name_selector->setFrame(true);
  name_selector->setCursorPosition(0);
  name_selector->setValidator(name_validator);
  row_layout->addWidget(name_selector);
  connect(name_selector, &QLineEdit::editingFinished, this, &ModelOutps::nameEdited);
  connect(name_selector, &QLineEdit::textEdited, this, &ModelOutps::nameChanged);

  QComboBox *type_selector = new QComboBox();
  type_selector->addItem("event", QVariant(Iov::TyEvent));
  type_selector->addItem("int", QVariant(Iov::TyInt));
  type_selector->addItem("bool", QVariant(Iov::TyBool));
  for ( int i=0; i<type_selector->count(); i++ ) 
    setComboBoxItemEnabled(type_selector, i, false); // Nothing enabled 
  type_selector->setCurrentIndex(-1);
  row_layout->addWidget(type_selector);
#if QT_VERSION >= 0x060000
  connect(type_selector, &QComboBox::currentIndexChanged, this, &ModelOutps::typeEdited);
#else
  connect(type_selector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ModelOutps::typeEdited);
#endif

  RowDesc *row_desc = new RowDesc(io, row_layout, name_selector, type_selector);
  
  widgetToRow.insert((QWidget*)name_selector, row_desc);
  widgetToRow.insert((QWidget*)type_selector, row_desc);
}

void ModelOutps::deleteRowFields(QHBoxLayout *row_layout)
{
  QLineEdit *name_selector = qobject_cast<QLineEdit*>(row_layout->itemAt(0)->widget());
  QComboBox *type_selector = qobject_cast<QComboBox*>(row_layout->itemAt(1)->widget());
  assert(name_selector);
  assert(type_selector);
  RowDesc *row_desc = widgetToRow.value(name_selector);
  assert(row_desc);
  Iov* io = row_desc->io;
  qDebug() << "Removing IO" << io->toString();
  model->removeIo(io);
  widgetToRow.remove(name_selector);
  widgetToRow.remove(type_selector);
  emit modelModified();
}

void ModelOutps::updateTypeChoices(RowDesc *row_desc)
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

void ModelOutps::nameChanged()
{
  qDebug() << "** Name changed !";
  QLineEdit* name_selector = qobject_cast<QLineEdit*>(sender());
  assert(name_selector);
  RowDesc *row_desc = widgetToRow.value(name_selector);
  assert(row_desc);
  updateTypeChoices(row_desc);
}

void ModelOutps::nameEdited()
{
  QLineEdit* name_selector = qobject_cast<QLineEdit*>(sender());
  assert(name_selector);
  RowDesc *row_desc = widgetToRow.value(name_selector);
  assert(row_desc);
  QString name = name_selector->text().trimmed();
  Iov* io = row_desc->io;
  qDebug() << "Setting output name to" << name;
  io->name = name;
  emit modelModified();
}

void ModelOutps::typeEdited()
{
  QComboBox* type_selector = qobject_cast<QComboBox*>(sender());
  assert(type_selector);
  RowDesc *row_desc = widgetToRow.value(type_selector);
  assert(row_desc);
  Iov* io = row_desc->io;
  io->type = (Iov::IoType)(type_selector->currentIndex());
  qDebug () << "Setting IO type: " << io->type;
  emit modelModified();
}

QStringList ModelOutps::retrieve()
{
  return QStringList();
}

ModelOutps::~ModelOutps()
{
}
