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

#include "modelProperties.h"
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
#include <QDebug>

ModelProperties::ModelProperties(Model *model, QWidget* parent) : QFrame(parent)
{
    qDebug() << "ModelProperties: model=" << model;
    this->model = model;

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setAlignment(Qt::AlignTop);

    IovPanel::Client client = { IovPanel::IcModel, { .model = this->model } }; 

    inps_panel = new IovPanel(Iov::IoIn, "Inputs", "Input", client);
    layout->addWidget(inps_panel);

    outps_panel = new IovPanel(Iov::IoOut, "Outputs", "Output", client);
    layout->addWidget(outps_panel);

    vars_panel = new IovPanel(Iov::IoVar, "Global variables", "Global variable", client);
    layout->addWidget(vars_panel);
    fill();
    this->setLayout(layout);

    connect(inps_panel, SIGNAL(modelModified()), Globals::mainWindow, SLOT(modelModified()));
    connect(outps_panel, SIGNAL(modelModified()), Globals::mainWindow, SLOT(modelModified()));
    connect(vars_panel, SIGNAL(modelModified()), Globals::mainWindow, SLOT(modelModified()));
    connect(this, SIGNAL(modelModified()), Globals::mainWindow, SLOT(modelModified()));
}

ModelProperties::~ModelProperties()
{
    qDebug() << "ModelProperties:delete";
}

void ModelProperties::fill()
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

void ModelProperties::clear()
{
  inps_panel->clear();
  outps_panel->clear();
  vars_panel->clear();
}

void ModelProperties::update()
{
  clear();
  fill();
}
