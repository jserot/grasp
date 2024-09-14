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

#include <QtWidgets>
#include <QFile>
#include <QMessageBox>
#include <QTabWidget>
#include <QLineEdit>
#include <QDialog>
#include <QFileDialog>

#include "qt_compat.h"
#include "compilerOptions.h"

CompilerOptions::CompilerOptions(QString specFile, QWidget *parent)
{
  this->parent = parent;
  readSpecFile(specFile);
  options.insert("-dot_no_captions", CompilerOption("dot","-dot_no_captions",true));
  // The above is a hack until the rfsmc option -dot_no_captions is replaced by -dot_captions
  // dump();
  dialog = NULL;
  tabs = NULL;
}
  
void CompilerOptions::readSpecFile(QString fname)
{
  QFile file(fname);
  file.open(QIODevice::ReadOnly);
  if ( file.error() != QFile::NoError ) {
    QMessageBox::warning(parent, "","Cannot read specification file " + file.fileName());
    return;
  }
  qDebug() << "Reading options from" << fname;
  while ( ! file.atEnd() ) {
    QString line = file.readLine();
    if ( line[0] == '#' ) continue;
    QStringList items = line.split(";");
#ifdef USE_QGV
    if ( items[2] == "-dot_external_viewer" ) continue;
#endif
    // 0: IDE flag, 1: category, 2: name, 3: kind, 5: desc
    if ( items.length() < 6 || items[0] != "ide" ) continue;
    if ( items[3] == "Arg.Unit" ) {
      CompilerOption opt(items[1], items[2], CompilerOption::UnitOpt, "", items[5].trimmed());
      options.insert(items[2], opt);
    }
    else if ( items[3] == "Arg.String" ) {
      CompilerOption opt(items[1], items[2], CompilerOption::StringOpt, "", items[5].trimmed());
      options.insert(items[2], opt);
    }
    else if ( items[3] == "Arg.Int" ) {
      CompilerOption opt(items[1], items[2], CompilerOption::IntOpt, "", items[5].trimmed());
      options.insert(items[2], opt);
    }
    else { /* TO FIX */ }
  }
  file.close();
}

void CompilerOptions::readFromFile(QString fname)
{
  QFile file(fname);
  file.open(QIODevice::ReadOnly);
  if ( file.error() != QFile::NoError ) {
    QMessageBox::warning(parent, "","Cannot read file " + file.fileName());
    return;
    }
  while ( ! file.atEnd() ) {
    QString line = file.readLine();
    QStringList items = line.split("=");
    QString name = items.at(0);
    QString txt = items.at(1).trimmed();
    // qDebug() << "Reading " << name << "=" << txt;
    if ( options.keys().contains(name) ) {
      CompilerOption opt = options.value(name);
      switch ( opt.kind ) {
      case CompilerOption::UnitOpt: opt.val = QVariant(txt=="true"); break;
      case CompilerOption::StringOpt: opt.val = txt; break;
      case CompilerOption::IntOpt: opt.val = txt.toInt(); break;
      }
      qDebug() << "Setting " << name << "=" << opt.val.toString();
      options.insert(name, opt);  // Replace
      }
    }
  file.close();
  logMessage("Read options from file " + file.fileName());
}

void CompilerOptions::edit(QWidget *parent)
{
  dialog = new QDialog(parent);
  tabs = new QTabWidget(dialog);
  addTab("General", "general", tabs, parent);
  addTab("Dot", "dot", tabs, parent);
  addTab("Sim", "sim", tabs, parent);
  addTab("SystemC", "systemc", tabs, parent);
  addTab("VHDL", "vhdl", tabs, parent);
  QVBoxLayout *mainLayout = new QVBoxLayout();
  mainLayout->addWidget(tabs);
  QDialogButtonBox *buttonBox =
    new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Save | QDialogButtonBox::Open);
  // dialog->connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
  // dialog->connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));
  dialog->connect(buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(buttonClicked(QAbstractButton*)));
  mainLayout->addWidget(buttonBox);
  dialog->setLayout(mainLayout);
  dialog->setWindowTitle("Compiler options");
  editedOptions.clear();
  if ( dialog->exec() == QDialog::Accepted ) {
    QMapIterator<QString, CompilerOption> i(editedOptions);
    while (i.hasNext()) {
      i.next();
      CompilerOption opt = options.value(i.key());
      opt.val = i.value().val;
      qDebug() << "Updating option " << i.key() << " with value " << opt.val.toString();
      options.insert(i.key(), opt); // Update
    }
  }
  delete tabs;
  delete dialog;
  dialog = NULL;
  tabs = NULL;
}

void CompilerOptions::addTab(QString title, QString category, QTabWidget *tabs, QWidget *parent)
{
    QWidget *container = new QWidget(parent);
    QFormLayout *layout = new QFormLayout(container);
    layout->setSizeConstraint(QLayout::SetFixedSize);

    QLineEdit *lineEdit;
    QCheckBox *checkBox;

    QMapIterator<QString, CompilerOption> i(options);
    while (i.hasNext()) {
      i.next();
      QLabel *name = new QLabel(i.key());
      CompilerOption opt = i.value();
      if ( opt.category != category ) continue;
      name->setToolTip(opt.desc);
      switch ( opt.kind ) {
        case CompilerOption::UnitOpt:
          checkBox = new QCheckBox();
          checkBox->setObjectName(i.key()); 
          connect(checkBox, SIGNAL(stateChanged(int)), this, SLOT(unitValueChanged(const int)));
          checkBox->setChecked(opt.val.toString()=="true");
          layout->addRow(name, checkBox);
          break;
        case CompilerOption::StringOpt:
          lineEdit = new QLineEdit();
          lineEdit->setObjectName(i.key()); 
          connect(lineEdit, SIGNAL(textEdited(const QString)), this, SLOT(stringValueChanged(const QString)));
          lineEdit->setText(opt.val.toString());
          layout->addRow(name, lineEdit);
          break;
        case CompilerOption::IntOpt:
          lineEdit = new QLineEdit();
          lineEdit->setObjectName(i.key()); 
          connect(lineEdit, SIGNAL(textEdited(const QString)), this, SLOT(intValueChanged(const QString)));
          lineEdit->setText(opt.val.toString());
          layout->addRow(name, lineEdit);
          break;
        }
      }
    container->setLayout(layout);
    QScrollArea *scroll = new QScrollArea(parent);
    scroll->setWidget(container);
    tabs->addTab(scroll, title);
}

void CompilerOptions::stringValueChanged(const QString txt)
{
  QString name = sender()->objectName();
  editedOptions.insert(name, CompilerOption("", name, txt.trimmed()));
}

void CompilerOptions::intValueChanged(const QString txt)
{
  QString name = sender()->objectName();
  editedOptions.insert(name, CompilerOption("", name, txt.toInt()));
}

void CompilerOptions::unitValueChanged(const int s)
{
  QString name = sender()->objectName();
  editedOptions.insert(name, CompilerOption("", name, s==Qt::Checked));
}
  
void CompilerOptions::buttonClicked(QAbstractButton *button)
{
  QString action = button->text();
  if ( action == "Save" ) {
    QString fname;
    fname = QFileDialog::getSaveFileName(parent, "Save options to file", "", "OPTS file (*.opts)");
    if ( fname.isEmpty() ) return;
    qDebug() << "Saving to file " << fname;
    saveToFile(fname);
    }
  else if ( action == "Open" ) {
    QString fname;
    fname = QFileDialog::getOpenFileName(parent, "Open file", "", "OPTS file (*.opts)");
    if ( fname.isEmpty() ) return;
    qDebug() << "Opening file " << fname;
    logMessage("Reading compiler options from file " + fname);
    readFromFile(fname);
    dialog->accept();  // Close ...
    this->edit(parent); // .. and re-open
    // Note: This way of doing generates the following warning:
    //    "modalSession has been exited prematurely - check for a reentrant call to endModalSession"
    // TO FIX ?
    }
  else if ( action == "OK" ) dialog->accept();
  else if ( action == "Cancel" ) dialog->reject();
}

void CompilerOptions::saveToFile(QString fname)
{
  QFileInfo fi(fname);
  QFile f(fname);
  if ( ! f.open(QFile::WriteOnly | QFile::Text) ) 
    QMessageBox::warning(parent, "","Cannot open file file " + fi.fileName() + " for writing");
  QTextStream os(&f);
  QMapIterator<QString, CompilerOption> i(options);
  while (i.hasNext()) {
    i.next();
    CompilerOption opt = i.value();
    QString v = opt.val.toString();
    qDebug() << i.key() << v;
    if ( ! v.isEmpty() ) 
      os << i.key() << "=" << v << QT_ENDL;
    }
  os.flush();
  f.close();
  qDebug() << "Compiler options saved to file " << fi.fileName();
}

QStringList CompilerOptions::getOptions(QString category)
{
  QStringList opts;
  foreach ( CompilerOption opt, options) {
    if ( opt.category == category ) {
      QString v = opt.getValue();
      if ( ! v.isEmpty() ) opts << v;
      }
    }
  return opts;
}

void CompilerOptions::logMessage(QString msg)
{
  qobject_cast<QMainWindow*>(parent)->statusBar()->showMessage(msg);
}

void CompilerOptions::dump() // For debug only
{
  QMapIterator<QString, CompilerOption> i(options);
  while (i.hasNext()) {
    i.next();
    CompilerOption opt = i.value();
    QString v = opt.val.toString();
    qDebug() << i.key() << "=" << v;
    }
}

CompilerOptions::~CompilerOptions()
{
}
