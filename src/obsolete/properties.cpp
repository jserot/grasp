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

#include "properties.h"
#include "mainwindow.h"
#include "state.h"
#include "transition.h"
#include "model.h"
#include "modelVars.h"
#include "stimuli.h"
#include "compilerPaths.h"

#include <QComboBox>
#include <QFrame>
#include <QGraphicsItem>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QListView>
#include <QString>
#include <QStringList>
#include <QStringListModel>
#include <QTextEdit>
#include <QVBoxLayout>
#include <stdexcept>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QRegularExpression>
#include <QDebug>

//const QRegularExpression PropertiesPanel::re_uid("[A-Z][A-Za-z0-9_]*");
const QRegularExpression PropertiesPanel::re_lid("[a-z][a-z0-9_]*");

PropertiesPanel::PropertiesPanel(Model *model, MainWindow* parent) : QFrame(parent)
{
    main_window = parent;

    createNamePanel();
    // createInputPanel();
    // createOutputPanel();
    // createVarPanel();

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setAlignment(Qt::AlignTop);
    //layout->setMinimumWidth(200);

    layout->addWidget(name_panel);
    // layout->addWidget(inp_panel);
    // layout->addWidget(outp_panel);

    var_panel = new ModelVars("Variables", model);
     
    layout->addWidget(var_panel);

    this->setLayout(layout);

    connect(model_name_field, &QLineEdit::editingFinished, this, &PropertiesPanel::setModelName);

    io_name_validator = new QRegularExpressionValidator(re_lid);
    assert(main_window);
    assert(main_window->getCompilerPaths());
    CommandExec* executor = main_window->getExecutor();
    assert(executor);

    // name_panel->show();
    // show_io_panels();
}

PropertiesPanel::~PropertiesPanel()
{
}

// [Name] panel

void PropertiesPanel::createNamePanel()
{
    name_panel = new QGroupBox("Name");
    QVBoxLayout* layout = new QVBoxLayout();
    name_panel->setMaximumHeight(100);
    //name_panel->setMinimumWidth(200);
    //QLabel* nameLabel = new QLabel("Name");
    model_name_field = new QLineEdit();
    model_name_field->setPlaceholderText("Model name");
    // Note: no validation installed here
    // layout->addWidget(nameLabel);
    layout->addWidget(model_name_field);
    name_panel->setLayout(layout);
}

void PropertiesPanel::setModelName()
{
  Model* model = main_window->getModel();
  QString name = model_name_field->text();
  model->setName(name.trimmed());
  main_window->setUnsavedChanges(true);
}

void PropertiesPanel::fillModelName()
{
    Model* model = main_window->getModel();
    if ( model == NULL ) return;
    model_name_field->setText(model->getName());
}

void PropertiesPanel::clearModelName()
{
    Model* model = main_window->getModel();
    if ( model == NULL ) return;
    model_name_field->setText("");
}

// [IO] Panels
// TODO : use subclasses of [DynamicPanel] here

// QPushButton* PropertiesPanel::createIoPanel(QString title, QGroupBox **io_panel, QVBoxLayout **io_layout)
// {
//   QHBoxLayout *rowLayout;
//   QGroupBox *panel = new QGroupBox(title);
  //panel->setObjectName(title);
  //io_panel->setMaximumHeight(200);
//   //io_panel->setMinimumWidth(200);
//   QVBoxLayout *layout = new QVBoxLayout();
//   layout->setSpacing(4);
//   layout->setContentsMargins(11, 11, 11, 11);

//   rowLayout = new QHBoxLayout();
//   QPushButton* add_button = new QPushButton("Add");
//   rowLayout->addWidget(add_button);
//   layout->addLayout(rowLayout);
//   panel->setLayout(layout);

//   *io_panel = panel;
//   *io_layout = layout;
//   return add_button;
// }

// void PropertiesPanel::show_io_panels()
// {
//     inp_panel->show();
//     outp_panel->show();
//     var_panel->show();
// }

// void PropertiesPanel::hide_io_panels()
// {
//     inp_panel->hide();
//     outp_panel->hide();
//     var_panel->hide();
// }


// void PropertiesPanel::createInputPanel()
// {
//   QPushButton* add_button = createIoPanel("Inputs", &inp_panel, &inp_layout);
//   connect(add_button, &QPushButton::clicked, this, &PropertiesPanel::addInput);
// }

// void PropertiesPanel::createOutputPanel()
// {
//   QPushButton* add_button = createIoPanel("Outputs", &outp_panel, &outp_layout);
//   connect(add_button, &QPushButton::clicked, this, &PropertiesPanel::addOutput);
// }

// void PropertiesPanel::createVarPanel()
// {
//   QPushButton* add_button = createIoPanel("Variables", &var_panel, &var_layout);
//   connect(add_button, &QPushButton::clicked, this, &PropertiesPanel::addVar);
// }

// void enableComboBoxItem(QComboBox* box, int i, bool enabled)
// {
//   QStandardItemModel *model = qobject_cast<QStandardItemModel*>(box->model());
//   Q_ASSERT(model);
//   QStandardItem *item = model->item(i);
//   Q_ASSERT(item);
//   item->setEnabled(enabled);
// }

// QGroupBox* PropertiesPanel::io_panel_of(Iov::IoKind kind)
// {
//   switch ( kind ) {
//   case Iov::IoIn: return inp_panel;
//   case Iov::IoOut: return outp_panel;
//   case Iov::IoVar: return var_panel;
//   }
// }

// QVBoxLayout* PropertiesPanel::io_layout_of(Iov::IoKind kind)
// {
//   switch ( kind ) {
//   case Iov::IoIn: return inp_layout;
//   case Iov::IoOut: return outp_layout;
//   case Iov::IoVar: return var_layout;
//   }
// }

// void PropertiesPanel::_addIo(Model* model, Iov* io)
// {
//   qDebug () << "Adding" << io->kind << io->name << "to panel";
//   Q_ASSERT(model);
//   Q_ASSERT(io);
//   QGroupBox *io_panel = io_panel_of(io->kind);
//   QVBoxLayout *io_layout = io_layout_of(io->kind);
//   QHBoxLayout *rowLayout = new QHBoxLayout(io_panel);
//   rowLayout->setObjectName("ioRowLayout");

//   QLineEdit *io_name = new QLineEdit();
//   io_name->setMinimumSize(80,io_name->minimumHeight());
//   io_name->setPlaceholderText("IO name");
//   io_name->setFrame(true);
//   io_name->setText(io->name);
//   io_name->setCursorPosition(0);
//   rowLayout->addWidget(io_name);
//   widgetToIo.insert((QWidget*)io_name, io);
//   widgetToLayout.insert((QWidget*)io_name, rowLayout);
//   io_name->setValidator(io_name_validator);
//   connect(io_name, &QLineEdit::editingFinished, this, &PropertiesPanel::editIoName);

//   QComboBox *io_type = new QComboBox();
//   io_type->addItem("event", QVariant(Iov::TyEvent));
//   io_type->addItem("int", QVariant(Iov::TyInt));
//   io_type->addItem("bool", QVariant(Iov::TyBool));
//   io_type->setCurrentIndex(io->type);
//   rowLayout->addWidget(io_type);
//   widgetToIo.insert((QWidget*)io_type, io);
//   widgetToLayout.insert((QWidget*)io_type, rowLayout);
// #if QT_VERSION >= 0x060000
//   connect(io_type, &QComboBox::currentIndexChanged, this, &PropertiesPanel::editIoType);
// #else
//   connect(io_type, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &PropertiesPanel::editIoType);
// #endif

//   if ( io->kind == Iov::IoIn ) {
//     QComboBox *io_stim = new QComboBox();
//     io_stim->addItem("None", QVariant(Stimulus::None));
//     io_stim->addItem("Periodic", QVariant(Stimulus::Periodic));
//     io_stim->addItem("Sporadic", QVariant(Stimulus::Sporadic));
//     io_stim->addItem("ValueChanges", QVariant(Stimulus::ValueChanges));
//     io_stim->setCurrentIndex(io->stim.kind);
//     setStimChoices(io_stim, io);
//     rowLayout->addWidget(io_stim);
// #if QT_VERSION >= 0x060000
//     connect(io_stim, &QComboBox::activated, this, &PropertiesPanel::editIoStim);
// #else
//     connect(io_stim, QOverload<int>::of(&QComboBox::activated), this, &PropertiesPanel::editIoStim);
// #endif
//     widgetToLayout.insert((QWidget*)io_stim, rowLayout);
//     widgetToIo.insert((QWidget*)io_stim, io);
//   }

//   QPushButton *io_delete = new QPushButton();
//   io_delete->setIcon(QIcon(":/images/delete.png"));
//   rowLayout->addWidget(io_delete);
//   connect(io_delete, &QPushButton::clicked, this, &PropertiesPanel::removeIo);
//   widgetToLayout.insert((QWidget*)io_delete, rowLayout);
//   widgetToIo.insert((QWidget*)io_delete, io);
//   io_layout->insertLayout(-1,rowLayout);
//   ioToLayout.insert(io,rowLayout);
// }

// void PropertiesPanel::addInput() { addIo(Iov::IoIn); }
// void PropertiesPanel::addOutput() { addIo(Iov::IoOut); }
// void PropertiesPanel::addVar() { addIo(Iov::IoVar); }

// void PropertiesPanel::addIo(Iov::IoKind kind)
// {
//   Model* model = main_window->getModel();
//   Q_ASSERT(model != 0);
//   Iov* io = model->addIo("", kind, Iov::TyEvent, Stimulus(Stimulus::None));
//   _addIo(model, io);
// }

// void PropertiesPanel::setStimChoices(QComboBox* box, Iov *io)
// {
//   switch ( io->type ) {
//   case Iov::TyEvent:
//     enableComboBoxItem(box, 0, true); // TO FIX: we should not use hardcoded index here 
//     enableComboBoxItem(box, 1, true);
//     enableComboBoxItem(box, 2, true);
//     enableComboBoxItem(box, 3, false);
//     break;
//   case Iov::TyInt:
//   case Iov::TyBool:
//     enableComboBoxItem(box, 0, true); // TO FIX: we should not use hardcoded index here 
//     enableComboBoxItem(box, 1, false);
//     enableComboBoxItem(box, 2, false);
//     enableComboBoxItem(box, 3, true);
//     break;
//     }
// }

// void PropertiesPanel::delete_io_row(QLayout *layout)
// {
//   while (layout->count() != 0) {
//     QLayoutItem* item = layout->takeAt(0);
//     QWidget* widget = item->widget();
//     widgetToLayout.remove(widget);
//     widgetToIo.remove(widget);
//     delete item->widget();
//     delete item;
//   }
//   delete layout;
// }

// void PropertiesPanel::_removeIo(Model* model, Iov* io)
// {
//   Q_ASSERT(model);
//   Q_ASSERT(io);
//   QHBoxLayout* row_layout = ioToLayout.take(io);
//   Q_ASSERT(row_layout);
//   QLineEdit *io_name = qobject_cast<QLineEdit*>(row_layout->itemAt(0)->widget());
//   Q_ASSERT(io_name);
//   QString name = io_name->text().trimmed();
//   qDebug() << "Removing IO" << name;
//   delete_io_row(row_layout);
//   model->removeIo(io);
//   main_window->setUnsavedChanges(true);
// }

// void PropertiesPanel::removeIo()
// {
//   QPushButton* button = qobject_cast<QPushButton*>(sender());
//   Iov* io = widgetToIo.take(button);
//   Model* model = main_window->getModel();
//   _removeIo(model, io);
// }

// void PropertiesPanel::editIoName()
// {
//   QLineEdit* ledit = qobject_cast<QLineEdit*>(sender());
//   Iov* io = widgetToIo.value(ledit);
//   Q_ASSERT(io);
//   QString name = ledit->text().trimmed();
//   // Model* model = main_window->getModel();
//   // if ( model->getIo(name) != NULL ) {
//   //     QMessageBox::warning( this, "Error", "IO or variable " + name + " is already defined");
//   //     return;
//   //     }
//   qDebug () << "Setting IO name: " << name;
//   io->name = name;
//   main_window->setUnsavedChanges(true);
// }

// void setComboBoxItemEnabled(QComboBox* comboBox, int index, bool enabled)
// {
//     auto * model = qobject_cast<QStandardItemModel*>(comboBox->model());
//     Q_ASSERT(model);
//     if(!model) return;

//     auto * item = model->item(index);
//     Q_ASSERT(item);
//     if(!item) return;
//     item->setEnabled(enabled);
// }

// void PropertiesPanel::editIoType()
// {
//   QComboBox* box = qobject_cast<QComboBox*>(sender());
//   Iov* io = widgetToIo.value(box);
//   Q_ASSERT(io);
//   io->type = (Iov::IoType)(box->currentIndex());
//   qDebug () << "Setting IO type: " << io->type;
//   QHBoxLayout* row_layout = widgetToLayout.value(box);
//   Q_ASSERT(row_layout);
//   // QComboBox *stim_box = qobject_cast<QComboBox*>(row_layout->itemAt(2)->widget());
//   // Q_ASSERT(stim_box);
//   // setStimChoices(stim_box, io);
//   main_window->setUnsavedChanges(true);
// }

// void PropertiesPanel::editIoStim()
// {
//   QComboBox* selector = qobject_cast<QComboBox*>(sender());
//   Iov* io = widgetToIo.value(selector);
//   Q_ASSERT(io);
//   QString io_name = io->name;
//   if ( io_name == "" ) {
//     QMessageBox::warning( this, "Error", "Please give a name to this IO before editing it");
//     selector->setCurrentIndex(Stimulus::None);
//     return;
//     }
//   Stimulus::Kind kind = (Stimulus::Kind)(selector->currentIndex()); 
//   switch ( kind ) {
//   case Stimulus::None:
//     io->stim = Stimulus(Stimulus::None);
//     break;
//   default:
//     Stimuli* stimDialog = new Stimuli(kind,io,main_window);
//     stimDialog->exec();
//     //fillIoList();
//     delete stimDialog;
//     break;
//     }
//   main_window->setUnsavedChanges(true);
// }

// void PropertiesPanel::fillIos()
// {
//   Model* model = main_window->getModel();
//   Q_ASSERT(model);
//   if ( model == NULL ) return;
//   for (auto io: model->getIos())
//     _addIo(model, io);
// }

// void PropertiesPanel::clearIos()
// {
//   foreach ( Iov* io, ioToLayout.keys() ) {
//     qDebug() << "Removing IO " << io->name << " from panel";
//     QHBoxLayout *row_layout = ioToLayout.take(io);
//     delete_io_row(row_layout);
//     }
// }

// [Transition] base panel (common to standard and initial transitions)
// Note: for initial transitions, the [start_state] and [event] elements will be hidden
// TODO: use a subclass of DynDialog for this !!

// void PropertiesPanel::createTransitionBasePanel()
// {
//     transition_base_panel = new QGroupBox("Transition");
//     QGridLayout* transitionLayout = new QGridLayout();
//     transition_base_panel->setMaximumHeight(180);

//     transition_start_state_label = new QLabel("Start State");
//     transition_start_state_field = new QComboBox();
//     transitionLayout->addWidget(transition_start_state_label, 0, 0, 1, 3);
//     transitionLayout->addWidget(transition_start_state_field, 1, 0, 1, 3);

//     transition_end_state_label = new QLabel("End State");
//     transition_end_state_field = new QComboBox();
//     transitionLayout->addWidget(transition_end_state_label, 0, 3, 1, 3);
//     transitionLayout->addWidget(transition_end_state_field, 1, 3, 1, 3);

//     transition_event_label = new QLabel("Event");
//     transition_event_field = new QComboBox();
//     transitionLayout->addWidget(transition_event_label, 2, 0, 1, 2);
//     transitionLayout->addWidget(transition_event_field, 2, 2, 1, 4);

//     transition_base_panel->setLayout(transitionLayout);
// }

// void PropertiesPanel::setTransitionSrcState(int index)
// {
//   if ( index == -1 ) return;
//   Transition* transition = qgraphicsitem_cast<Transition*>(selected_item);
//   if ( transition == nullptr ) return;
//   QString state_id = transition_start_state_field->itemText(index);
//   State* state = main_window->getModel()->getState(state_id);
//   if ( state == nullptr )
//     throw std::invalid_argument(std::string("No state found with id : ") + state_id.toStdString());
//   transition->setSrcState(state);
//   main_window->getModel()->update();
//   main_window->setUnsavedChanges(true);
// }

// void PropertiesPanel::setTransitionDstState(int index)  // TODO: factorize with above code
// {
//   if ( index == -1 ) return;
//   Transition* transition = qgraphicsitem_cast<Transition*>(selected_item);
//   if ( transition == nullptr ) return;
//   QString state_id = transition_end_state_field->itemText(index);
//   State* state = main_window->getModel()->getState(state_id);
//   if ( state == nullptr )
//     throw std::invalid_argument(std::string("No state found with id : ") + state_id.toStdString());
//   transition->setDstState(state);
//   main_window->getModel()->update();
//   main_window->setUnsavedChanges(true);
// }

// void PropertiesPanel::_setTransitionEvent(QComboBox* selector, Transition* transition)
// {
//   Q_ASSERT(selector);
//   Q_ASSERT(transition);
//   QString event = selector->currentText();
//   qDebug() << "setTransitionEvent:" << event;
//   transition->setEvent(event);
//   main_window->getModel()->update();
//   main_window->setUnsavedChanges(true);
// }

// void PropertiesPanel::setTransitionEvent()
// {
//   QComboBox* selector = qobject_cast<QComboBox*>(sender());
//   Transition* transition = qgraphicsitem_cast<Transition*>(selected_item);
//   _setTransitionEvent(selector,transition);
// }

// [Transition Actions] panel
// TODO: use a subclass of DynDialog for this !!

// void PropertiesPanel::createTransitionActionsPanel()
// {
//   QGroupBox *panel = new QGroupBox("Transition actions");
//   //panel->setMaximumHeight(200);
//   //panel->setMinimumWidth(200);
//   QVBoxLayout *layout = new QVBoxLayout();
//   layout->setSpacing(4);
//   layout->setContentsMargins(11, 11, 11, 11);
//   QHBoxLayout *rowLayout = new QHBoxLayout();
//   QPushButton* add_button = new QPushButton("Add");
//   rowLayout->addWidget(add_button);
//   layout->addLayout(rowLayout);
//   panel->setLayout(layout);
//   transition_actions_panel = panel;
//   transition_actions_layout = layout;
//   connect(add_button, &QPushButton::clicked, this, &PropertiesPanel::addTransitionAction);
// }

// void PropertiesPanel::addTransitionAction()
// {
//   _addTransitionAction("");
// }

// void PropertiesPanel::_addTransitionAction(QString action)
// {
//   QHBoxLayout *rowLayout = new QHBoxLayout(transition_actions_panel);
//   rowLayout->setObjectName("actionRowLayout");
//   QLineEdit *action_field = new QLineEdit();
//   action_field->setPlaceholderText("<output or variable> := <expr>");
//   action_field->setMinimumSize(80,action_field->minimumHeight());
//   action_field->setFrame(true);
//   action_field->setText(action);
//   action_field->setCursorPosition(0);
//   rowLayout->addWidget(action_field);
//   connect(action_field, &QLineEdit::editingFinished, this, &PropertiesPanel::updateTransitionActions);

//   QPushButton *action_delete = new QPushButton();
//   action_delete->setIcon(QIcon(":/images/delete.png"));
//   rowLayout->addWidget(action_delete);
//   connect(action_delete, &QPushButton::clicked, this, &PropertiesPanel::removeTransitionAction);
//   transition_actions_layout->insertLayout(-1,rowLayout);
// }

// void PropertiesPanel::updateTransitionActions() // SLOT
// {
//   QStringList actions;
//   SyntaxChecker* syntaxChecker = main_window->getSyntaxChecker();
//   Model *model = main_window->getModel();
//   assert(model);
//   for ( int i=1; i<transition_actions_layout->count(); i++ ) {
//     QHBoxLayout *row_layout = static_cast<QHBoxLayout*>(transition_actions_layout->itemAt(i));
//     QLineEdit *ledit = qobject_cast<QLineEdit*>(row_layout->itemAt(0)->widget());
//     assert(ledit);
//     QString action = ledit->text().trimmed();
//     QString msg = syntaxChecker->check_action(model->getInpNonEvents(), model->getOutputs(), model->getVars(), action);
//     if ( msg == "Ok" )
//       actions << action;
//     else {
//       // ledit->clear();
//       QMessageBox::warning(this, "Error", msg);
//       return;
//       }
//     }
//   qDebug () << "** Updating transition with actions=" << actions;
//   setTransitionActions(actions);
//   main_window->setUnsavedChanges(true);
// }

// void PropertiesPanel::removeTransitionAction() // SLOT
// {
//   QPushButton* button = qobject_cast<QPushButton*>(sender());
//   QHBoxLayout *row_layout = NULL;
//   // Retrieve the row to delete in [transition_actions_layout] 
//   int row;
//   int nb_rows = transition_actions_layout->count();
//   for ( row=1; row<nb_rows; row++ ) {
//     row_layout = static_cast<QHBoxLayout*>(transition_actions_layout->itemAt(row));
//     if ( button == qobject_cast<QPushButton*>(row_layout->itemAt(1)->widget()) ) break;
//     }
//   assert(row >= 1 && row <nb_rows && row_layout );
//   qDebug() << "Deleting action #" << row;
//   delete_action_row(row_layout);
//   transition_actions_layout->takeAt(row);
//   updateTransitionActions();
//   main_window->setUnsavedChanges(true);
// }

// void PropertiesPanel::delete_action_row(QHBoxLayout *row_layout)
// {
//   QLineEdit *ledit = qobject_cast<QLineEdit*>(row_layout->itemAt(0)->widget());
//   assert(ledit);
//   QPushButton *button = qobject_cast<QPushButton*>(row_layout->itemAt(1)->widget());
//   assert(button);
//   delete ledit;
//   delete button;
// }

// void PropertiesPanel::clearTransitionActionsPanel(QVBoxLayout *layout)
// {
//   QHBoxLayout *row_layout; 
//   qDebug() << "Clearing transition actions panel";
//   while ( layout->count() > 1 ) { // Do not delete the [Add] button !
//     row_layout = static_cast<QHBoxLayout*>(layout->itemAt(1));
//     delete_action_row(row_layout);
//     layout->takeAt(1);
//   }
// }

// void PropertiesPanel::setTransitionActions(QStringList& actions)
// {
//   Transition* transition = qgraphicsitem_cast<Transition*>(selected_item);
//   if ( transition == nullptr ) return;
//   transition->setActions(actions);
//   main_window->getModel()->update();
//   main_window->setUnsavedChanges(true);
// }

// // [Transition guards] panel 
// // TODO : use a subclass of DynamicPanel (as StateValuationsPanel)

// void PropertiesPanel::createTransitionGuardsPanel()
// {
//   QGroupBox *panel = new QGroupBox("Transition guards");
//   //panel->setMaximumHeight(200);
//   //panel->setMinimumWidth(200);
//   QVBoxLayout *layout = new QVBoxLayout();
//   layout->setSpacing(4);
//   layout->setContentsMargins(11, 11, 11, 11);
//   QHBoxLayout *rowLayout = new QHBoxLayout();
//   QPushButton* add_button = new QPushButton("Add");
//   rowLayout->addWidget(add_button);
//   layout->addLayout(rowLayout);
//   panel->setLayout(layout);
//   transition_guards_panel = panel;
//   transition_guards_layout = layout;
//   connect(add_button, &QPushButton::clicked, this, &PropertiesPanel::addTransitionGuard);
// }

// void PropertiesPanel::addTransitionGuard()
// {
//   _addTransitionGuard("");
// }

// void PropertiesPanel::_addTransitionGuard(QString guard)
// {
//   QHBoxLayout *rowLayout = new QHBoxLayout(transition_guards_panel);
//   rowLayout->setObjectName("guardRowLayout");
//   QLineEdit *guard_field = new QLineEdit();
//   guard_field->setPlaceholderText("Boolean expression");
//   guard_field->setMinimumSize(80,guard_field->minimumHeight());
//   guard_field->setFrame(true);
//   guard_field->setText(guard);
//   guard_field->setCursorPosition(0);
//   rowLayout->addWidget(guard_field);
//   connect(guard_field, &QLineEdit::editingFinished, this, &PropertiesPanel::updateTransitionGuards);

//   QPushButton *guard_delete = new QPushButton();
//   guard_delete->setIcon(QIcon(":/images/delete.png"));
//   rowLayout->addWidget(guard_delete);
//   connect(guard_delete, &QPushButton::clicked, this, &PropertiesPanel::removeTransitionGuard);
//   transition_guards_layout->insertLayout(-1,rowLayout);
// }

// void PropertiesPanel::updateTransitionGuards() // SLOT
// {
//   SyntaxChecker* syntaxChecker = main_window->getSyntaxChecker();
//   assert(syntaxChecker);
//   Model *model = main_window->getModel();
//   assert(model);
//   QStringList guards;
//   for ( int i=1; i<transition_guards_layout->count(); i++ ) {
//     QHBoxLayout *row_layout = static_cast<QHBoxLayout*>(transition_guards_layout->itemAt(i));
//     assert(row_layout);
//     QLineEdit *ledit = qobject_cast<QLineEdit*>(row_layout->itemAt(0)->widget());
//     assert(ledit);
//     QString guard = ledit->text().trimmed();
//     QString msg = syntaxChecker->check_guard(model->getInpNonEvents(), model->getOutputs(), model->getVars(), guard);
//     if ( msg == "Ok" )
//       guards << guard;
//     else {
//       // ledit->clear();
//       QMessageBox::warning(this, "Error", msg);
//       return;
//       }
//     }
//   if ( guards.length() >= 2 ) {
//     for ( int i = 0; i<guards.length(); i++ ) {
//       QString guard = guards.at(i);
//       guards.replace(i, "(" + guard + ")");
//     }
//   }
//   qDebug () << "** Updating transition with guards=" << guards;
//   setTransitionGuards(guards);
//   main_window->setUnsavedChanges(true);
// }

// void PropertiesPanel::removeTransitionGuard() // SLOT
// {
//   QPushButton* button = qobject_cast<QPushButton*>(sender());
//   QHBoxLayout *row_layout = NULL;
//   // Retrieve the row to delete in [transition_guards_layout] 
//   int row;
//   int nb_rows = transition_guards_layout->count();
//   for ( row=1; row<nb_rows; row++ ) {
//     row_layout = static_cast<QHBoxLayout*>(transition_guards_layout->itemAt(row));
//     if ( button == qobject_cast<QPushButton*>(row_layout->itemAt(1)->widget()) ) break;
//     }
//   assert(row >= 1 && row <nb_rows && row_layout );
//   qDebug() << "Deleting guard #" << row;
//   delete_guard_row(row_layout);
//   transition_guards_layout->takeAt(row);
//   updateTransitionGuards();
//   main_window->setUnsavedChanges(true);
// }

// void PropertiesPanel::delete_guard_row(QHBoxLayout *row_layout)
// {
//   QLineEdit *ledit = qobject_cast<QLineEdit*>(row_layout->itemAt(0)->widget());
//   assert(ledit);
//   QPushButton *button = qobject_cast<QPushButton*>(row_layout->itemAt(1)->widget());
//   assert(button);
//   delete ledit;
//   delete button;
// }

// void PropertiesPanel::clearTransitionGuardsPanel(QVBoxLayout *layout)
// {
//   QHBoxLayout *row_layout; 
//   qDebug() << "Clearing transition guards panel";
//   while ( layout->count() > 1 ) { // Do not delete the [Add] button !
//     row_layout = static_cast<QHBoxLayout*>(layout->itemAt(1));
//     delete_guard_row(row_layout);
//     layout->takeAt(1);
//   }
// }

// void PropertiesPanel::setTransitionGuards(QStringList& guards)
// {
//   Transition* transition = qgraphicsitem_cast<Transition*>(selected_item);
//   if ( transition == nullptr ) return;
//   transition->setGuards(guards);
//   main_window->getModel()->update();
//   main_window->setUnsavedChanges(true);
// }

// [Select/Unselect] actions

// void PropertiesPanel::unselectItem()
// {
//   // TO BE REMOVED when only IO panels will be left
//     qDebug() << "Unselect item";
//     selected_item = nullptr;
//       //hide_state_panels();
//     transition_base_panel->hide();
//     transition_actions_panel->hide();
//     transition_guards_panel->hide();
//     show_io_panels();
// }

// NOW handled by the State valuations panel
// void PropertiesPanel::setSelectedItem(State* state)
// {
//   Q_UNUSED(state);
    // qDebug() << "State " << state->getId() << " selected";
    // transition_base_panel->hide();
    // transition_actions_panel->hide();
    // transition_guards_panel->hide();
    // if ( ! state->isPseudo() ) {
    //   selected_item = state;
    //   state_name_field->setText(state->getId());
    //   QStringList valuations = state->getAttrs();
    //   hide_io_panels();
    //   // state_valuations_panel->init(valuations);
    //     //show_state_panels();
    //   }
// }

// void PropertiesPanel::show_transition_base_panel(bool isInitial)
// {
//   transition_start_state_label->setVisible(!isInitial); 
//   transition_start_state_field->setVisible(!isInitial); 
//   transition_event_label->setVisible(!isInitial); 
//   transition_event_field->setVisible(!isInitial); 
//   transition_base_panel->show();
// }

// void PropertiesPanel::setSelectedItem(Transition* transition)
// {
//     selected_item = transition;
//       //hide_state_panels();
//     hide_io_panels();

//     Q_ASSERT(transition);
//     Model* model = main_window->getModel();
//     Q_ASSERT(model);
//     QStringList inpEvents = model->getInpEvents();
//     if ( inpEvents.isEmpty() ) {
//       QMessageBox::warning( this, "Error", "No input event available to trigger this transition. Please define one.");
//       // TODO: delete the concerned transition !
//       qDebug() << "Deleting transition" << transition->toString();
//       model->removeItem(transition);
//       delete transition;
//       show_io_panels();
//       return;
//       }
//     bool isInitial = transition->isInitial();
//     qDebug() << "Transition" << transition->toString() << "(" << isInitial << ")" << " selected";
//     show_transition_base_panel(isInitial);
//     if ( isInitial ) transition_guards_panel->hide();
//     else transition_guards_panel->show();
//     transition_start_state_field->clear();
//     transition_end_state_field->clear();
//     transition_actions_panel->show();
//     if ( isInitial ) {
//       foreach ( State* state, model->states() ) {
//         if ( ! state->isPseudo() ) {
//           QString id = state->getId();
//           transition_end_state_field->addItem(id, QVariant(id));
//           if ( transition->getDstState()->getId() == id ) 
//             transition_end_state_field->setCurrentIndex(transition_end_state_field->count()-1);
//           }
//         }
//       }
//     else {
//       foreach ( State* state, model->states() ) {
//         if ( ! state->isPseudo() ) {
//           QString id = state->getId();
//           transition_start_state_field->addItem(id, QVariant(id));
//           transition_end_state_field->addItem(id, QVariant(id));
//           if ( transition->getSrcState()->getId() == id )
//             transition_start_state_field->setCurrentIndex(transition_start_state_field->count()-1);
//           if ( transition->getDstState()->getId() == id ) 
//             transition_end_state_field->setCurrentIndex(transition_end_state_field->count()-1);
//           }
//         }
//       transition_event_field->clear();
//       for ( auto ev: inpEvents ) 
//         transition_event_field->addItem(ev, QVariant(ev));
//       QString event = transition->getEvent();
//       if ( event == "" ) {
//         transition_event_field->setCurrentIndex(0);
//         _setTransitionEvent(transition_event_field,transition);
//         }
//       else {
//         if ( inpEvents.contains(event) ) 
//           transition_event_field->setCurrentText(event);
//         else
//           QMessageBox::warning( this, "Error", "The triggering event for this transition is no longer listed in the model inputs");
//       }
//       clearTransitionGuardsPanel(transition_guards_layout);
//       foreach ( QString guard, transition->getGuards() ) {
//         _addTransitionGuard(guard);
//         }
//     }

//     clearTransitionActionsPanel(transition_actions_layout);
//       foreach ( QString action, transition->getActions() ) {
//         _addTransitionAction(action);
//         }
// }

void PropertiesPanel::update()
{
    // clearIos();
    // fillIos();
    fillModelName();
}

void PropertiesPanel::clear()
{
  qDebug() << "Clearing properties panel";
  selected_item = nullptr;
  // clearIos();
  clearModelName();
}



