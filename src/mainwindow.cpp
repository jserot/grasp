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

#include "globals.h"
#include "transition.h"
#include "state.h"
#include "model.h"
#include "mainwindow.h"
#include "imageviewer.h"
#include "textviewer.h"
#ifdef USE_QGV
#include "dotviewer.h"
#endif
#include "compilerPaths.h"
#include "compilerOptions.h"
#include "commandExec.h"
#include "compiler.h"
#include "debug.h"
#include "stimuli.h"
#include "modelPanel.h"
#include "automatonPanel.h"
#include "nameInputDialog.h"

#include <QtWidgets>
#include <QVariant>

const QString MainWindow::title = "RFSM Light";
const QList<int> MainWindow::splitterSizes = { 250, 250, 250 };
const double MainWindow::zoomInFactor = 1.25;
const double MainWindow::zoomOutFactor = 0.8;
const double MainWindow::minScaleFactor = 0.2;
const double MainWindow::maxScaleFactor = 2.0;
// const QStringList guiOnlyOpts = { "-dot_external_viewer", "-sync_externals" };

MainWindow::MainWindow()
{
    // Non-GUI setup

    Globals::mainWindow = this;
    QString appDir = QApplication::applicationDirPath();
    qDebug() << "APPDIR=" << appDir;
    Globals::compilerPaths = new CompilerPaths(appDir + "/rfsm-light.ini", this);
    connect(Globals::compilerPaths, SIGNAL(compilerPathChanged(QString)), this, SLOT(compilerPathUpdated(QString)));
    Globals::compilerOptions = new CompilerOptions(appDir + "/options_spec.txt", this);
    Globals::initDir = Globals::compilerPaths->getPath("INITDIR");
    QString compilerPath = Globals::compilerPaths->getPath("COMPILER");
    if ( compilerPath.isNull() || compilerPath.isEmpty() ) compilerPath = "rfsmc"; // Last chance..
    Globals::compiler = new Compiler(compilerPath);
    Globals::executor = new CommandExec();

    // GUI setup

    createActions();
    createMenus();
    createToolbars();

    QSplitter *splitter = new QSplitter;

    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //sizePolicy.setHorizontalStretch(0);
    //sizePolicy.setVerticalStretch(0);
    //sizePolicy.setHeightForWidth(splitter->sizePolicy().hasHeightForWidth());
    splitter->setSizePolicy(sizePolicy);
    splitter->setMinimumSize(QSize(0, 300));
    splitter->setOrientation(Qt::Horizontal);

    setCentralWidget(splitter);

    model = new Model(QString(),this);
    Q_ASSERT(model);

    // Left panel

    model_panel = new ModelPanel(model,this); 
    model_panel->setMinimumWidth(280);
    model_panel->setMaximumWidth(360);

    splitter->addWidget(model_panel);

    // Central panel (automatons) 

    automatons_panel = new QTabWidget(splitter);
    automatons_panel->setMinimumHeight(400);
    automatons_panel->setMinimumWidth(300);
    automatons_panel->setDocumentMode(false);
    automatons_panel->setTabsClosable(true);
    automatons_panel->setMovable(true);
    addAutomatonTabs(model);

    connect(automatons_panel, SIGNAL(tabCloseRequested(int)), this, SLOT(closeAutomatonTab(int)));
    connect(automatons_panel, SIGNAL(currentChanged(int)), this, SLOT(automatonTabChanged(int)));
    //connect(automatons_panel, SIGNAL(tabBarDoubleClicked(int)), this, SLOT(automatonTabChangeName(int)));

    splitter->addWidget(automatons_panel);

    // Right panel (DOT rendering and generated code)

    results_panel = new QTabWidget(splitter);
    results_panel->setMinimumHeight(400);
    results_panel->setMinimumWidth(300);
    results_panel->setDocumentMode(false);
    results_panel->setTabsClosable(true);
    results_panel->setMovable(true);

    connect(results_panel, SIGNAL(tabCloseRequested(int)), this, SLOT(closeResultTab(int)));
    connect(results_panel, SIGNAL(currentChanged(int)), this, SLOT(resultTabChanged(int)));

    //splitter->addWidget(results_panel);

    // Status bar

    statusBar = new QStatusBar;
    setStatusBar(statusBar);

    // Final initialisations

    setWindowTitle(title);
    setUnifiedTitleAndToolBarOnMac(true);

    codeFont.setFamily("Courier");
    codeFont.setFixedPitch(true);
    codeFont.setPointSize(11);

    initCursors();

    unsaved_changes = false;
    updateActions();
    
    splitter->setSizes(splitterSizes); 

    currentScaleFactor = 1.0;
}


void MainWindow::modelModified()
{
  qDebug() << "Model modified !";
  setUnsavedChanges(true);
}

void MainWindow::compilerPathUpdated(QString path)
{
  qDebug() << "Compiler path updated to" << path;
  Globals::compiler->setPath(path);
}

void MainWindow::about()
{
    QMessageBox::about(this,
      "About RFSM Light",
      "<p>Finite State Diagram Editor, Simulator and Compiler</p>\
          <p>version " + Globals::version + "</p>\
         <p><a href=\"github.com/jserot/rfsm-light\">github.com/jserot/rfsm-light</a></p>\
         <p>(C) J. Sérot (jocelyn.serot@uca.fr), 2019-now");
}

// Actions

void MainWindow::createActions()
{
    aboutAction = new QAction(tr("About"), this);
    aboutAction->setShortcut(tr("F1"));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

    exitAction = new QAction(tr("Q&uit"), this);
    exitAction->setShortcuts(QKeySequence::Quit);
    connect(exitAction, SIGNAL(triggered()), this, SLOT(quit()));

    newModelAction = new QAction(QIcon(":/images/new.png"), tr("&New model"), this);
    newModelAction->setShortcuts(QKeySequence::New);
    newModelAction->setToolTip(tr("Start a new model"));
    connect(newModelAction, SIGNAL(triggered()), this, SLOT(newModel()));

    openFileAction = new QAction(QIcon(":/images/open.png"), "&Open model", this);
    openFileAction->setShortcut(QKeySequence::Open);
    openFileAction->setToolTip(tr("Open an existing model"));
    connect(openFileAction, SIGNAL(triggered()), this, SLOT(openFile()));
 
    saveFileAction = new QAction(QIcon(":/images/save.png"), "&Save model", this);
    saveFileAction->setShortcut(QKeySequence::Save);
    saveFileAction->setToolTip(tr("Save current model"));
    connect(saveFileAction, SIGNAL(triggered()), this, SLOT(save()));
 
    saveFileAsAction = new QAction(QIcon(":/images/save.png"), "&Save as", this);
    saveFileAsAction->setShortcut(QKeySequence::SaveAs);
    saveFileAction->setToolTip(tr("Save current model as..."));
    connect(saveFileAsAction, SIGNAL(triggered()), this, SLOT(saveAs()));
 
    checkAutomatonAction = new QAction(tr("Check automaton"), this);
    checkAutomatonAction->setShortcut(tr("Ctrl+A"));
    saveFileAction->setToolTip(tr("Check current automaton"));
    connect(checkAutomatonAction, SIGNAL(triggered()), this, SLOT(checkAutomaton()));

    checkModelAction = new QAction(tr("Check model"), this);
    checkModelAction->setShortcut(tr("Ctrl+K"));
    saveFileAction->setToolTip(tr("Check model"));
    connect(checkModelAction, SIGNAL(triggered()), this, SLOT(checkModel()));

    checkModelWithStimuliAction = new QAction(tr("Check model and stimuli"), this);
    checkModelWithStimuliAction->setShortcut(tr("Ctrl+Shift+K"));
    checkModelWithStimuliAction->setToolTip(tr("Check model and stimuli"));
    connect(checkModelWithStimuliAction, SIGNAL(triggered()), this, SLOT(checkModelWithStimuli()));

    renderDotsAction = new QAction(QIcon(":/images/compileDot.png"), tr("Generate DOT representations"), this);
    renderDotsAction->setToolTip(tr("Generate DOT representations"));
    renderDotsAction->setShortcut(tr("Ctrl+R"));
    connect(renderDotsAction, SIGNAL(triggered()), this, SLOT(renderDots()));

#ifndef USE_QGV
    renderDotAction = new QAction(QIcon(":/images/compileDot.png"), tr("Generate DOT representation"), this);
    renderDotAction->setToolTip(tr("Generate DOT representation"));
    renderDotAction->setShortcut(tr("Ctrl+Shift+R"));
    connect(renderDotAction, SIGNAL(triggered()), this, SLOT(renderDot()));
#endif

    generateRfsmModelAction = new QAction(tr("Generate RFSM code (model only)"), this);
    connect(generateRfsmModelAction, SIGNAL(triggered()), this, SLOT(generateRfsmModel()));

    generateRfsmTestbenchAction = new QAction(tr("Generate RFSM code (model+testbench) "), this);
    connect(generateRfsmTestbenchAction, SIGNAL(triggered()), this, SLOT(generateRfsmTestbench()));

    generateCTaskAction = new QAction(QIcon(":/images/compileCTask.png"),tr("Generate CTask code"), this);
    generateCTaskAction->setToolTip(tr("Generate CTask code"));
    connect(generateCTaskAction, SIGNAL(triggered()), this, SLOT(generateCTask()));

    generateSystemCModelAction = new QAction(QIcon(":/images/compileSystemC.png"),tr("Generate SystemC code (model only)"), this);
    generateSystemCModelAction->setToolTip(tr("Generate SystemC code"));
    connect(generateSystemCModelAction, SIGNAL(triggered()), this, SLOT(generateSystemCModel()));

    generateSystemCTestbenchAction = new QAction(tr("Generate SystemC code (model+testbench)"), this);
    connect(generateSystemCTestbenchAction, SIGNAL(triggered()), this, SLOT(generateSystemCTestbench()));

    generateVHDLModelAction = new QAction(QIcon(":/images/compileVHDL.png"),tr("Generate VHDL code (model only)"), this);
    generateVHDLModelAction->setToolTip(tr("Generate VHDL code"));
    connect(generateVHDLModelAction, SIGNAL(triggered()), this, SLOT(generateVHDLModel()));

    generateVHDLTestbenchAction = new QAction(tr("Generate VHDL code (model+testbench)"), this);
    connect(generateVHDLTestbenchAction, SIGNAL(triggered()), this, SLOT(generateVHDLTestbench()));

    runSimulationAction = new QAction(QIcon(":/images/runSimulation.png"),tr("Run simulator"), this);
    runSimulationAction->setToolTip(tr("Simulate and open VCD viewer"));
    connect(runSimulationAction, SIGNAL(triggered()), this, SLOT(runSimulation()));

    zoomInAction = new QAction(tr("Zoom In"), this);
    zoomInAction->setShortcut(tr("Ctrl++"));
    connect(zoomInAction, SIGNAL(triggered()), this, SLOT(zoomIn()));

    zoomOutAction = new QAction(tr("Zoom Out"), this);
    zoomOutAction->setShortcut(tr("Ctrl+-"));
    connect(zoomOutAction, SIGNAL(triggered()), this, SLOT(zoomOut()));

    normalSizeAction = new QAction(tr("Normal size (100%)"), this);
    connect(normalSizeAction, SIGNAL(triggered()), this, SLOT(normalSize()));

    fitToWindowAction = new QAction(tr("Fit to Window"), this);
    fitToWindowAction->setShortcut(tr("Ctrl+F"));
    connect(fitToWindowAction, SIGNAL(triggered()), this, SLOT(fitToWindow()));
    fitToWindowAction->setCheckable(true);

    closeResultsAction = new QAction(tr("Close all results_panel"), this);
    connect(closeResultsAction, SIGNAL(triggered()), this, SLOT(closeResultTabs()));

    pathConfigAction = new QAction(tr("Compiler and tools"), this);
    connect(pathConfigAction, SIGNAL(triggered()), this, SLOT(setCompilerPaths()));

    compilerOptionsAction = new QAction(tr("Compiler options"), this);
    connect(compilerOptionsAction, SIGNAL(triggered()), this, SLOT(setCompilerOptions()));

    fontConfigAction = new QAction(tr("Code font"), this);
    QObject::connect(fontConfigAction, SIGNAL(triggered()), this, SLOT(setCodeFont()));
    
    modelActions = new QActionGroup(this);

    addAutomatonAction = new QAction(QIcon(":/images/page.png")," Add automaton to model", modelActions);
    connect(addAutomatonAction, SIGNAL(triggered()), this, SLOT(addAutomatonToModel()));

    duplAutomatonAction = new QAction(QIcon(":/images/page.png")," Duplicate current automaton", modelActions);
    connect(duplAutomatonAction, SIGNAL(triggered()), this, SLOT(duplicateAutomaton()));

    // dumpModelAction = new QAction("Dump", modelActions); // For debug only
    // connect(dumpModelAction, SIGNAL(triggered()), this, SLOT(dumpModel())); // For debug only

    automatonActions = new QActionGroup(this);

    selectItemAction = new QAction(QIcon(":/images/select.png")," Select item", automatonActions);
    addStateAction = new QAction(QIcon(":/images/state.png")," Add state", automatonActions);
    addInitStateAction = new QAction(QIcon(":/images/initstate.png")," Add initial state", automatonActions);
    addTransitionAction = new QAction(QIcon(":/images/transition.png")," Add transition", automatonActions);
    addSelfTransitionAction = new QAction(QIcon(":/images/loop.png")," Add self transition", automatonActions);
    deleteItemAction = new QAction(QIcon(":/images/delete.png")," Delete item", automatonActions);

    selectItemAction->setData(QVariant::fromValue((int)Globals::SelectItem));
    addStateAction->setData(QVariant::fromValue((int)Globals::InsertState));
    addInitStateAction->setData(QVariant::fromValue((int)Globals::InsertPseudoState));
    addTransitionAction->setData(QVariant::fromValue((int)Globals::InsertTransition));
    addSelfTransitionAction->setData(QVariant::fromValue((int)Globals::InsertLoopTransition));
    deleteItemAction->setData(QVariant::fromValue((int)Globals::DeleteItem));

    selectItemAction->setCheckable(true);
    addStateAction->setCheckable(true);
    addInitStateAction->setCheckable(true);
    addTransitionAction->setCheckable(true);
    addSelfTransitionAction->setCheckable(true);
    deleteItemAction->setCheckable(true);

    selectItemAction->setChecked(true);
    addStateAction->setChecked(false);
    addInitStateAction->setChecked(false);
    addTransitionAction->setChecked(false);
    addSelfTransitionAction->setChecked(false);
    deleteItemAction->setChecked(false);

    connect(automatonActions, SIGNAL(triggered(QAction*)), this, SLOT(editModel(QAction*)));
}

void MainWindow::updateActions()
{
  updateViewActions(); // Nothing else for now
}

void MainWindow::updateViewActions()
{
  QWidget *widget;
  QString kind;
  if ( results_panel->count() == 0 ) {
    closeResultsAction->setEnabled(false);
    goto unselect;
    }
  closeResultsAction->setEnabled(true);
  widget = results_panel->widget(results_panel->currentIndex());  
  if ( widget == NULL ) goto unselect;
  kind = widget->metaObject()->className();
  if ( kind == "ImageViewer" ) {
    ImageViewer* viewer = static_cast<ImageViewer*>(widget);
    bool b = viewer->isFittedToWindow();
    fitToWindowAction->setEnabled(true);
    fitToWindowAction->setChecked(b);
    zoomInAction->setEnabled(!b);
    zoomOutAction->setEnabled(!b);
    normalSizeAction->setEnabled(!b);
    return;
    }
  else if ( kind == "DotViewer" ) {
    fitToWindowAction->setEnabled(true);
    fitToWindowAction->setChecked(true);
    zoomInAction->setEnabled(true);
    zoomOutAction->setEnabled(true);
    normalSizeAction->setEnabled(true);
    return;
    }
 unselect:
    fitToWindowAction->setEnabled(false);
    zoomInAction->setEnabled(false);
    zoomOutAction->setEnabled(false);
    normalSizeAction->setEnabled(false);
}

// Menus

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newModelAction);
    fileMenu->addAction(openFileAction);
    fileMenu->addAction(saveFileAction);
    fileMenu->addAction(saveFileAsAction);
    fileMenu->addAction(aboutAction);
    fileMenu->addAction(exitAction);

    modelMenu = menuBar()->addMenu(tr("&Model"));
    modelMenu->addAction(addAutomatonAction);
    modelMenu->addAction(duplAutomatonAction);
    modelMenu->addAction(checkAutomatonAction);
    modelMenu->addAction(checkModelAction);
    modelMenu->addAction(checkModelWithStimuliAction);

    compileMenu = menuBar()->addMenu(tr("&Compile"));
    compileMenu->addAction(renderDotsAction);
#ifndef USE_QGV
    compileMenu->addAction(renderDotAction);
#endif
    compileMenu->addAction(generateCTaskAction);
    compileMenu->addAction(generateSystemCModelAction);
    compileMenu->addAction(generateVHDLModelAction);
    compileMenu->addSeparator();
    compileMenu->addAction(generateSystemCTestbenchAction);
    compileMenu->addAction(generateVHDLTestbenchAction);
    compileMenu->addSeparator();
    compileMenu->addAction(generateRfsmModelAction);
    compileMenu->addAction(generateRfsmTestbenchAction);
    compileMenu->addSeparator();
    compileMenu->addAction(runSimulationAction);

    viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(zoomInAction);
    viewMenu->addAction(zoomOutAction);
    viewMenu->addAction(normalSizeAction);
    viewMenu->addAction(fitToWindowAction);
    viewMenu->addSeparator();
    viewMenu->addAction(closeResultsAction);

    configMenu = menuBar()->addMenu("&Configuration");
    configMenu->addAction(pathConfigAction);
    configMenu->addAction(compilerOptionsAction);
    configMenu->addAction(fontConfigAction);
}

void MainWindow::createToolbars()
{
     QWidget *spacer1 = new QWidget(this);
     spacer1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
     QWidget *spacer2 = new QWidget(this);
     spacer2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

     modelToolBar = addToolBar(tr("Model"));
     modelToolBar->addAction(newModelAction);
     modelToolBar->addAction(openFileAction);
     modelToolBar->addAction(saveFileAction);
     modelToolBar->addAction(addAutomatonAction);
     //modelToolBar->addAction(duplAutomatonAction);
     //modelToolBar->addAction(dumpModelAction);

     editToolBar = addToolBar(tr("Edit automaton"));
     editToolBar->addWidget(spacer1); 
     editToolBar->addAction(selectItemAction);
     editToolBar->addAction(addStateAction);
     editToolBar->addAction(addInitStateAction);
     editToolBar->addAction(addTransitionAction);
     editToolBar->addAction(addSelfTransitionAction);
     editToolBar->addAction(deleteItemAction);

     compileToolBar = addToolBar(tr("Compile"));
     compileToolBar->addWidget(spacer2);
     compileToolBar->addAction(renderDotsAction);
// #ifndef USE_QGV
//      compileToolBar->addAction(renderDotAction);
// #endif
     compileToolBar->addAction(generateCTaskAction);
     compileToolBar->addAction(generateSystemCModelAction);
     compileToolBar->addAction(generateVHDLModelAction);
     compileToolBar->addAction(runSimulationAction);
}

void MainWindow::createPropertiesPanel()
{
  model_panel = new ModelPanel(model,this);
  model_panel->setMinimumWidth(180);
  model_panel->setMaximumWidth(360);
}

// File IO

void MainWindow::setUnsavedChanges(bool unsaved_changes)
{
    this->unsaved_changes = unsaved_changes;
    setWindowTitle(unsaved_changes ? title + " (Unsaved changes)" : title);
}

void MainWindow::checkUnsavedChanges()
{
    if ( unsaved_changes ) {
        QMessageBox save_message;
        save_message.setText("Do you want to save your changes?");
        save_message.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        save_message.setDefaultButton(QMessageBox::Save);
        switch( save_message.exec() ) {
            case QMessageBox::Cancel: return;
            case QMessageBox::Save: save();
            }
        }
}

void MainWindow::openFile()
{
    checkUnsavedChanges();
    
    QString fname = QFileDialog::getOpenFileName(this, "Open file", Globals::initDir, "FSD file (*.fsd)");
    if ( fname.isEmpty() ) return;
    try {
      model->readFromFile(fname);
      }
    catch(const std::exception& e) {
      QMessageBox::warning(this, "Error", "Unable to import : " + QString(e.what()));
      return;
      }
    closeAutomatonTabs();
    addAutomatonTabs(model);
    model_panel->update();
    currentFileName = fname;
    setUnsavedChanges(false);
}

bool MainWindow::checkAutomaton()
{ 
  if ( ! model ) return true;
  int index = automatons_panel->currentIndex();
  if ( index < 0 ) return true;
  QWidget *panel = automatons_panel->widget(index);
  Q_ASSERT(panel);
  Automaton *automaton = panelToAutomaton.value(panel);
  Q_ASSERT(automaton);
  qDebug() << "Checking automaton" << automaton->getName();
  QList<Iov*> global_ios = model->getIos();
  return automaton->check(global_ios);
}

bool MainWindow::checkModel()
{ 
  return model ? model->check(false) : true;
}

bool MainWindow::checkModelWithStimuli()
{ 
  return model ? model->check(true) : true;
}

void MainWindow::newModel()
{
  checkUnsavedChanges();
  model_panel->clear();
  closeAutomatonTabs();
  model->clear();
  currentFileName.clear();
  setUnsavedChanges(false);
}

void MainWindow::saveToFile(QString fname)
{
  model->saveToFile(fname);
  logMessage("Saved file " + fname);
  setUnsavedChanges(false);
}

void MainWindow::save()
{
  if ( model->getName().isEmpty() ) {
      QMessageBox::warning(Globals::mainWindow, "Error", "Please give a name to the model before saving it.");
      return;
      }
  if ( currentFileName.isEmpty() ) saveAs();
  else saveToFile(currentFileName);
}

void MainWindow::saveAs()
{
  QString fname = QFileDialog::getSaveFileName( this, "Save to file", "", "FSM file (*.fsd)");
  if ( fname.isEmpty() ) return;
  saveToFile(fname);
  currentFileName = fname;
}

// Model and automaton editing

void MainWindow::addAutomatonToModel()
{
  Automaton *automaton = new Automaton(model);
  model->addAutomaton(automaton);
  //model->focus = automaton;
  addAutomatonTab(automaton);
  setUnsavedChanges(true);
}

void MainWindow::duplicateAutomaton()
{
  int index = automatons_panel->currentIndex();
  if ( index < 0 ) return;
  QWidget *panel = automatons_panel->widget(index);
  Q_ASSERT(panel);
  Automaton *automaton = panelToAutomaton.value(panel);
  Q_ASSERT(automaton);
  Automaton *automaton2 = automaton->duplicate();
  model->addAutomaton(automaton2);
  addAutomatonTab(automaton2);
  setUnsavedChanges(true);
}

void MainWindow::editModel(QAction *action)
{
  Globals::mode = static_cast<Globals::Mode>(action->data().value<int>());
}

// Generating result files

QString MainWindow::getCurrentFileName()
{
  if ( currentFileName.isEmpty() )
    return QFileDialog::getSaveFileName( this, "Please save source file before compiling", "", "FSD file (*.fsd)");
  else
    return currentFileName;
}

QString changeSuffix(QString fname, QString suffix)
{
  QFileInfo f(fname);
  return f.path() + "/" + f.completeBaseName() + suffix;
}

QString removeSuffix(QString fname)
{
  QFileInfo f(fname);
  return f.path() + "/" + f.completeBaseName();
}

void MainWindow::renderDots()
{
    QString sFname = getCurrentFileName();
    qDebug() << "renderDots" << sFname;
    if ( sFname.isEmpty() ) return;
    QString basename = removeSuffix(sFname);
    QStringList opts = Globals::compilerOptions->getOptions("dot");
    QStringList rfnames = model->exportDots(basename,opts);
    for ( QString rfname: rfnames ) {
      logMessage("Wrote file " + rfname);
      openResultFile(rfname);
      }
}

#ifndef USE_QGV
void MainWindow::renderDot()
{
    QString sFname = getCurrentFileName();
    if ( sFname.isEmpty() ) return;
    QString rFname = changeSuffix(sFname, ".dot");
    QStringList opts = Globals::compilerOptions->getOptions("dot");
    model->exportDot(rFname, opts);
    logMessage("Wrote file " + rFname);
    openResultFile(rFname);
}
#endif

QString MainWindow::generateRfsm(bool withTestbench ) // TODO : factorize
{
  if ( ! checkModelWithStimuli() ) return "";
  QString sFname = getCurrentFileName();
  if ( sFname.isEmpty() ) return "";
  QString rFname = changeSuffix(sFname, ".fsm");
  model->exportRfsm(rFname, withTestbench);
  return rFname;
}

void MainWindow::generateRfsmModel()
{
  QString rFname = generateRfsm(false);
  if ( ! rFname.isEmpty() ) {
    logMessage("Wrote file " + rFname);
    openResultFile(rFname);
    }
}

void MainWindow::generateRfsmTestbench()
{
  QString rFname = generateRfsm(true);
  if ( ! rFname.isEmpty() ) {
    logMessage("Wrote file " + rFname);
    openResultFile(rFname);
    }
}

// Central panel (automatons)

void MainWindow::addAutomatonTab(Automaton* automaton)
{
  AutomatonPanel *automaton_panel = new AutomatonPanel(automaton,automatons_panel);
  automatons_panel->addTab(automaton_panel, automaton->getName());
  automatons_panel->setCurrentIndex(automatons_panel->count()-1);
  panelToAutomaton.insert(automaton_panel,automaton);
}


void MainWindow::addAutomatonTabs(Model *model)
{
  for ( Automaton* automaton: model->getAutomatons() )
    addAutomatonTab(automaton);
}

void MainWindow::closeAutomatonTab(int index)
{
  qDebug() << "Closing automaton tab" << index;
  QWidget *panel = automatons_panel->widget(index);
  Q_ASSERT(panel);
  Automaton *automaton = panelToAutomaton.value(panel);
  Q_ASSERT(automaton);
  qDebug() << "** Removing automaton" << automaton->getName();
  automatons_panel->removeTab(index);
  model->removeAutomaton(automaton);
  panelToAutomaton.remove(panel);
  delete panel; // removeTab does _not_ delete the tabbed widget
}

void MainWindow::closeAutomatonTabs()
{
  while ( automatons_panel->count() > 0 )
    closeAutomatonTab(automatons_panel->currentIndex());
  //updateActions();
}

void MainWindow::automatonTabChanged(int index)
{
  Q_UNUSED(index);
  //updateActions();
}

void MainWindow::automatonTabChangeName(int index)
{
  QWidget *panel = automatons_panel->widget(index);
  Q_ASSERT(panel);
  Automaton *automaton = panelToAutomaton.value(panel);
  Q_ASSERT(automaton);
  qDebug() << "** Changing name for automaton" << index;
  NameInputDialog dialog(automaton->getName(),panel);
  if ( dialog.exec() == QDialog::Accepted ) {
    QString name = dialog.getResult();
    automaton->setName(name);
    automatons_panel->setTabText(index,name);
    modelModified();
    }
}

// Right panel (DOT rendering and generated code)

void MainWindow::addResultTab(QString fname)
{
  QFile file(fname);
  if ( ! file.open(QIODevice::ReadOnly | QIODevice::Text) ) {
      QMessageBox::warning(this,"Error:","cannot open file:\n"+fname);
      return;
    }
  QFileInfo f(fname);
  QString tabName = f.suffix() == "gif" ? changeSuffix(f.fileName(),".dot") : f.fileName();
  for ( int i=0; i<results_panel->count(); i++ )
    if ( results_panel->tabText(i) == tabName ) closeResultTab(i); // Do not open two tabs with the same name
  if ( f.suffix() == "gif" ) {
    QPixmap pixmap(f.filePath());
    ImageViewer *viewer = new ImageViewer(pixmap, results_panel);
    results_panel->addTab(viewer, tabName);
    } 
  else {
    TextViewer *viewer = new TextViewer(file, codeFont, results_panel);
    results_panel->addTab(viewer, tabName);
    }
  results_panel->setCurrentIndex(results_panel->count()-1);
}

void MainWindow::closeResultTab(int index) // TODO: factorize with automatons tab
{
  QWidget *w = results_panel->widget(index);
  results_panel->removeTab(index);
  if ( w ) delete w; // removeTab does _not_ delete the tabbed widget
}

void MainWindow::closeResultTabs()
{
  while ( results_panel->count() > 0 )
    closeResultTab(results_panel->currentIndex());
  updateActions();
}

void MainWindow::openResultFile(QString fname)
{
  QFileInfo f(fname);
  QString wDir = f.canonicalPath();
  qDebug() << "Displaying file : " << fname;
  QStringList genOpts = Globals::compilerOptions->getOptions("general");
  QStringList args = { fname };
  if ( f.suffix() == "dot" ) {
    if ( genOpts.contains("-dot_external_viewer") )
      customView("DOTVIEWER", args, wDir, true);
    else {
      if ( dotTransform(f, wDir) )
        openResultFile(changeSuffix(fname, ".gif"));
      }
    }
  else if ( f.suffix() == "vcd" ) {
    QString gFile = changeSuffix(fname, ".gtkw");
    QFile gf(gFile);
    if ( gf.exists() ) args << gFile;
    customView("VCDVIEWER", args, wDir, true);
    }
  else {
    addResultTab(fname);
    }
}

void MainWindow::customView(QString toolName, QStringList args, QString wDir, bool detach)
{
   QString cmd = Globals::compilerPaths->getPath(toolName);
   if ( cmd.isNull() || cmd.isEmpty() ) {
     QMessageBox::warning(this, "", "No path specified for " + toolName);
     return;
     }
  QStringList genOpts = Globals::compilerOptions->getOptions("general");
  if ( ! Globals::executor->execute(wDir, cmd, args, detach) ) {
    QMessageBox::warning(this, "", "Failed to launch external program " + toolName + " (" + cmd + ")");
     }
}

void MainWindow::resultTabChanged(int index)
{
  Q_UNUSED(index);
  updateActions();
}

void MainWindow::generate(QString target, bool withTestbench)
{
  QString fname = generateRfsm(withTestbench);
  QFileInfo fi(fname);
  if ( fname.isEmpty() ) return;
  qDebug () << "generate.fname = " << fname;
  QString wDir = QFileInfo(fname).absolutePath();
  QStringList genOpts = Globals::compilerOptions->getOptions("general");
  QString targetDir = ".";
  if ( target != "sim" && genOpts.contains("-target_dirs") ) {
    targetDir = target;
    QString targetPath = wDir + "/" + target; // TO FIX : do not use raw, OS-dependent "/" in file path
    genOpts.removeOne("-target_dirs");
    QDir dir(targetPath);
    if ( ! dir.exists() ) {
      qDebug() << "Creating directory " << targetPath;
      QDir().mkdir(targetPath);
      }
    }
  foreach ( QString opt, genOpts)
    if ( genOpts.contains(opt) ) genOpts.removeOne(opt);
  QString mainName = model->getName().isEmpty() ? "main" : model->getName();
  QStringList args =
    QStringList()
    << "-" + target
    << "-main" << mainName
    << "-target_dir" << targetDir
    << genOpts
    << Globals::compilerOptions->getOptions(target);
  //if ( target == "sim" ) args << "-main" <<  fi.baseName();
  if ( target == "ctask" || target == "systemc" ) args << "-show_models";
  if ( Globals::compiler->run(fi.fileName(), args, wDir) ) {
    QStringList resFiles = Globals::compiler->getOutputFiles(target, wDir, mainName); 
    if ( ! resFiles.isEmpty() ) {
    logMessage("Generated file(s) : " + resFiles.join(", "));
    foreach ( QString rFile, resFiles) 
      openResultFile(rFile);
      }
    }
  else {
    QStringList compileErrors = Globals::compiler->getErrors();
    QMessageBox::warning(this, "", "Error when compiling model\n" + compileErrors.join("\n"));
    }
  updateActions();
}

void MainWindow::generateCTask() { generate("ctask", false); }

void MainWindow::generateSystemCModel() { generate("systemc", false); }
void MainWindow::generateSystemCTestbench() { generate("systemc", true); }

void MainWindow::generateVHDLModel() { generate("vhdl", false); }
void MainWindow::generateVHDLTestbench() { generate("vhdl", true); }

void MainWindow::runSimulation() { generate("sim", true); }

bool MainWindow::dotTransform(QFileInfo f, QString wDir)
{
  QString dotProgram = Globals::compilerPaths->getPath("DOTPROGRAM");
  if ( dotProgram.isNull() || dotProgram.isEmpty() ) dotProgram = "dot"; // Last chance..
  QString srcFile = f.filePath();
  QString dstFile = changeSuffix(srcFile, ".gif");
  //QString opts = ""; // getOption("-dot_options");
  QString wdir = f.canonicalPath();
  QStringList args = { "-Tgif",  "-o", dstFile, srcFile };
  if ( Globals::executor->execute(wDir, dotProgram, args) )
    return true;
  else {
    QMessageBox::warning(this, "", "Failed to run DOT program");
    return false;
    }
}

void MainWindow::setCodeFont()
{
  bool ok;
  QFont font = QFontDialog::getFont(&ok, QFont("Courier", 10), this);
  //qDebug() << "Got font " << font.toString();
  if ( ok ) {
    for ( int i=0; i<results_panel->count(); i++ )
      (static_cast<QPlainTextEdit*>(results_panel->widget(i)))->document()->setDefaultFont(font);
    codeFont = font;
    }
}

void MainWindow::zoomIn()
{
  scaleImage(zoomInFactor);
}


void MainWindow::zoomOut()
{
  scaleImage(zoomOutFactor);
}

void MainWindow::normalSize()
{
  QWidget *w = selectedTab();
  QString k = w->metaObject()->className();
  if ( k == "ImageViewer" ) {
    ImageViewer* viewer = static_cast<ImageViewer*>(w);
    if ( viewer == NULL ) return;
    viewer->normalSize();
    }
  // updateSelectedTabTitle(); // TODO ? 
}

void MainWindow::fitToWindow()
{
  QWidget *w = selectedTab();
  QString k = w->metaObject()->className();
  if ( k == "ImageViewer" ) {
    ImageViewer* viewer = static_cast<ImageViewer*>(w);
    if ( viewer == NULL ) return;
    viewer->fitToWindow(fitToWindowAction->isChecked() );
    }
  // updateSelectedTabTitle(); // TODO ? 
  //updateViewActions(viewer);
}

QWidget* MainWindow::selectedTab()
{
  int i = results_panel->currentIndex();
  if ( i < 0 ) return NULL;
  QWidget *tab = results_panel->widget(i);
  return tab;
}

void MainWindow::scaleImage(double factor)
{
  QWidget *w = selectedTab();
  QString k = w->metaObject()->className();
  currentScaleFactor = factor * currentScaleFactor;
  if ( k == "ImageViewer" ) {
    ImageViewer* viewer = static_cast<ImageViewer*>(w);
    if ( viewer == NULL ) return;
    viewer->scaleImage(currentScaleFactor); // Absolute scaling
   }
  else if ( k == "DotViewer" ) {
    QGraphicsView* dotView = static_cast<QGraphicsView*>(w);
    if ( dotView == NULL ) return;
    dotView->scale(factor, factor); // Relative scaling
  }
  zoomInAction->setEnabled(currentScaleFactor < maxScaleFactor);
  zoomOutAction->setEnabled(currentScaleFactor > minScaleFactor);
  // updateSelectedTabTitle(); // TODO ? 
}

#ifdef USE_QGV
void MainWindow::addDotTab(void)
{
  QString tabName = "dot";
  for ( int i=0; i<results_panel->count(); i++ )
    if ( results_panel->tabText(i) == tabName ) closeResultTab(i); // Do not open two tabs with the same name
  DotViewer *view = new DotViewer(model, 200, 400, results_panel); // TODO: let the dotViewer class decide of the canvas dimensions ?
  results_panel->addTab(view, tabName);
  results_panel->setCurrentIndex(results_panel->count()-1);
}
#endif

// Dynamic cursor handling (since 1.3.0)

void MainWindow::initCursors()
{
  default_cursor = Qt::ArrowCursor;
  cursors[Globals::InsertState] = QCursor(QPixmap(":cursors/state.png"),0,0);
  cursors[Globals::InsertPseudoState] = QCursor(QPixmap(":cursors/initstate.png"),0,0);
  cursors[Globals::InsertTransition] = QCursor(QPixmap(":cursors/transition.png"),0,0);
  cursors[Globals::InsertLoopTransition] = QCursor(QPixmap(":cursors/loop.png"),0,0);
  cursors[Globals::DeleteItem] = QCursor(QPixmap(":cursors/delete.png"),0,0);
}

void MainWindow::updateCursor()
{
  QCursor cursor = cursors.contains(Globals::mode) ? cursors.value(Globals::mode) : default_cursor;
  setCursor(cursor);
  // setCursor(Qt::PointingHandCursor);
}

void MainWindow::resetCursor()
{
  setCursor(default_cursor);
  //setCursor(Qt::PointingHandCursor);
}

// Configuration

void MainWindow::setCompilerPaths()
{
  Globals::compilerPaths->edit(this);
}

void MainWindow::setCompilerOptions()
{
  Globals::compilerOptions->edit(this);
  QStringList opts = Globals::compilerOptions->getOptions("general");
  traceMode = opts.contains("-debug");
  //if ( traceMode ) qDebug() << "Debug mode activated";
  //else qDebug() << "Debug mode desactivated";
}

// Logging 

void MainWindow::logMessage(QString msg)
{
  statusBar->showMessage(msg);
}

void MainWindow::dumpModel(void) // For debug only
{
  model->dump(); 
}

// Bye

void MainWindow::quit()
{
    checkUnsavedChanges();
    close();
}
