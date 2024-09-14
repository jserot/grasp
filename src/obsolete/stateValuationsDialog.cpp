#include "stateValuationsDialog.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>

StateValuationsDialog::StateValuationsDialog(QString title, QStringList *values, QWidget *parent)
  : DynDialog(title, values, parent)
{
  foreach ( QString v, *values ) {
    addRow(v);
    }
}

void StateValuationsDialog::addRowFields(QHBoxLayout *row_layout, QString& v)
{
  int nb_rows = row_layout->count();
  QString name(QString(tr("valuation #%1").arg(nb_rows)));
  QLineEdit *valuation = new QLineEdit();
  valuation->setObjectName(name);
  valuation->setPlaceholderText("<output>=<value>");
  valuation->setMinimumSize(120,valuation->minimumHeight());
  valuation->setFrame(true);
  valuation->setText(v);
  valuation->setCursorPosition(0);
  connect(valuation, &QLineEdit::editingFinished, this, &StateValuationsDialog::rowEdited);
  row_layout->addWidget(valuation);
}

void StateValuationsDialog::rowEdited()
{
  QLineEdit* ledit = qobject_cast<QLineEdit*>(sender());
  qDebug() << ledit->objectName() << "=" << ledit->text();
}

QStringList StateValuationsDialog::retrieve()
{
  QStringList valuations;
  for ( int i=0; i<layout->count()-1; i++ ) { // Exclude last row, carrying panel buttons
    QHBoxLayout* row = static_cast<QHBoxLayout*>(layout->itemAt(i));
    QLineEdit* ledit = qobject_cast<QLineEdit*>(row->itemAt(0)->widget());
    QString valuation = ledit->text().trimmed();
    valuations << valuation;
    }
  // qDebug () << "StateValuationDialog: retrieve: valuations=" << valuations;
  return valuations;
}

StateValuationsDialog::~StateValuationsDialog()
{
}
