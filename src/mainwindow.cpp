/***********************************************************************/
/*                                                                     */
/*       This file is part of the Grasp software package               */
/*                                                                     */
/*  Copyright (c) 2025-present, Jocelyn SEROT (jocelyn.serot@uca.fr)   */
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
#include "imageViewer.h"
#include "textViewer.h"
#include "textsViewer.h"
#include "compilerPaths.h"
#include "compilerOptions.h"
#include "commandExec.h"
#include "compiler.h"
#include "debug.h"
#include "stimuli.h"
#include "modelProperties.h"

#include <QtWidgets>
#include <QVariant>
#include <QListWidget>

const QString MainWindow::title = "Grasp";

MainWindow::MainWindow()
{
    // Non-GUI setup

    Globals::mainWindow = this;
    QString appDir = QApplication::applicationDirPath();
#ifdef Q_OS_LINUX
    appDir.replace("bin","share/grasp");
#endif
    qDebug() << "APPDIR=" << appDir;
    Globals::compilerPaths = new CompilerPaths(appDir + "/grasp.ini", this);
    connect(Globals::compilerPaths, SIGNAL(compilerPathChanged(QString)), this, SLOT(compilerPathUpdated(QString)));
    Globals::compilerOptions = new CompilerOptions(appDir + "/options_spec.txt", this);
    Globals::initDir = Globals::compilerPaths->getPath("INITDIR");

    // Start compiler server
    QString compilerPath = Globals::compilerPaths->getPath("COMPILER");
    if ( compilerPath.isNull() || compilerPath.isEmpty() ) compilerPath = "rfsmc"; // Last chance..
    Globals::compiler = new Compiler();
    connect(Globals::compiler, &Compiler::serverError, this, &MainWindow::serverError);
    Globals::compiler->startServer(compilerPath,Globals::socketPath);
    QTimer::singleShot(300, this, [this]() { checkCompilerVersion(); }); // Allow 300ms delay for server to start up

    Globals::executor = new CommandExec(); // For other programs (graphviz, ...)

    
    model = NULL;

    // GUI setup

    createActions();
    createMenus();
    createToolbar();

    QHBoxLayout *layout = new QHBoxLayout;

    // Diagram tabs

    diagrams = new QTabWidget();
    diagrams->setMinimumHeight(400);
    diagrams->setMinimumWidth(300);
    diagrams->setDocumentMode(false);
    diagrams->setTabsClosable(true);
    diagrams->setMovable(true);
    Globals::diagrams = diagrams;

    connect(diagrams, SIGNAL(tabCloseRequested(int)), this, SLOT(closeDiagram(int)));
    //connect(diagrams, SIGNAL(currentChanged(int)), this, SLOT(diagramTabChanged(int)));
    //connect(diagrams, SIGNAL(tabBarDoubleClicked(int)), this, SLOT(diagramTabChangeName(int)));

    layout->addWidget(diagrams);

    //setCentralWidget(diagrams);
    QWidget *widget = new QWidget;
    widget->setLayout(layout);
    setCentralWidget(widget);

    // Dock window
    dock = NULL; // The dock window will be created (by [createDockWindow]) when creating or reading a model from file

    // Status bar

    statusBar = new QStatusBar;
    setStatusBar(statusBar);

    // Final initialisations

    setWindowTitle(title);
    setUnifiedTitleAndToolBarOnMac(true);

    initCursors();

    updateActions(); // Most of actions will be disabled before a model is created / read from file

    unsaved_changes = false;
}

MainWindow::~MainWindow()
{
  if ( Globals::compiler ) {
    Globals::compiler->close();
    delete Globals::compiler; // This will shutdown the compiler server
    }
}

Diagram *MainWindow::currentDiagram()
{
  int index = diagrams->currentIndex();
  Q_ASSERT(index >= 0);  // TO CHECK : this fn cannot be called if there's no diagram
  QGraphicsView *view = qobject_cast<QGraphicsView*>(diagrams->widget(index));
  Q_ASSERT(view);
  return qobject_cast<Diagram*>(view->scene());
}

void MainWindow::modelModified()
{
  qDebug() << "MainWindow::modelModified called by " << sender();
  updateActions();
  setUnsavedChanges(true);
}

void MainWindow::compilerPathUpdated(QString path)
{
  qDebug() << "Compiler path updated to" << path;
  // Globals::compiler->setPath(path);
  // TO FIX : close and relaunch server here !
}

void MainWindow::about()
{
    QMessageBox::about(this,
      "About Grasp",
      "<p>Finite State Diagram Editor, Simulator and Compiler</p>\
          <p>version " + Globals::version + "</p>\
         <p><a href=\"github.com/jserot/grasp\">github.com/jserot/grasp</a></p>\
         <p>(C) J. Sérot (jocelyn.serot@uca.fr), 2025-now");
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
 
    checkDiagramAction = new QAction(tr("Check current diagram"), this);
    checkDiagramAction->setShortcut(tr("Ctrl+A"));
    saveFileAction->setToolTip(tr("Check current diagram"));
    connect(checkDiagramAction, SIGNAL(triggered()), this, SLOT(checkDiagram()));

    checkModelAction = new QAction(tr("Check model"), this);
    checkModelAction->setShortcut(tr("Ctrl+K"));
    saveFileAction->setToolTip(tr("Check model"));
    connect(checkModelAction, SIGNAL(triggered()), this, SLOT(checkModel()));

    checkModelWithStimuliAction = new QAction(tr("Check model and stimuli"), this);
    checkModelWithStimuliAction->setShortcut(tr("Ctrl+Shift+K"));
    // checkModelWithStimuliAction->setToolTip(tr("Check model and stimuli"));
    connect(checkModelWithStimuliAction, SIGNAL(triggered()), this, SLOT(checkModelWithStimuli()));

    renderDotAction = new QAction(QIcon(":/images/compileDot.png"), tr("Generate DOT representation"), this);
    renderDotAction->setToolTip(tr("Generate single DOT representation (with diagrams as sub-graphs)"));
    renderDotAction->setShortcut(tr("Ctrl+R"));
    connect(renderDotAction, SIGNAL(triggered()), this, SLOT(renderDot()));

    renderDotsAction = new QAction(QIcon(":/images/compileDot.png"), tr("Generate separate DOT representations"), this);
    renderDotsAction->setToolTip(tr("Generate separate DOT representations (one graph per diagram)"));
    renderDotsAction->setShortcut(tr("Ctrl+Shift+R"));
    connect(renderDotsAction, SIGNAL(triggered()), this, SLOT(renderDots()));

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

    pathConfigAction = new QAction(tr("Compiler and tools"), this);
    connect(pathConfigAction, SIGNAL(triggered()), this, SLOT(setCompilerPaths()));

    compilerOptionsAction = new QAction(tr("Compiler options"), this);
    connect(compilerOptionsAction, SIGNAL(triggered()), this, SLOT(setCompilerOptions()));

    modelActions = new QActionGroup(this);

    addDiagramAction = new QAction(QIcon(":/images/page.png")," Add diagram to model", modelActions);
    connect(addDiagramAction, SIGNAL(triggered()), this, SLOT(newDiagram()));

    duplDiagramAction = new QAction(QIcon(":/images/page.png")," Duplicate current diagram", modelActions);
    connect(duplDiagramAction, SIGNAL(triggered()), this, SLOT(duplicateDiagram()));

    // dumpModelAction = new QAction("Dump", modelActions); // For debug only
    // connect(dumpModelAction, SIGNAL(triggered()), this, SLOT(dumpModel())); // For debug only

    diagramActions = new QActionGroup(this);

    selectItemAction = new QAction(QIcon(":/images/select.png")," Select item", diagramActions);
    addStateAction = new QAction(QIcon(":/images/state.png")," Add state", diagramActions);
    addInitStateAction = new QAction(QIcon(":/images/initstate.png")," Add initial state", diagramActions);
    addTransitionAction = new QAction(QIcon(":/images/transition.png")," Add transition", diagramActions);
    addSelfTransitionAction = new QAction(QIcon(":/images/loop.png")," Add self transition", diagramActions);
    deleteItemAction = new QAction(QIcon(":/images/delete.png")," Delete item", diagramActions);

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

    connect(diagramActions, SIGNAL(triggered(QAction*)), this, SLOT(editDiagram(QAction*)));
}

void MainWindow::updateActions()
{
    bool enabled = model;
    saveFileAction->setEnabled(enabled);
    saveFileAsAction->setEnabled(enabled);
    checkDiagramAction->setEnabled(enabled && Globals::check_model);
    checkModelAction->setEnabled(enabled && Globals::check_model);
    checkModelWithStimuliAction->setEnabled(enabled && Globals::check_model);
    // dumpModelAction->setEnabled(enabled);
    renderDotAction->setEnabled(enabled);
    renderDotsAction->setEnabled(enabled);
    generateRfsmModelAction->setEnabled(enabled);
    generateRfsmTestbenchAction->setEnabled(enabled);
    generateCTaskAction->setEnabled(enabled);
    generateSystemCModelAction->setEnabled(enabled);
    generateSystemCTestbenchAction->setEnabled(enabled);
    generateVHDLModelAction->setEnabled(enabled);
    generateVHDLTestbenchAction->setEnabled(enabled);
    runSimulationAction->setEnabled(enabled);
    addDiagramAction->setEnabled(enabled);
    duplDiagramAction->setEnabled(enabled);
    selectItemAction->setEnabled(enabled);
    addStateAction->setEnabled(enabled);
    addInitStateAction->setEnabled(enabled);
    addTransitionAction->setEnabled(enabled);
    addSelfTransitionAction->setEnabled(enabled);
    deleteItemAction->setEnabled(enabled);
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
    modelMenu->addAction(addDiagramAction);
    modelMenu->addAction(duplDiagramAction);
    modelMenu->addAction(checkDiagramAction);
    modelMenu->addAction(checkModelAction);
    modelMenu->addAction(checkModelWithStimuliAction);
    // modelMenu->addAction(dumpModelAction); // For debug only

    compileMenu = menuBar()->addMenu(tr("&Compile"));
    compileMenu->addAction(renderDotAction);
    compileMenu->addAction(renderDotsAction);
    compileMenu->addSeparator();
    compileMenu->addAction(generateCTaskAction);
    compileMenu->addSeparator();
    compileMenu->addAction(generateSystemCModelAction);
    compileMenu->addAction(generateSystemCTestbenchAction);
    compileMenu->addSeparator();
    compileMenu->addAction(generateVHDLModelAction);
    compileMenu->addAction(generateVHDLTestbenchAction);
    compileMenu->addSeparator();
    compileMenu->addAction(generateRfsmModelAction);
    compileMenu->addAction(generateRfsmTestbenchAction);
    compileMenu->addSeparator();
    compileMenu->addAction(runSimulationAction);

    viewMenu = menuBar()->addMenu(tr("&View"));

    configMenu = menuBar()->addMenu("&Configuration");
    configMenu->addAction(pathConfigAction);
    configMenu->addAction(compilerOptionsAction);
}

void MainWindow::createToolbar()
{
     QWidget *spacer1 = new QWidget(this);
     spacer1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
     QWidget *spacer2 = new QWidget(this);
     spacer2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

     editToolBar = addToolBar(tr("Edit diagram"));
     editToolBar->addWidget(spacer1);
     editToolBar->addAction(selectItemAction);
     editToolBar->addAction(addStateAction);
     editToolBar->addAction(addInitStateAction);
     editToolBar->addAction(addTransitionAction);
     editToolBar->addAction(addSelfTransitionAction);
     editToolBar->addAction(deleteItemAction);
     editToolBar->addWidget(spacer2);
}

// Dock for model IOs

void MainWindow::createDockWindow(Model *model)
{
    if ( dock ) delete dock;

    dock = new QDockWidget(tr("Model IOs"), this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    model_panel = new ModelProperties(model, dock);
    dock->setWidget(model_panel);
    addDockWidget(Qt::RightDockWidgetArea, dock);
    viewMenu->addAction(dock->toggleViewAction());
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
    Model *new_model = Model::readFromFile(fname);
    if ( new_model ) {
      qDebug() << "MainWindow::openFile: read model completed";
      new_model->dump();
      if ( model ) delete model; // This also deletes the enclosed diagrams
      model = new_model;
      closeDiagramTabs();
      addDiagramTabs();
      createDockWindow(model);
      currentFileName = fname;
      setUnsavedChanges(false);
      updateActions();
      }
    else {
      QMessageBox::warning(this, "Error", "Error when reading file " + fname);
      // Leave current model unchanged
      }
}

bool MainWindow::checkDiagram()
{ 
  Q_ASSERT(model); 
  Diagram *diagram = currentDiagram();
  Q_ASSERT(diagram);
  qDebug() << "Checking diagram" << diagram->getName();
  return diagram->check();
}

bool MainWindow::checkModel()
{ 
  Q_ASSERT(model);
  bool r = Globals::check_model ? model->check(false) : true;
  qDebug() << "Checking model: " << r;
  return r;
}

bool MainWindow::checkModelWithStimuli()
{ 
  Q_ASSERT(model); 
  bool r = Globals::check_model ? model->check(true) : true;
  qDebug() << "Checking model with stimuli: " << r;
  return r;
}

void MainWindow::newModel()
{
  checkUnsavedChanges();
  if ( model ) delete model; // This will delete all the enclosed diagrams
  model = new Model;
  closeDiagramTabs();
  createDockWindow(model);
  newDiagram();
  currentFileName.clear();
  setUnsavedChanges(false);
  updateActions();
}

void MainWindow::saveToFile(QString fname)
{
  Q_ASSERT(model); 
  model->saveToFile(fname);
  logMessage("Saved file " + fname);
  setUnsavedChanges(false);
}

void MainWindow::save()
{
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

// Model and diagram editing

void MainWindow::addDiagramTab(Diagram* diagram)
{
  Q_ASSERT(model);
  QGraphicsView* view = diagram->getView();
  QString name = diagram->getName();
  int index = diagrams->addTab(view, name);
  diagrams->setCurrentIndex(index);
  qDebug() << "MainWindow::addDiagramTab: view=" << view;
  setUnsavedChanges(true);
}

void MainWindow::newDiagram()
{
  Q_ASSERT(model); 
  QString name = "A" + QString::number(diagrams->count());
  Diagram *diagram = new Diagram(model, name, diagrams);
  model->addDiagram(diagram);
  addDiagramTab(diagram);
  setUnsavedChanges(true);
}

void MainWindow::duplicateDiagram()
{
  Q_ASSERT(model); 
  Diagram *diagram = currentDiagram();
  Q_ASSERT(diagram);
  Diagram *diagram2 = diagram->duplicate();
  model->addDiagram(diagram2);
  addDiagramTab(diagram2);
  diagram2->edit();
  setUnsavedChanges(true);
}

void MainWindow::editDiagram(QAction *action)
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
    Q_ASSERT(model); 
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

void MainWindow::renderDot()
{
    Q_ASSERT(model); 
    QString sFname = getCurrentFileName();
    if ( sFname.isEmpty() ) return;
    QString rFname = changeSuffix(sFname, ".dot");
    QStringList opts = Globals::compilerOptions->getOptions("dot");
    model->exportDot(rFname, opts);
    logMessage("Wrote file " + rFname);
    openResultFile(rFname);
}

QString MainWindow::generateRfsm(bool withTestbench ) // TODO : factorize
{
  Q_ASSERT(model); 
  if ( Globals::check_model ) {
    if ( withTestbench && ! checkModelWithStimuli() ) return "";
    if ( ! withTestbench && ! checkModel() ) return "";
    }
  QString sFname = getCurrentFileName();
  if ( sFname.isEmpty() ) return "";
  QString rFname = changeSuffix(sFname, ".fsm");
  model->exportRfsm(rFname, withTestbench);
  return rFname;
}

void MainWindow::generateRfsmModel()
{
  Q_ASSERT(model); 
  QString rFname = generateRfsm(false);
  if ( ! rFname.isEmpty() ) {
    logMessage("Wrote file " + rFname);
    openResultFile(rFname);
    }
}

void MainWindow::generateRfsmTestbench()
{
  Q_ASSERT(model); 
  QString rFname = generateRfsm(true);
  if ( ! rFname.isEmpty() ) {
    logMessage("Wrote file " + rFname);
    openResultFile(rFname);
    }
}

// Diagrams

Diagram* MainWindow::diagramOf(int index)
{
  QGraphicsView *view = qobject_cast<QGraphicsView*>(diagrams->widget(index));
  Q_ASSERT(view);
  //Diagram* diagram = viewToDiagram.value(view);
  Diagram* diagram = qobject_cast<Diagram*>(view->scene());
  Q_ASSERT(diagram);
  return diagram;
}

void MainWindow::addDiagramTabs()
{
  Q_ASSERT(model);
  for ( Diagram* diagram: model->getDiagrams() )
    addDiagramTab(diagram);
}

void MainWindow::closeDiagram(int index, bool confirm)
{
  Q_ASSERT(model);
  QGraphicsView *view = qobject_cast<QGraphicsView*>(diagrams->widget(index));
  Q_ASSERT(view);
  qDebug() << "Closing diagram tab" << index << "/" << diagrams->count();
  Diagram *diagram = qobject_cast<Diagram*>(view->scene());
  Q_ASSERT(diagram);
  if ( confirm ) {
    QMessageBox save_message;
    save_message.setText("Do you really want to close this tab ? This will remove the corresponding diagram from the model.");
    save_message.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    save_message.setDefaultButton(QMessageBox::No);
    if ( save_message.exec() != QMessageBox::Yes ) return;
    }
  qDebug() << "Removing diagram" << diagram->getName();
  diagrams->removeTab(index);
  model->removeDiagram(diagram);
  delete diagram; // removeTab does _not_ delete the tabbed widget
  setUnsavedChanges(true);
}

void MainWindow::closeDiagramTabs() // Note: this does _not_ delete the displayed diagram !
{
  while ( diagrams->count() > 0 )
    diagrams->removeTab(diagrams->currentIndex());
}

// Popup windows (DOT rendering and generated code)

void MainWindow::openTextFiles(QStringList fnames)
{
  foreach ( QString fname, fnames ) {
    QFile file(fname);
    if ( ! file.open(QIODevice::ReadOnly | QIODevice::Text) ) {
      QMessageBox::warning(this,"Error:","cannot open file:\n"+fname);
      return;
      }
    }
  TextsViewer *viewer = new TextsViewer(fnames,this);
  viewer->show();
}

void MainWindow::openTextFile(QString fname)
{
  QFile file(fname);
  if ( ! file.open(QIODevice::ReadOnly | QIODevice::Text) ) {
    QMessageBox::warning(this,"Error:","cannot open file:\n"+fname);
    return;
    }
  TextViewer *viewer = new TextViewer(fname, this);
  viewer->show();
}

void MainWindow::openImageFile(QString fname)
{
  QFile file(fname);
  if ( ! file.open(QIODevice::ReadOnly | QIODevice::Text) ) {
    QMessageBox::warning(this,"Error:","cannot open file:\n"+fname);
    return;
    }
  ImageViewer *viewer = new ImageViewer(fname, this);
  viewer->show();
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
        openImageFile(changeSuffix(fname, ".gif"));
      }
    }
  else if ( f.suffix() == "vcd" ) {
    QString gFile = changeSuffix(fname, ".gtkw");
    QFile gf(gFile);
    if ( gf.exists() ) args << gFile;
    customView("VCDVIEWER", args, wDir, true);
    }
  else {
    openTextFile(fname);
    }
}

void MainWindow::openResultFiles(QStringList fnames)
{
  openTextFiles(fnames); // TO FIX : also handle multi-dot !
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

bool MainWindow::isMainFile(QString fname) {
  QString mainName = model->getName();
  if ( mainName.isEmpty() ) mainName = "main";
  mainName += ".cpp";
  QFileInfo f(fname);
  return f.fileName() == mainName;
}

// void MainWindow::generate(QString target, bool withTestbench)
// {
  // Q_ASSERT(model);
  // QString fname = generateRfsm(withTestbench);
  // QFileInfo fi(fname);
  // if ( fname.isEmpty() ) return;
  // qDebug () << "generate.fname = " << fname;
  // QString wDir = QFileInfo(fname).absolutePath();
  // QStringList genOpts = Globals::compilerOptions->getOptions("general");
  // QString targetDir = ".";
  // if ( target != "sim" && genOpts.contains("-target_dirs") ) {
  //   targetDir = target;
  //   QString targetPath = wDir + "/" + target; // TO FIX : do not use raw, OS-dependent "/" in file path
  //   genOpts.removeOne("-target_dirs");
  //   QDir dir(targetPath);
  //   if ( ! dir.exists() ) {
  //     qDebug() << "Creating directory " << targetPath;
  //     QDir().mkdir(targetPath);
  //     }
  //   }
  // foreach ( QString opt, genOpts)
  //   if ( genOpts.contains(opt) ) genOpts.removeOne(opt);
  // QString mainName = model->getName();
  // if ( mainName.isEmpty() ) mainName = "main";
  // QStringList args =
  //   QStringList()
  //   << "-" + target
  //   << "-main" << mainName
  //   << "-target_dir" << targetDir
  //   << genOpts
  //   << Globals::compilerOptions->getOptions(target);
  // //if ( target == "sim" ) args << "-main" <<  fi.baseName();
  // if ( target == "ctask" || target == "systemc" ) args << "-show_models";
  // if ( Globals::compiler->run(fi.fileName(), args, wDir) ) {
  //   QStringList resFiles = Globals::compiler->getOutputFiles(target, wDir, mainName); 
  //   logMessage("Generated file(s) : " + resFiles.join(", "));
  //   if ( ! withTestbench && target == "systemc" ) 
  //     resFiles.removeIf([this](QString fname) { return this->isMainFile(fname); });
  //   switch ( resFiles.size() ) {
  //     case 0: 
  //       break;
  //     case 1: 
  //       openResultFile(resFiles.first());
  //       break;
  //     default:
  //       openResultFiles(resFiles);
  //     }
  //   }
  // else {
  //   QStringList compileErrors = Globals::compiler->getErrors();
  //   QMessageBox::warning(this, "", "Error when compiling model\n" + compileErrors.join("\n"));
  //   }
// }

void MainWindow::generate(QString target, bool withTestbench)
{
  Q_ASSERT(model);
  QString fname = generateRfsm(withTestbench);
  QFileInfo fi(fname);
  if ( fname.isEmpty() ) return;
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
  QString mainName = model->getName();
  if ( mainName.isEmpty() ) mainName = "main";
  QStringList args =
    QStringList()
    << "-" + target
    << "-main" << mainName
    << "-target_dir" << targetDir
    << genOpts
    << Globals::compilerOptions->getOptions(target);
  //if ( target == "sim" ) args << "-main" <<  fi.baseName();
  if ( target == "ctask" || target == "systemc" ) args << "-show_models";
  args << fi.fileName();
  Response r = Globals::compiler->compile(args);
  qDebug() << "compile result =" << r.toString();
  if ( r.kind() == Response::Kind::Compiled ) {
    if ( r.result() == true ) {
      QStringList resFiles = r.files();
      qDebug() << "Generated files=" << resFiles;
      logMessage("Generated file(s) : " + resFiles.join(", "));
      if ( ! withTestbench && target == "systemc" ) 
        resFiles.removeIf([this](QString fname) { return this->isMainFile(fname); });
      switch ( resFiles.size() ) {
        case 0: break;
        case 1: openResultFile(resFiles.first()); break;
        default: openResultFiles(resFiles); break;
        }
      }
    else { // Compilation failed
      QString errMsg = r.message();
      qDebug() << "Compilation failed: " << errMsg;
      QMessageBox::warning(this, "", "Error when compiling model\n" + errMsg);
      }
    }
  else {
    qDebug() << "Wrong response to compile request: " << r.message();
    QMessageBox::warning(this, "", "Compilation failed\n" + r.message());
    }
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

// Dynamic cursor handling

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
  Globals::check_model = ! opts.contains("-no_model_check");
  updateActions(); 
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
  Q_ASSERT(model);
  model->dump(); 
}

void MainWindow::serverError(QString error)
{
  QMessageBox::critical(this, "Compiler", error); 
  delete this;
}

void MainWindow::checkCompilerVersion()
{
  Response r = Globals::compiler->getVersion();
  if ( r.kind() == Response::Kind::Version ) {
    QString v = r.version();
    qDebug() << "Compiler version=" << v;
    if ( v < Compiler::minimalVersion ) {
      QMessageBox::critical(this, "Compiler", "Invalid RFSM compiler version (" + v + ") (must be >= " + Compiler::minimalVersion + ")"); 
      exit(1);
      }
    }
  else {
    QMessageBox::critical(this, "Compiler", "Cannot retrieve compiler version");
    exit(1);
  }
}

// Bye

void MainWindow::quit()
{
    checkUnsavedChanges();
    close();
}
