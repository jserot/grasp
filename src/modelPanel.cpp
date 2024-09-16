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

#include "modelPanel.h"
#include "mainwindow.h"
#include "model.h"
#include "iovPanel.h"
#include "stimuli.h"
#include "compilerPaths.h"

#include <QFrame>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QString>
#include <QStringList>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QRegularExpression>
#include <QDebug>

QRegularExpressionValidator *ModelPanel::io_name_validator;

ModelPanel::ModelPanel(Model *model, MainWindow* parent) : QFrame(parent)
{
    this->model = model;

    io_name_validator = new QRegularExpressionValidator(Globals::re_lid);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setAlignment(Qt::AlignTop);
    //layout->setMinimumWidth(200);

    name_panel = new QGroupBox("Model name");
    name_panel->setMaximumHeight(100);
    model_name_field = new QLineEdit();
    model_name_field->setPlaceholderText("Model name");
    //model_name_field->setText(Globals::defaultModelName);
    layout->addWidget(name_panel);
    layout->addWidget(model_name_field);
    //name_panel->setLayout(layout);

    IovPanel::Client client;
    client.icKind = IovPanel::IcModel;
    client.icClient.model = model;
    inps_panel = new IovPanel(Iov::IoIn, "Inputs", "Input", client, io_name_validator);
    layout->addWidget(inps_panel);

    outps_panel = new IovPanel(Iov::IoOut, "Outputs", "Output", client, io_name_validator);
    layout->addWidget(outps_panel);

    vars_panel = new IovPanel(Iov::IoVar, "Global variables", "Global variable", client, io_name_validator);
    layout->addWidget(vars_panel);

    // QPushButton *dump_button = new QPushButton("dump");  // For debug only
    // layout->addWidget(dump_button);
    
    this->setLayout(layout);

    connect(model_name_field, &QLineEdit::editingFinished, this, &ModelPanel::setModelName);
    // connect(dump_button, &QPushButton::clicked, this, &ModelPanel::dumpModel); // For debug only
    // connect(inps_panel, SIGNAL(modelModified()), this, SLOT(modelUpdated()));
    // connect(outps_panel, SIGNAL(modelModified()), this, SLOT(modelUpdated()));
    // connect(vars_panel, SIGNAL(modelModified()), this, SLOT(modelUpdated()));
    connect(inps_panel, SIGNAL(modelModified()), Globals::mainWindow, SLOT(modelModified()));
    connect(outps_panel, SIGNAL(modelModified()), Globals::mainWindow, SLOT(modelModified()));
    connect(vars_panel, SIGNAL(modelModified()), Globals::mainWindow, SLOT(modelModified()));
    connect(this, SIGNAL(modelModified()), Globals::mainWindow, SLOT(modelModified()));
}

ModelPanel::~ModelPanel()
{
}

void ModelPanel::modelUpdated()
{
  qDebug() << "modelPanel:: modelUpdated";
  emit modelModified(); // To main window
}

// [Name] panel

void ModelPanel::setModelName()
{
  Q_ASSERT(model);
  QString name = model_name_field->text().trimmed();
  model->setName(name);
  emit modelModified();
}

void ModelPanel::fillModelName()
{
  Q_ASSERT(model);
  model_name_field->setText(model->getName());
}

void ModelPanel::clearModelName()
{
  model_name_field->setText("");
}

// [Inputs], [Outputs] and [Variables] panels

void ModelPanel::fillIovPanel()
{
  Q_ASSERT(model);
  QList<Iov*> ios = model->getIos();
  foreach (Iov *io, ios) {
    switch ( io->kind ) {
    case Iov::IoIn: inps_panel->addRow((void *)(io)); break;
    case Iov::IoOut: outps_panel->addRow((void *)(io)); break;
    case Iov::IoVar: vars_panel->addRow((void *)(io)); break;
    }
  }
}

void ModelPanel::clearIovPanel()
{
  inps_panel->clear();
  outps_panel->clear();
  vars_panel->clear();
}

void ModelPanel::fill()
{
  fillModelName();
  fillIovPanel();
}

void ModelPanel::clear()
{
  clearModelName();
  clearIovPanel();
}

void ModelPanel::update()
{
  clear();
  fill();
}

void ModelPanel::dumpModel() // For debug only
{
  Q_ASSERT(model);
  model->dump();
}
