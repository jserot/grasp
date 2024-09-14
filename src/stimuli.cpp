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

#include <QLabel>
#include <QDialogButtonBox>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QPushButton>

#include "stimuli.h"

Stimuli::Stimuli(Stimulus::Kind kind, Iov* inp, QWidget *parent)
    : QDialog(parent)
{
  selectedInp = inp;
  selectedKind = kind;
  QRect r = parent->geometry();
  setGeometry(QRect(r.x()+r.width()/2,r.y()+r.height()/2,250,40));
  centralWidget = new QWidget(parent);
  verticalLayout = new QVBoxLayout(centralWidget);
  verticalLayout->setSpacing(6);
  verticalLayout->setContentsMargins(11, 11, 11, 11);

  form = new QFrame(centralWidget);
  //form->setObjectName("form");
  form->setFrameShape(QFrame::StyledPanel);
  form->setFrameShadow(QFrame::Raised);
  verticalLayout_2 = new QVBoxLayout(form);
  verticalLayout_2->setSpacing(6);
  verticalLayout_2->setContentsMargins(11, 11, 11, 11);
  verticalLayout_2->setObjectName("verticalLayout_2");
  verticalSpacer = new QSpacerItem(20, 30, QSizePolicy::Minimum, QSizePolicy::Expanding);
  verticalLayout_2->addItem(verticalSpacer);

  verticalLayout->addWidget(form);

  formLayout = qobject_cast<QVBoxLayout*>(form->layout());
  //formLayout->setObjectName("formLayout");

  QHBoxLayout *rowLayout;
  QPushButton* addButton;
  std::tuple<int,int,int> p;
  QList<int> ts;
  QList<QPair<int,int>> vcs;
  switch ( selectedKind ) {
  case Stimulus::None: 
    break;
  case Stimulus::Periodic: 
    p = selectedInp->stim.kind == Stimulus::Periodic ?  // Existing values
      std::make_tuple(selectedInp->stim.desc.periodic.period,
                      selectedInp->stim.desc.periodic.start_time,
                      selectedInp->stim.desc.periodic.end_time) :
      std::make_tuple(0, 0, 0);  // Fresh values
    addPeriodicRow("Period", std::get<0>(p), 5, 0, maxTime);
    addPeriodicRow("Start Time", std::get<1>(p), 0, 0, maxTime);
    addPeriodicRow("End Time", std::get<2>(p), 0, 0, maxTime);
    break;
  case Stimulus::Sporadic:
    ts = selectedInp->stim.kind == Stimulus::Sporadic ?  // Existing values
      selectedInp->stim.desc.sporadic.dates :
    QList<int>(); // Fresh values
    rowLayout = new QHBoxLayout(form);
    rowLayout->setObjectName("rowLayout");
    addButton = new QPushButton("Add", form);
    rowLayout->addWidget(addButton);
    connect(addButton, &QPushButton::clicked, this, &Stimuli::addSporadicRow);
    mButtonToLayoutMap.insert(addButton, rowLayout);
    rows.append(rowLayout);
    formLayout->addLayout(rowLayout);
    for (int i=0; i<ts.length(); i++ )
      _addSporadicRow(ts[i]);
    break;
  case Stimulus::ValueChanges:
    vcs = selectedInp->stim.kind == Stimulus::ValueChanges ?  // Existing values
      selectedInp->stim.desc.valueChanges.vcs :
    QList<QPair<int,int>>(); // Fresh values
    rowLayout = new QHBoxLayout(form);
    rowLayout->setObjectName("rowLayout");
    addButton = new QPushButton("Add", form);
    rowLayout->addWidget(addButton);
    connect(addButton, &QPushButton::clicked, this, &Stimuli::addValueChangesRow);
    mButtonToLayoutMap.insert(addButton, rowLayout);
    rows.append(rowLayout);
    formLayout->addLayout(rowLayout);
    vcs = selectedInp->stim.desc.valueChanges.vcs; 
    for (int i=0; i<vcs.length(); i++ )
      _addValueChangesRow(vcs[i]);
    break;
  }

  QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  verticalLayout->addWidget(buttonBox);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(acceptChanges()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(cancelChanges()));
  setLayout(verticalLayout);
  setWindowTitle(tr("Stimuli for input %1").arg(inp->name));

}

void Stimuli::addPeriodicRow(QString name, int val, int step, int lo, int hi)
{
    QHBoxLayout *rowLayout;
    rowLayout = new QHBoxLayout(form);
    rowLayout->setObjectName("rowLayout");
    QLabel *label = new QLabel(name);
    rowLayout->addWidget(label);
    QSpinBox *spinBox = new QSpinBox;
    spinBox->setObjectName("data");
    spinBox->setRange(lo,hi);
    spinBox->setSingleStep(step);
    spinBox->setValue(val);
    rowLayout->addWidget(spinBox);
    formLayout->insertLayout(-1,rowLayout);
    rows.append(rowLayout);
}

void Stimuli::_addSporadicRow(int t) {
  QHBoxLayout* rowLayout = new QHBoxLayout(form);
  rowLayout->setObjectName("rowLayout");
  QLabel *label = new QLabel("Date");
  rowLayout->addWidget(label);
  QSpinBox *spinBox = new QSpinBox;
  spinBox->setObjectName("spinBox");
  spinBox->setRange(0,maxTime);
  spinBox->setSingleStep(1);
  spinBox->setValue(t);
  rowLayout->addWidget(spinBox);
  QPushButton *delButton = new QPushButton("", form);
  delButton->setIcon(QIcon(":/images/delete.png"));
  rowLayout->addWidget(delButton);
  connect(delButton, &QPushButton::clicked, this, &Stimuli::deleteDynamicRow);
  mButtonToLayoutMap.insert(delButton, rowLayout);
  formLayout->addLayout(rowLayout);
  rows.append(rowLayout);
}

void Stimuli::addSporadicRow() { _addSporadicRow(0); }

void Stimuli::_addValueChangesRow(QPair<int,int> vc) {
  QHBoxLayout* rowLayout = new QHBoxLayout(form);
  rowLayout->setObjectName("rowLayout");
  QLabel *label = new QLabel("Date");
  rowLayout->addWidget(label);
  QSpinBox *spinBox = new QSpinBox;
  spinBox->setObjectName("spinBox");
  spinBox->setRange(0,maxTime);
  spinBox->setSingleStep(1);
  spinBox->setValue(vc.first);
  rowLayout->addWidget(spinBox);
  QLabel *label2 = new QLabel("Value");
  rowLayout->addWidget(label2);
  QSpinBox *spinBox2 = new QSpinBox;
  spinBox2->setObjectName("spinBox");
  if ( selectedInp->type == Iov::TyBool ) spinBox2->setRange(0,1);
  spinBox2->setSingleStep(1);
  spinBox2->setValue(vc.second);
  rowLayout->addWidget(spinBox2);
  QPushButton *delButton = new QPushButton("Delete", form);
  rowLayout->addWidget(delButton);
  connect(delButton, &QPushButton::clicked, this, &Stimuli::deleteDynamicRow);
  mButtonToLayoutMap.insert(delButton, rowLayout);
  formLayout->addLayout(rowLayout);
  rows.append(rowLayout);
}

void Stimuli::addValueChangesRow() { _addValueChangesRow(QPair<int,int>(0,0)); }

void Stimuli::acceptChanges()
{
  QList<int> values;
  for ( int i = 0; i < formLayout->count(); ++i ) {
    QLayout* layout = formLayout->itemAt(i)->layout();
    if ( layout && layout->objectName() == "rowLayout" )
      for ( int j = 0; j < layout->count(); ++j ) {
        QSpinBox *spinBox = qobject_cast<QSpinBox*>(layout->itemAt(j)->widget()); // Returns NULL if not a spinBox
        if ( spinBox ) values.append(spinBox->value());
        }
    }
  Stimulus stim(selectedKind, values);
  selectedInp->stim = stim;
  clearForm();
  QDialog::accept();
}

void Stimuli::cancelChanges()
{
  QDialog::reject();
}

void delete_row(QLayout *layout)
{
  while (layout->count() != 0) {
    QLayoutItem* item = layout->takeAt(0);
    delete item->widget();
    delete item;
  }
  delete layout;
}

void Stimuli::deleteDynamicRow()
{
  QPushButton* button = qobject_cast<QPushButton*>(sender());
  QHBoxLayout* row_layout = mButtonToLayoutMap.take(button);
  delete_row(row_layout);
  rows.removeOne(row_layout);
}

void Stimuli::clearForm() {
  while ( ! rows.isEmpty() ) {
    QHBoxLayout* row = rows.takeFirst();
    delete_row(row);
    }
}

Stimuli::~Stimuli()
{
}
