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

#include "automatonPanel.h"
#include "mainwindow.h"
#include "iovPanel.h"

#include <QFrame>
#include <QVBoxLayout>
#include <QGraphicsView>
#include <QRegularExpression>
#include <QDebug>

QRegularExpressionValidator *AutomatonPanel::var_name_validator;

AutomatonPanel::AutomatonPanel(Automaton *automaton, QWidget* parent) : QFrame(parent)
{
    this->automaton = automaton;

    var_name_validator = new QRegularExpressionValidator(Globals::re_lid);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignTop);
    //layout->setMinimumWidth(200);

    view = new QGraphicsView(automaton);
    automaton->setView(view);
    view->setMinimumWidth(300);
    //view->setMaximumWidth(500);
    view->setMinimumHeight(400);
    layout->addWidget(view);
    
    IovPanel::Client client;
    client.icKind = IovPanel::IcAutomaton;
    client.icClient.automaton = automaton;
    vars_panel = new IovPanel(Iov::IoVar, "Local variables", "Local variable", client, var_name_validator);
    fillVarsPanel();
    layout->addWidget(vars_panel);

    connect(vars_panel, SIGNAL(modelModified()), Globals::mainWindow, SLOT(modelModified()));
}

AutomatonPanel::~AutomatonPanel()
{
}

// [Local variables] panel

void AutomatonPanel::fillVarsPanel()
{
  Q_ASSERT(automaton);
  QList<Iov*> vars = automaton->getVars();
  foreach (Iov *var, vars)
    vars_panel->addRow((void *)(var));
}

void AutomatonPanel::clearVarsPanel()
{
  vars_panel->clear();
}

void AutomatonPanel::fill()
{
  fillVarsPanel();
}

void AutomatonPanel::clear()
{
  clearVarsPanel();
}

void AutomatonPanel::update()
{
  clear();
  fill();
}
