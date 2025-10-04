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

#include "globals.h"
#include "state.h"
#include "modelProperties.h"
#include "model.h"
#include "compiler.h"

#include <QMainWindow>
#include <QFileInfo>
#include <QFrame>
#include <QStatusBar>
#include <QDockWidget>

QT_BEGIN_NAMESPACE
class QAction;
class QToolBox;
class QSpinBox;
class QComboBox;
class QFontComboBox;
class QButtonGroup;
class QLineEdit;
class QDockWidget;
//class QGraphicsTextItem;
//class QFont;
class QToolButton;
class QAbstractButton;
class QTabWidget;
class QActionGroup;
class ImageViewer;
class CompilerPaths;
class CompilerOptions;
class CommandExec;
class Compiler;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
   MainWindow();
  ~MainWindow();

  Model *getModel() { return model; }
  QTabWidget *getDiagrams() { return diagrams; }

public slots:
    void modelModified();
private slots:
    void save();
    void saveAs();
    void openFile();
    void newModel();
    Diagram* diagramOf(int index);
    void editDiagram(QAction *);    
    void addDiagramTab(Diagram* diagram);
    void newDiagram();
    void duplicateDiagram();
    void quit();
    void about();
    bool checkDiagram();
    bool checkModel();
    bool checkModelWithStimuli();
    void renderDots();
    void renderDot();
    void generateRfsmModel();
    void generateRfsmTestbench();
    void generateCTask();
    void generateSystemCModel();
    void generateSystemCTestbench();
    void generateVHDLModel();
    void generateVHDLTestbench();
    void runSimulation();
  void closeDiagram(int index, bool confirm = true);
    void closeDiagramTabs();
    void setCompilerPaths();
    void setCompilerOptions();
    void updateCursor();
    void resetCursor();
    void compilerPathUpdated(QString path); 
    void dumpModel(void); // for debug only
    void serverError(QString err);

private:
    void createActions();
    void updateActions();
    void updateViewActions();
    void createMenus();
    void createToolbar();
    void createDockWindow(Model *model);

    void checkUnsavedChanges();
    QString getCurrentFileName();
    void saveToFile(QString fname);
    QString generateRfsm(bool withTestbench);
    void openTextFile(QString fname);
    void openTextFiles(QStringList fname);
    void openImageFile(QString fname);
    void addDiagramTabs();
    void openResultFile(QString fname);
    void openResultFiles(QStringList fname);
    Diagram *currentDiagram();
    void checkCompilerVersion();
    
    Model* model; // The model (ios + diagrams)
    ModelProperties *model_panel; // For editing model IOs
    QDockWidget* dock; // Holding the model_panel
    QTabWidget *diagrams; // The tabs holding the graphical representations of the model diagrams
    QFrame *toolbar;
    QButtonGroup *buttons;
    QStatusBar *statusBar;

    QAction *newModelAction;
    QAction *openFileAction;
    QAction *saveFileAction;
    QAction *saveFileAsAction;
    QAction *aboutAction;
    QAction *exitAction;
    QAction *checkDiagramAction;
    QAction *checkModelAction;
    QAction *checkModelWithStimuliAction;
    QAction *renderDotsAction;
    QAction *renderDotAction;
    QAction *generateRfsmModelAction;
    QAction *generateRfsmTestbenchAction;
    QAction *generateCTaskAction;
    QAction *generateSystemCModelAction;
    QAction *generateSystemCTestbenchAction;
    QAction *generateVHDLModelAction;
    QAction *generateVHDLTestbenchAction;
    QAction *runSimulationAction;
    QAction *closeResultsAction;
    QAction *pathConfigAction;
    QAction *compilerOptionsAction;
    QActionGroup *modelActions;
    QAction* addDiagramAction;
    QAction* duplDiagramAction;
    QAction* dumpModelAction;
    QActionGroup *diagramActions;
    QAction* selectItemAction;
    QAction* addStateAction;
    QAction* addInitStateAction;
    QAction* addTransitionAction;
    QAction* addSelfTransitionAction;
    QAction* deleteItemAction;

    QMenu *fileMenu;
    QMenu *modelMenu;
    QMenu *compileMenu;
    QMenu *viewMenu;
    QMenu *configMenu;

    QToolBar *fileToolBar;
    QToolBar *modelToolBar;
    QToolBar *editToolBar;
    QToolBar *compileToolBar;

    QStringList compile(QString target, QString wDir, QString srcFile, QStringList args);
    QStringList getOutputFiles(QString target, QString wdir);
    void generate(QString target, bool withTestbench);
    void customView(QString toolName, QStringList args, QString wDir, bool detach);
    void customView(QString toolName, QString fname, QString wDir);
    void exportDot();
    void exportRfsmModel();
    void exportRfsmTestbench();
    bool dotTransform(QFileInfo f, QString wDir);
    bool executeCmd(QString wDir, QString cmd, QStringList args, bool sync=true);
    bool isMainFile(QString fname);

    bool unsaved_changes;
    QString currentFileName;
    double currentScaleFactor;

    QCursor default_cursor;
    QMap<Globals::Mode,QCursor> cursors;
    void initCursors();

    static const QString title;

    void logMessage(QString msg);

public:
  void setUnsavedChanges(bool unsaved_changes = true);
};

