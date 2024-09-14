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

#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <QComboBox>
#include <QFrame>
#include <QGraphicsItem>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QListView>
#include <QString>
#include <QStringListModel>
#include <QVBoxLayout>
#include <QPushButton>
#include "iov.h"

class MainWindow;
class State;
class Transition;
class Model;
class ModelVars;

class PropertiesPanel : public QFrame
{
    Q_OBJECT

  static const QRegularExpression re_uid;
  static const QRegularExpression re_lid;

  private:
    MainWindow* main_window;

    QGraphicsItem* selected_item;

    QGroupBox* name_panel;
    QLineEdit* model_name_field;

    QGroupBox* inp_panel;
    QGroupBox* outp_panel;
    ModelVars* var_panel;
    QLineEdit* io_name;
    QComboBox* io_kind;
    QLineEdit* io_type;
    QStringListModel* ios_model;
    QListView* ios_view;

    // QGroupBox* state_base_panel;
    // QLineEdit* state_name_field;

    // StateValuationsPanel* state_valuations_panel;
    // QVBoxLayout* state_valuations_layout;

    // QGroupBox* transition_base_panel;
    // QLabel*    transition_start_state_label;
    // QComboBox* transition_start_state_field;
    // QLabel*    transition_end_state_label;
    // QComboBox* transition_end_state_field;
    // QLabel*    transition_event_label;
    // QComboBox* transition_event_field;

    QVBoxLayout* inp_layout;
    QVBoxLayout* outp_layout;
    QVBoxLayout* var_layout;

    // QGroupBox* transition_guards_panel;
    // QVBoxLayout* transition_guards_layout;
    // QGroupBox* transition_actions_panel;
    // QVBoxLayout* transition_actions_layout;

    QHash<QWidget*, QHBoxLayout*> widgetToLayout; 
    QHash<QWidget*, Iov*> widgetToIo; 
    QHash<Iov*, QHBoxLayout*> ioToLayout; 

  public:
    explicit PropertiesPanel(MainWindow* parent);
    ~PropertiesPanel();

    // void unselectItem();
  // void setSelectedItem(State* state);
    // void setSelectedItem(Transition* transition);

    void toggleStimuliPanel();                                                    

    QSize sizeHint() const { return QSize(275,300); }; 

  public slots:
    void setModelName();
  //void setStateName();

    // void addInput();
    // void addOutput();
    // void addVar();
    // void removeIo();
    // void removeTransitionGuard();
    // void removeTransitionAction();
  //void removeStateValuation();
    // void editIoName();
    // void editIoType();
    // void editIoStim();
    // void updateTransitionGuards();
    // void updateTransitionActions();
  // void updateStateValuations();

    // void setTransitionSrcState(int index);
    // void setTransitionDstState(int index);
    // void _setTransitionEvent(QComboBox* selector, Transition* transition);
    // void setTransitionEvent();
    // void setTransitionGuards(QStringList& guards);
    // void setTransitionActions(QStringList& actions);
  //void setStateValuations(QStringList& valuations);

    void update();
    void clear();
    void clearModelName();
  //void clearIos();

  private:
    void createNamePanel();
    QPushButton* createIoPanel(QString title, QGroupBox **io_panel, QVBoxLayout **io_layout);
    void createInputPanel();
    void createOutputPanel();
    void createVarPanel();
    // void createStateBasePanel();
  //void createStateValuationsPanel();
    // void createTransitionPanels();
    // void createTransitionBasePanel();
    // void createTransitionGuardsPanel();
    // void createTransitionActionsPanel();
    // void clearTransitionActionsPanel(QVBoxLayout *layout);
    // void clearTransitionGuardsPanel(QVBoxLayout *layout);
  //void clearStateValuationsPanel(QVBoxLayout *layout);

    void fillModelName();
    void fillIos();
    void setStimChoices(QComboBox* box, Iov *io);

    void delete_io_row(QLayout *layout);
    // void delete_action_row(QHBoxLayout *row_layout);
    // void delete_guard_row(QHBoxLayout *row_layout);
    // void delete_valuation_row(QHBoxLayout *row_layout);
    void _removeIo(Model* model, Iov* io);

    QGroupBox* io_panel_of(Iov::IoKind kind);
    QVBoxLayout* io_layout_of(Iov::IoKind kind);
    void _addIo(Model* model, Iov* io);
    void addIo(Iov::IoKind kind);
    // void _addTransitionGuard(QString guard);
    // void addTransitionGuard();
    // void _addTransitionAction(QString action);
    // void addTransitionAction();
  //void _addStateValuation(QString action);
    // void addStateValuation();

    void show_io_panels();
    void hide_io_panels();
  //void show_state_panels();
      //void hide_state_panels();

    // void show_transition_base_panel(bool isInitial);

    // QRegularExpressionValidator *state_name_validator;
    QRegularExpressionValidator *io_name_validator;
};

#endif
