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

#include <QtWidgets>
#include <QFile>
#include <QMessageBox>
#include <QLineEdit>
#include <QDialog>
#include <QFileDialog>
#include <QPushButton>

#include "compilerPaths.h"
#include "appFiles.h"

static const QString defaultCompiler = "rfsmc";  // Fall-back, default values
static const QString defaultDotProgram = "dot";
static const QString defaultDotViewer = "graphviz";
static const QString defaultVcdViewer = "gtkwave";
static const int defaultPathLength = 60;

CompilerPaths::CompilerPaths(QWidget *parent) : parent(parent)
{
  setDefaults();
  QString iniFile = AppFiles::iniFile();
  readFromFile(iniFile); 
  QMapIterator<QString, QString> i(paths);
  while (i.hasNext()) {
    i.next();
    qDebug() << i.key() << "=" << i.value();
    }
}

void CompilerPaths::setDefaults()
{
  paths.clear();
  paths.insert("COMPILER", defaultCompiler);
#ifndef USE_QGV
  paths.insert("DOTPROGRAM", defaultDotProgram);
  paths.insert("DOTVIEWER", defaultDotViewer);
#endif
  paths.insert("VCDVIEWER", defaultDotViewer);
  paths.insert("INITDIR", "");
}

void CompilerPaths::readFromFile(QString fname)
{
  qDebug() << "Reading paths from file " << fname;
  QFile file(fname);
  file.open(QIODevice::ReadOnly);
  if ( file.error() != QFile::NoError ) {
    logMessage("Cannot read file " + file.fileName() + ". Using default values for compiler paths");
    return;
    }
  while ( ! file.atEnd() ) {
    QString line = file.readLine();
    QStringList items = line.split("=");
    if ( items.length() == 2 && items.at(0) != "" && items.at(1) != "" ) {
      QString key = items.at(0).trimmed();
      QString val = items.at(1).trimmed();
      if ( paths.keys().contains(key) ) {
        // qDebug() << "Path " << key << "<-" << val;
        paths.insert(key, val);
        }
      }
    }
  file.close();
  logMessage("Read file " + file.fileName());
}

void CompilerPaths::edit(QWidget *parent)
{
    QDialog *dialog = new QDialog(parent);
    dialog->setWindowTitle("Compiler paths");

    QFormLayout *layout = new QFormLayout(dialog);
    layout->setSizeConstraint(QLayout::SetFixedSize);

    QMapIterator<QString, QString> i(paths);
    while (i.hasNext()) {
        i.next();
        QLabel *name = new QLabel(i.key());
        QLineEdit *path = new QLineEdit();
        path->setObjectName(i.key()); // Sender identification
        path->setMinimumWidth(path->fontMetrics().averageCharWidth()*defaultPathLength);
        connect(path, SIGNAL(textEdited(const QString&)), this, SLOT(valueChanged(const QString&)));
        path->setText(i.value());
        layout->addRow(name, path);
    }

    QDialogButtonBox *buttonBox =
        new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    QPushButton *saveButton = new QPushButton("Save", dialog);
    buttonBox->addButton(saveButton, QDialogButtonBox::ActionRole);

    QHBoxLayout *buttonLayout = new QHBoxLayout; // To center buttons
    buttonLayout->addStretch();
    buttonLayout->addWidget(buttonBox);
    buttonLayout->addStretch();

    layout->addRow(buttonLayout);

    dialog->adjustSize();

    QObject::connect(buttonBox, &QDialogButtonBox::accepted, dialog, &QDialog::accept);
    QObject::connect(buttonBox, &QDialogButtonBox::rejected, dialog, &QDialog::reject);
    QObject::connect(saveButton, &QPushButton::clicked, this, &CompilerPaths::saveToFile);

    if (dialog->exec() == QDialog::Accepted) {
        qDebug() << "Dialog accepted !";
        QMapIterator<QString, QString> i(editedPaths);
        while (i.hasNext()) {
            i.next();
            qDebug() << i.key() << "<-" << i.value();
            paths.insert(i.key(), i.value());
        }
    }

    delete dialog;
}

void CompilerPaths::valueChanged(const QString& txt)
{
  QString path = sender()->objectName();
  QString value = txt.trimmed();
  editedPaths.insert(path, value);
  if ( path == "COMPILER" )
    emit(compilerPathChanged(value));
}

void CompilerPaths::saveToFile()
{
  QString fname;
  fname = QFileDialog::getSaveFileName(parent, "Save configuration to file", "", "INI file (*.ini)");
  if ( fname.isEmpty() ) return;
  QFileInfo fi(fname);
  QFile f(fname);
  if ( ! f.open(QFile::WriteOnly | QFile::Text) ) 
    QMessageBox::warning(parent, "","Cannot open file file " + fi.fileName() + " for writing");
  QTextStream os(&f);
  QMapIterator<QString, QString> i(paths);
  while (i.hasNext()) {
    i.next();
    os << i.key().trimmed() << "=" << i.value().trimmed() << "\n";
    }
  os.flush();
  f.close();
  logMessage("Saved compiler paths to file " + fname);
}

QString CompilerPaths::getPath(QString name)
{
  return paths.contains(name) ? paths.value(name) : "";
}

void CompilerPaths::logMessage(QString msg)
{
  qobject_cast<QMainWindow*>(parent)->statusBar()->showMessage(msg);
}

  
CompilerPaths::~CompilerPaths()
{
}
