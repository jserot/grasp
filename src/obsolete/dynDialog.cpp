#include "dynDialog.h"

#include <QPushButton>
#include <QBoxLayout>
#include <QHBoxLayout>

DynDialog::DynDialog(QString title, QStringList *values, QWidget *parent) : QDialog(parent)
{
  setWindowTitle(title);
  layout = new QVBoxLayout();
  QHBoxLayout *but_layout = new QHBoxLayout();
  add_button = new QPushButton("Add");
  clear_button = new QPushButton("Clear");
  cancel_button = new QPushButton("Cancel");
  accept_button = new QPushButton("Done");
  but_layout->addWidget(add_button);
  but_layout->addWidget(clear_button);
  but_layout->addWidget(cancel_button);
  but_layout->addWidget(accept_button);
  layout->addLayout(but_layout);
  setLayout(layout);
  connect(add_button, &QPushButton::clicked, this, &DynDialog::addNewRow);
  connect(clear_button, &QPushButton::clicked, this, &DynDialog::clear);
  connect(cancel_button, &QPushButton::clicked, this, &DynDialog::cancel);
  connect(accept_button, &QPushButton::clicked, this, &DynDialog::accept);
  this->values = values;
}

void DynDialog::addRow(QString v) {
  QHBoxLayout* row_layout = new QHBoxLayout();
  int nb_rows = layout->count();
  row_layout->setObjectName("row" + QString::number(nb_rows));
  QString name(QString(tr("#%1:").arg(nb_rows)));
  addRowFields(row_layout, v); // DynDialog specific items added here 
  QPushButton *delButton = new QPushButton("Delete");
  row_layout->addWidget(delButton);
  delButton->setIcon(QIcon(":/images/delete.png"));
  connect(delButton, &QPushButton::clicked, this, &DynDialog::deleteRow);
  mButtonToLayoutMap.insert(delButton, row_layout);
  layout->insertLayout(0, row_layout); // Add to top
}

void DynDialog::addNewRow()
{
  addRow(QString(""));
}

void DynDialog::delete_row(QLayout *row_layout)
{
  while (row_layout->count() != 0) {
    QLayoutItem* item = row_layout->takeAt(0);
    delete item->widget();
    delete item;
  }
  delete row_layout;
}

void DynDialog::deleteRow()
{
  QPushButton* button = qobject_cast<QPushButton*>(sender());
  QHBoxLayout* row_layout = static_cast<QHBoxLayout*>(mButtonToLayoutMap.take(button));
  DynDialog::delete_row(row_layout);
}

void DynDialog::clear()
{
  //qDebug() << "DynDialog: clearing:" << layout->count();
  while ( layout->count() > 1 ) { // Do not erase the last row !
    QHBoxLayout* row = static_cast<QHBoxLayout*>(layout->takeAt(0));
    assert(row);
    // qDebug() << "DynDialog: deleting row" << row->objectName();
    delete_row(row);
  }
}

void DynDialog::accept()
{
  values->clear();
  *values = retrieve(); // Back to caller
  qDebug() << "DynDialog::close: " << *values;
  clear();
  while (layout->count() != 0) {
    QHBoxLayout* row = static_cast<QHBoxLayout*>(layout->takeAt(0));
    assert(row);
    delete_row(row);
  }
  QDialog::accept();
}

void DynDialog::cancel()
{
  QDialog::reject();
}

DynDialog::~DynDialog()
{
  // TBC
}
