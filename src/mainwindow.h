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

#pragma once

#include "globals.h"
#include "state.h"
#include "modelPanel.h"
#include "model.h"

#include <QMainWindow>
#include <QFileInfo>
#include <QFrame>
#include <QStatusBar>

QT_BEGIN_NAMESPACE
class QAction;
class QToolBox;
class QSpinBox;
class QComboBox;
class QFontComboBox;
class QButtonGroup;
class QLineEdit;
class QGraphicsTextItem;
class QFont;
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

public slots:
    void modelModified();
private slots:
    void save();
    void saveAs();
    void openFile();
    void newModel();
    void editModel(QAction *);    
    void addAutomatonToModel();
    void duplicateAutomaton();
    void quit();
    void about();
    bool checkAutomaton();
    bool checkModel();
    bool checkModelWithStimuli();
    void renderDots();
#ifndef USE_QGV
    void renderDot();
#endif
    void generateRfsmModel();
    void generateRfsmTestbench();
    void generateCTask();
    void generateSystemCModel();
    void generateSystemCTestbench();
    void generateVHDLModel();
    void generateVHDLTestbench();
    void runSimulation();
    void closeAutomatonTab(int index);
    void closeResultTab(int index);
    void resultTabChanged(int index);
    void automatonTabChanged(int index);
    void automatonTabChangeName(int index);
    void zoomIn(); 
    void zoomOut();
    void normalSize();
    void fitToWindow();
    void closeAutomatonTabs();
    void closeResultTabs();
    void setCompilerPaths();
    void setCompilerOptions();
    void setCodeFont();
    void updateCursor();
    void resetCursor();
    void compilerPathUpdated(QString path); 
    void dumpModel(void); // for debug only

private:
    void createActions();
    void updateActions();
    void updateViewActions();
    void createMenus();
    void createToolbars();
    void createPropertiesPanel();

    void checkUnsavedChanges();
    QString getCurrentFileName();
    void saveToFile(QString fname);
    QString generateRfsm(bool withTestbench);
    void addResultTab(QString fname);
    void addAutomatonTab(Automaton *a);
    void addAutomatonTabs(Model *m);
#ifdef USE_QGV
    void addDotTab(void);
#endif
    void openResultFile(QString fname);
    
    Model* model;
    QMap<QWidget*,Automaton*> panelToAutomaton;
    QFrame *toolbar;
    QButtonGroup *buttons;
    ModelPanel* model_panel; // Left panel (model IOs and properties)
    QTabWidget *automatons_panel; // Center panel (automata editor)
    QTabWidget *results_panel; // Right panel (DOT rendering and generated code)
    QStatusBar *statusBar;

    QAction *newModelAction;
    QAction *openFileAction;
    QAction *saveFileAction;
    QAction *saveFileAsAction;
    QAction *aboutAction;
    QAction *exitAction;
    QAction *checkAutomatonAction;
    QAction *checkModelAction;
    QAction *checkModelWithStimuliAction;
    QAction *renderDotsAction;
#ifndef USE_QGV
    QAction *renderDotAction;
#endif
    QAction *generateRfsmModelAction;
    QAction *generateRfsmTestbenchAction;
    QAction *generateCTaskAction;
    QAction *generateSystemCModelAction;
    QAction *generateSystemCTestbenchAction;
    QAction *generateVHDLModelAction;
    QAction *generateVHDLTestbenchAction;
    QAction *runSimulationAction;
    QAction *zoomInAction;
    QAction *zoomOutAction;
    QAction *normalSizeAction;
    QAction *fitToWindowAction;
    QAction *closeResultsAction;
    QAction *pathConfigAction;
    QAction *compilerOptionsAction;
    QAction *fontConfigAction;
    QActionGroup *modelActions;
    QAction* addAutomatonAction;
    QAction* duplAutomatonAction;
    QAction* dumpModelAction;
    QActionGroup *automatonActions;
    QAction* selectItemAction;
    // QAction* editItemAction;
    QAction* addStateAction;
    QAction* addInitStateAction;
    QAction* addTransitionAction;
    QAction* addSelfTransitionAction;
    QAction* deleteItemAction;

    //QMenu *aboutMenu;
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
    void scaleImage(double factor);

    bool unsaved_changes;
    QString currentFileName;
    QWidget* selectedTab(); // TODO: disambiguate; there are now two tab collections
    double currentScaleFactor;

    QCursor default_cursor;
    QMap<Globals::Mode,QCursor> cursors;
    void initCursors();

    QFont codeFont;
    static const QString title;
    static const QList<int> splitterSizes;
    static const double zoomInFactor;
    static const double zoomOutFactor;
    static const double minScaleFactor;
    static const double maxScaleFactor;

    void logMessage(QString msg);

public:
  Model* getModel() const { return model; }
  void setUnsavedChanges(bool unsaved_changes = true);
};

