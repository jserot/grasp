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
#include <QAbstractButton>

class CompilerPaths : public QObject
{
  Q_OBJECT
public:
    CompilerPaths(QWidget *parent = 0);
    ~CompilerPaths();

    QString getPath(QString name);
    void edit(QWidget *parent);

  const QString defaultCompiler;
  const QString defaultDotProgram;
  const QString defaultDotViewer;
  const QString defaultVcdViewer;

private slots:
  void valueChanged(const QString&);
  //void buttonClicked(QAbstractButton*);
  void saveToFile();

signals:
    void compilerPathChanged(QString newPath);

private:
  void applyEditedPaths(); // Applies (and clears) the edits made in the dialog
    QWidget *parent;
    QDialog *dialog;
    QMap<QString,QString> paths;
    QMap<QString,QString> editedPaths;
    void setDefaults();
    void readFromFile(QString fname);
    void logMessage(QString msg);
};

