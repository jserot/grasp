#include "simplePanel.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>

SimplePanel::SimplePanel(QString title) : Panel(title)
{
}

void SimplePanel::addRowFields(QHBoxLayout *row_layout)
{
  int nb_rows = row_layout->count();
  QString name(QString(tr("#%1:").arg(nb_rows)));
  QLabel *label = new QLabel(name);
  row_layout->addWidget(label);
  QLineEdit *ledit = new QLineEdit();
  ledit->setPlaceholderText("Enter text");
  ledit->setObjectName(name);
  connect(ledit, &QLineEdit::editingFinished, this, &SimplePanel::rowEdited);
  row_layout->addWidget(ledit);
}

void SimplePanel::rowEdited()
{
  // QLineEdit* ledit = qobject_cast<QLineEdit*>(sender());
  // qDebug() << ledit->objectName() << "=" << ledit->text();
}

void SimplePanel::retrieve()
{
  for ( int i=0; i<layout->count()-1; i++ ) { // Exclude last row, carrying panel buttons
    QHBoxLayout* row = static_cast<QHBoxLayout*>(layout->itemAt(i));
    QLineEdit* ledit = qobject_cast<QLineEdit*>(row->itemAt(1)->widget());
    qDebug() << ledit->objectName() << "=" << ledit->text();
  }
}

SimplePanel::~SimplePanel()
{
}
