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

#include <QFrame>
#include <QVBoxLayout>

class MainWindow;
class Automaton;
class IovPanel;
class QGraphicsView;
class QRegularExpressionValidator;

class AutomatonPanel : public QFrame
{
  Q_OBJECT
private:
  MainWindow* main_window;
  Automaton *automaton;
  QGraphicsView *view;
  IovPanel *vars_panel;
  
public:
  explicit AutomatonPanel(Automaton *automaton, QWidget* parent);
  ~AutomatonPanel();
  QGraphicsView *getView() { return view; }

  QSize sizeHint() const { return QSize(300,400); }; 

signals:
  void modelModified();
  
public slots:
  void fillVarsPanel();
  void clearVarsPanel();
  void clear();
  void fill();
  void update();

private:
  static QRegularExpressionValidator *var_name_validator;
};
