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

#pragma once

#include <QObject>
#include <QString>
#include <QMap>
#include <QTabWidget>
#include "compilerOption.h"

class CompilerOptions : public QObject
{
  Q_OBJECT
public:
    CompilerOptions(QString specFile, QWidget *parent = 0);
    ~CompilerOptions();

    void edit(QWidget *parent);
    QStringList getOptions(QString category);

private slots:
  void stringValueChanged(const QString);
  void intValueChanged(const QString);
  void unitValueChanged(const int);
  void buttonClicked(QAbstractButton *button);

private:
    QWidget *parent;
    QDialog *dialog;
    QTabWidget* tabs;
    QMap<QString,CompilerOption> options;
    QMap<QString,CompilerOption> editedOptions;
    void readSpecFile(QString fname);
    void readFromFile(QString fname);
    void saveToFile(QString fname);
    void addTab(QString title, QString category, QTabWidget *tabs, QWidget *parent);
    void logMessage(QString msg);
    void dump(); // For debug only
};

