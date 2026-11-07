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

#include <QHBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QTabWidget>
#include <QMessageBox>
#include <QGraphicsView>
#include <QtDebug>

#include "diagramProperties.h"
#include "diagram.h"
#include "iov.h"
#include "iovPanel.h"
#include "globals.h"
#include "model.h"
#include <QRegularExpression>

DiagramProperties::DiagramProperties(Diagram *diagram, QWidget *parent)
  : QDialog(parent)
{
  this->diagram = diagram;

  QString id = diagram->getName();
  setWindowTitle("Diagram " + id);
  QVBoxLayout *layout = new QVBoxLayout();

  QHBoxLayout *row_layout1 = new QHBoxLayout();
  QLabel* name_label1 = new QLabel("Name");
  name_field = new QLineEdit();
  name_field->setText(id);
  row_layout1->addWidget(name_label1);
  row_layout1->addWidget(name_field);
  layout->addLayout(row_layout1);

  QHBoxLayout *row_layout3 = new QHBoxLayout();
  QLabel* name_label3 = new QLabel("Scale");
  scaleCombo = new QComboBox;
  QStringList scales;
  scales << tr("50%") << tr("75%") << tr("100%") << tr("125%") << tr("150%") << fitScale;
  scaleCombo->addItems(scales);
  // Show the current scale. If it is not one of the listed values, it has been set by "Fit"
  int current = qRound(diagram->getView()->transform().m11() * 100);
  int index = scaleCombo->findText(QString::number(current) + "%");
  scaleCombo->setCurrentIndex(index >= 0 ? index : scaleCombo->findText(fitScale));
  row_layout3->addWidget(name_label3);
  row_layout3->addWidget(scaleCombo);
  layout->addLayout(row_layout3);

  IovPanel::Client client = { IovPanel::IcDiagram, { .diagram = this->diagram } }; 
  vars_panel = new IovPanel(IoKind::IoVar, "Local variables", "Local variable", client);
  fill();
  layout->addWidget(vars_panel);

  QHBoxLayout *row_layout2 = new QHBoxLayout();
  QPushButton *cancel_button = new QPushButton("Cancel");
  QPushButton *accept_button = new QPushButton("Done");
  cancel_button->setDefault(false);
  accept_button->setDefault(true);
  row_layout2->addWidget(cancel_button);
  row_layout2->addWidget(accept_button);
  layout->addLayout(row_layout2);

  setLayout(layout);

  connect(scaleCombo, &QComboBox::currentTextChanged, this, &DiagramProperties::changeScale);
  connect(cancel_button, &QPushButton::clicked, this, &DiagramProperties::cancel);
  connect(accept_button, &QPushButton::clicked, this, &DiagramProperties::accept);
}

void DiagramProperties::fill()
{
  Q_ASSERT(diagram);
  QList<Iov*> vars = diagram->getVars();
  foreach (Iov *var, vars) {
    vars_panel->addRow((void *)(var));
    }
}

const QString DiagramProperties::fitScale = "Fit"; // Scale such that the whole diagram is visible

void DiagramProperties::changeScale(const QString& scale)
{
  Q_ASSERT(diagram);
  QGraphicsView *view = diagram->getView();
  Q_ASSERT(view);
  if ( scale == fitScale ) {
    diagram->fitView(1.5); // Not above the largest listed scale
    return;
    }
  double s = scale.left(scale.indexOf(tr("%"))).toDouble() / 100.0;
  view->setTransform(QTransform::fromScale(s, s));
  diagram->centerView();
}

void DiagramProperties::accept()
{
  QString id = name_field->text().trimmed();
  qDebug() << "DiagramProperties::accept";
  // Diagram names are used as identifiers in the generated RFSM and DOT code
  static const QRegularExpression re_id("^[A-Za-z][A-Za-z0-9_]*$");
  if ( ! re_id.match(id).hasMatch() ) {
    QMessageBox::warning(this, "Error", "Invalid diagram name. It must start with a letter, followed by letters, digits or _");
    return; // Leave the dialog opened
    }
  if ( diagram->enclosingModel()->isDiagramNameUsed(id, diagram) ) {
    QMessageBox::warning(this, "Error", "The name " + id + " is already used by another diagram");
    return;
    }
  if ( id != diagram->getName() ) {
    diagram->setName(id);
    // The dialog is not modal: the current tab may not be the one of this diagram anymore
    int index = Globals::diagrams->indexOf(diagram->getView());
    if ( index >= 0 ) Globals::diagrams->setTabText(index, id);
    emit diagram->modelModified(); // To main window
    }
  QDialog::done(Accepted);
}

void DiagramProperties::cancel()
{
  qDebug() << "DiagramProperties::cancel";
  QDialog::done(Rejected);
}

DiagramProperties::~DiagramProperties()
{
}
