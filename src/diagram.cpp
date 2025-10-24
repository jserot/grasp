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

#include "globals.h"
#include "model.h"
#include "diagram.h"
#include "transition.h"
#include "diagramProperties.h"
#include "stateProperties.h"
#include "compiler.h"
#include "transitionProperties.h"
#include "include/nlohmann_json.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QDir>
#include <QGraphicsView>
#include <QGraphicsSceneMouseEvent>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QGuiApplication>

#define QEVENT_LEAVE QEvent::GraphicsSceneLeave
#define SKIP_EMPTY_PARTS Qt::SkipEmptyParts

QString Diagram::statePrefix = "S";
int Diagram::stateCounter = 0;
QColor Diagram::lineColor = Qt::lightGray;
QColor Diagram::boxColor = Qt::black;

const int Diagram::canvas_width = 500;
const int Diagram::canvas_height = 1000;

Diagram::Diagram(
  Model *model,
  QString name,
  QList<Iov*> vars,
  QList<State*> states,
  QList<Transition*> transitions,
  QWidget *parent)
  : QGraphicsScene(parent)
{
  this->name = name;
  this->model = model;
  this->parent = parent;
  this->view = new QGraphicsView(this);
  setSceneRect(QRectF(0, 0, canvas_width, canvas_height));
  foreach ( Iov* var, vars) {
      qDebug () << "Creating diagram: adding var" << var->name << var->type;
      this->vars.append(var);
      }
    foreach ( State *state, states ) {
      qDebug () << "Creating diagram: adding state" << state->getId();
      addState(state);
      }
    foreach ( Transition *transition, transitions ) {
      qDebug () << "Creating diagram: adding transition" << transition->getSrcState()->getId() << " -> " << transition->getDstState()->getId();
      addTransition(transition);
      transition->updatePosition();
      }
    Q_ASSERT(Globals::mainWindow);
    connect(this, SIGNAL(modelModified()), Globals::mainWindow, SLOT(modelModified()));
    connect(this, SIGNAL(mouseEnter()), Globals::mainWindow, SLOT(updateCursor()));
    connect(this, SIGNAL(mouseLeave()), Globals::mainWindow, SLOT(resetCursor()));
    qDebug() << "Diagram::ctor: this=" << this << "view=" << view;
}

Diagram::Diagram(Model *model, QString name, QWidget *parent)
  : Diagram(model, name, QList<Iov*>(), QList<State*>(), QList<Transition*>(), parent)
{
}

Diagram *Diagram::duplicate()
{
    // Note: QGraphicsItems have no copy constructors...
    qDebug() << "Duplicating diagram" << name;
    QMap<State*,State*> copied_states;
    for ( State *state : this->states() ) {
      State *copied_state = 
        state->isPseudo() ?
          new State(state->pos())
        : new State(state->getId(), state->getAttrs(), state->pos());
      copied_states.insert(state,copied_state);
      }   
    QList<Transition *> copied_transitions;
    for ( Transition *transition : this->transitions() ) {
      State *srcState = copied_states.value(transition->getSrcState());
      State *dstState = copied_states.value(transition->getDstState());
      Transition *copied_transition = new Transition(srcState,
                                              dstState,
                                              transition->getEvent(),
                                              transition->getGuards(),
                                              transition->getActions(),
                                              transition->getLocation());
      copied_transitions.append(copied_transition);
      }
    qDebug() << "Copied transitions" << copied_transitions;
    QList<Iov *> copied_vars;
    for ( Iov *var : this->vars ) {
      Iov *copied_var = new Iov(var->name, var->kind, var->type, var->stim); 
      copied_vars.append(copied_var);
      }
    QString new_name = name + name.last(1); // S0 -> S00 
    Diagram *copied_diagram =
      new Diagram(this->model, new_name, copied_vars, copied_states.values(), copied_transitions, parent); 
    return copied_diagram;
}

Diagram::~Diagram()
{
  qDebug() << "Diagram::delete:" << this;
  for ( Iov* var: vars ) delete var;
  // for ( State* state: states.values() ) delete state; // States and transitions, being GraphicsItem will be deleted when the ...
  // for ( Transition* transition: transitions ) delete transition; // ... QGraphicsScene destructor, as a parent, will be called (?)
  // Q_ASSERT(view);
  // delete view;  // This crashes the program when the app is closed
}


Iov* Diagram::addVar(const QString name, const Iov::IoType type)
{
  qDebug () << "Diagram::addVar" << name << type;
  Iov *var = new Iov(name, Iov::IoVar, type, Stimulus("None"));
  vars.append(var);
  return var;
}

void Diagram::removeVar(Iov *var)
{
  vars.removeOne(var);
}

void Diagram::clear(void)
{
  name = "";
  vars.clear();
  stateCounter = 0;
  QGraphicsScene::clear();
}

void Diagram::addState(State *state)
{
  state->setBrush(boxColor);
  addItem(state);
}

State* Diagram::addState(QPointF pos, QString id, QStringList attrs)
{
  State* state = new State(id, attrs);
  state->setPos(pos);
  addState(state);
  return state;
}

State* Diagram::addPseudoState(QPointF pos)
{
  State* state = new State(); // Pseudo-state
  state->setPos(pos);
  addState(state);
  return state;
}

void Diagram::editState(State *state)
{
  qDebug() << "Editing state" << state->getId();
  StateProperties *dialog = new StateProperties(state, this, view);
  dialog->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint | Qt::WindowSystemMenuHint);
  dialog->setAttribute(Qt::WA_DeleteOnClose);
  dialog->show();
  // NOTE : the following _modal_ formulation seems not to work correctly
  //        When returning from the dialog the main GUI loop sometimes becomes partially unresponsive
  // StateProperties dialog(state, this, view);
  // int r = dialog.exec();
  // qDebug() << "state properties dialog returned" << r;
  // switch ( r ) {
  //   case QDialog::Accepted:
  //     qDebug() << "state" << state->getId() << "updated";
  //     update();
  //     emit modelModified(); To main window
  //     break;
  //   case QDialog::Rejected:
  //     qDebug() << "state" << state->getId() << "unchanged";
  //     break;
  //  }
  state->setSelected(false);
}

void Diagram::editTransition(Transition *transition)
{
  qDebug() << "Editing transition" << transition->toString();
  bool isInitial = transition->isInitial();
  QStringList inpEvents = model->getInpEvents();
  if ( !isInitial && inpEvents.isEmpty() ) {
      QMessageBox::warning(Globals::mainWindow, "Error", "No input event available to trigger this transition. Please define one.");
      qDebug() << "No input event. Removing transition" << transition->toString();
      removeTransition(transition);
      return;
      }
  TransitionProperties *dialog = new TransitionProperties(transition,this,isInitial,view);
  dialog->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint | Qt::WindowSystemMenuHint);
  dialog->setAttribute(Qt::WA_DeleteOnClose);
  dialog->show();
  // NOTE : the following _modal_ formulation seems not to work correctly
  //        When returning from the dialog the main GUI loop sometimes becomes partially unresponsive
  // TransitionProperties dialog(transition,this,isInitial,view);
  // if ( dialog.exec() == QDialog::Accepted ) {
  //   qDebug() << "Transition" << transition->toString() << "updated";
  //   update();
  //   emit modelModified(); // To main window
  //   }
}

QList<State*> Diagram::states()
{
  QList<State*> states;
  for ( const auto item: items() )
    if ( item->type() == State::Type )
      states.append(qgraphicsitem_cast<State *>(item));
  return states;
}

QList<Transition*> Diagram::transitions()
{
  QList<Transition*> transitions;
  for ( const auto item: items() )
    if ( item->type() == Transition::Type )
      transitions.append(qgraphicsitem_cast<Transition *>(item));
  return transitions;
}

State* Diagram::initState()
{
  QList<Transition*> transitions;
  for ( const auto item: items() )
    if ( item->type() == Transition::Type ) {
      Transition *t = qgraphicsitem_cast<Transition *>(item);
      if ( t->isInitial() ) return t->getDstState();
      }
  return NULL;
}

Transition* Diagram::initTransition()
{
  QList<Transition*> transitions;
  for ( const auto item: items() )
    if ( item->type() == Transition::Type ) {
      Transition *t = qgraphicsitem_cast<Transition *>(item);
      if ( t->isInitial() ) return t;
      }
  return NULL;
}

State* Diagram::getState(QString id)
{
  foreach ( State* s, states() )
    if ( s->getId() == id ) return s;
  return NULL;
}

QList<Iov*> Diagram::getVars()
{
  return vars;
}

QStringList Diagram::getVarNames()
{
  QStringList r;
  for ( const auto var : vars )
    r.append(var->name);
  return r;
}

bool Diagram::hasPseudoState()
{
  foreach ( State* s, states() )
    if ( s->isPseudo() ) return true;
  return false;
}

void Diagram::addTransition(Transition *transition)
{
  State *srcState = transition->getSrcState();
  State *dstState = transition->getDstState();
  srcState->addTransition(transition);
  if ( dstState != srcState ) dstState->addTransition(transition); // Do _not_ add self-transitions twice !
  transition->setZValue(-1000.0);
  addItem(transition);
}

Transition* Diagram::addTransition(State* srcState,
                               State* dstState,
                               QString event,
                               QStringList guards,
                               QStringList actions,
                               State::Location location)
{
  Transition *transition = new Transition(srcState, dstState, event, guards, actions, location);
  addTransition(transition);
  return transition;
}

bool Diagram::event(QEvent *event)
{
  // qDebug() << "Got event " << event->type();
  switch ( event->type() ) {
    // Note. The [Enter] and [Leave] events cannot be handled by the model itself
    // because the associated action [setCursor] can only be applied to the _enclosing_ view...
    // This workaround uses signals to delegate 
    case QEvent::Enter:
      emit mouseEnter();
      return true;
    case QEVENT_LEAVE:
      emit mouseLeave();
      return true;
    default:
        break;
    }
  return QGraphicsScene::event(event);
}

void Diagram::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    State *state;
    Transition *transition;
    QGraphicsItem *item;
    Qt::MouseButton buttonPressed = mouseEvent->button();
    qDebug() << "Diagram::mousePressEvent: " << buttonPressed << QGuiApplication::keyboardModifiers();
    switch ( buttonPressed ) {
    case Qt::LeftButton:
      switch ( Globals::mode ) {
        case Globals::InsertState:
          state = addState(mouseEvent->scenePos(), statePrefix + QString::number(stateCounter++), QStringList());
          editState(state);
          break;
        case Globals::InsertPseudoState:
          if ( ! hasPseudoState() ) {
            startState = addPseudoState(mouseEvent->scenePos());
            line = new QGraphicsLineItem(QLineF(mouseEvent->scenePos(), mouseEvent->scenePos()));
            line->setPen(QPen(lineColor, 2));
            addItem(line);
            }
          else
            QMessageBox::warning(Globals::mainWindow, "Error",
                                 "There's already one initial transition !\nDelete it first to add another one");
          break;
        case Globals::InsertTransition:
          line = new QGraphicsLineItem(QLineF(mouseEvent->scenePos(), mouseEvent->scenePos()));
          line->setPen(QPen(lineColor, 2));
          addItem(line);
          break;
        case Globals::InsertLoopTransition:
          item = itemAt(mouseEvent->scenePos(), QTransform());
          if ( item != NULL && item->type() == State::Type ) {
            state = qgraphicsitem_cast<State *>(item);
            if ( ! state->isPseudo() ) {
              State::Location location = state->locateEvent(mouseEvent);
              Transition *transition = addTransition(state, state, "", QStringList(), QStringList(), location);
              transition->updatePosition();
              editTransition(transition);
              }
            }
            break;
        case Globals::DeleteItem:
          item = itemAt(mouseEvent->scenePos(), QTransform());
          if ( item != NULL ) {
            switch ( item->type() ) {
            case Transition::Type:
              transition = qgraphicsitem_cast<Transition *>(item);
              Q_ASSERT(transition);
              removeTransition(transition);
              break;
            case State::Type:
              state = qgraphicsitem_cast<State *>(item);
              Q_ASSERT(state);
              removeState(state);
              break;
            default:
              break;
            }
          }
          break;
        case Globals::SelectItem:
            qDebug() << "** SelectItem at" << mouseEvent->scenePos();
            qDebug() << "** Existing items:";
            foreach (QGraphicsItem* item, items()) 
              qDebug() << "     " << item << item->scenePos() << item->boundingRect();
            item = itemAt(mouseEvent->scenePos(), QTransform());
            qDebug() << "** SelectItem got" << item;
            if ( item != NULL ) {
              // if ( QGuiApplication::keyboardModifiers().testFlag(Qt::ControlModifier) ) // LeftClick+Ctl
              //   editItem(item);
              // else
                QGraphicsScene::mousePressEvent(mouseEvent); // Default behavior (select and move, ...)
              }
        } // Mode
      break;
    case Qt::RightButton: // if ( mode == SelectItem ) 
      item = itemAt(mouseEvent->scenePos(), QTransform());
      qDebug() << "** RightSelectItem got" << item;
      if ( item != NULL )
        editItem(item);
      else {
        edit();
        }
      break;
    default:
      break;
    } // buttonPressed
}

void Diagram::editItem(QGraphicsItem *item)
{
  Q_ASSERT(item);
  switch ( item->type() ) {
  case State::Type:
    qDebug() << "Diagram::editItem: state " << *qgraphicsitem_cast<State *>(item);
    editState(qgraphicsitem_cast<State *>(item));
    break;
  case Transition::Type:
    qDebug() << "Diagram::editItem transition" << *qgraphicsitem_cast<Transition *>(item);
    editTransition(qgraphicsitem_cast<Transition *>(item));
    break;
  default:
    break;
  }
}

void Diagram::edit()
{
  DiagramProperties *dialog = new DiagramProperties(this, this->view);
  dialog->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint | Qt::WindowSystemMenuHint);
  dialog->setAttribute(Qt::WA_DeleteOnClose);
  dialog->show();  // Non-modal diagram
}

// void Diagram::contextMenuEvent(QGraphicsSceneContextMenuEvent *contextMenuEvent)
// {
//   QGraphicsItem *item = itemAt(contextMenuEvent->scenePos(), QTransform());
//   if ( item ) {
//     item->setSelected(true);
//     editItem(item);
//     }
//   QGraphicsScene::contextMenuEvent(contextMenuEvent);
// }

void Diagram::removeState(State *state)
{
  qDebug() << "Removing state" << state->getId();
  state->removeTransitions();
  removeItem(state);
  emit modelModified();
  delete state;
}

void Diagram::removeTransition(Transition *transition)
{
  qDebug() << "Diagram::removeTransition" << transition->toString();
  State *srcState = transition->getSrcState();
  State *dstState = transition->getDstState();
  if ( transition->isInitial() ) {
    srcState->removeTransitions();
    removeState(srcState);
    }
  else {
    srcState->removeTransition(transition);
    dstState->removeTransition(transition);
    }
  removeItem(transition);
  emit modelModified();
  delete transition;
}

void Diagram::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
  if ( (Globals::mode == Globals::InsertTransition || Globals::mode == Globals::InsertPseudoState) && line != 0 ) {
    QLineF newLine(line->line().p1(), mouseEvent->scenePos());
    line->setLine(newLine);
    }
  else if (Globals::mode == Globals::SelectItem) {
    QGraphicsScene::mouseMoveEvent(mouseEvent);
    }
}

void Diagram::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
  Qt::MouseButton buttonPressed = mouseEvent->button();
  if ( buttonPressed != Qt::LeftButton ) return;
  if ( line != 0 && (Globals::mode == Globals::InsertTransition || Globals::mode == Globals::InsertPseudoState) ) {
    QList<QGraphicsItem *> srcStates = items(line->line().p1());
    if (srcStates.count() && srcStates.first() == line) srcStates.removeFirst();
    QList<QGraphicsItem *> dstStates = items(line->line().p2());
    if (dstStates.count() && dstStates.first() == line) dstStates.removeFirst();
    removeItem(line);
    delete line;
    if (srcStates.count() > 0 && dstStates.count() > 0 &&
        srcStates.first()->type() == State::Type &&
        dstStates.first()->type() == State::Type) {
      State *srcState = qgraphicsitem_cast<State *>(srcStates.first());
      State *dstState = qgraphicsitem_cast<State *>(dstStates.first());
      if ( srcState != dstState ) {
        State::Location location = srcState == dstState ? srcState->locateEvent(mouseEvent) : State::None;
        Transition *transition = addTransition(srcState, dstState, "", QStringList(), QStringList(), location);
        transition->updatePosition();
        editTransition(transition);
        emit modelModified();
        }
      }
    else if ( Globals::mode == Globals::InsertPseudoState && startState != NULL ) {
      // An initial pseudo-state has been created but not connected
      removeItem(startState);
      delete startState;
      }
    }
  line = 0;
  QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

bool Diagram::isItemChange(int type)
{
    foreach (QGraphicsItem *item, selectedItems()) {
        if (item->type() == type)
            return true;
    }
    return false;
}

// Checking

void Diagram::report_error(QString loc, QString msg)
{
  QMessageBox::warning(Globals::mainWindow, tr("Diagram checking"), loc + "\n\n" + msg);
}

bool Diagram::check_response(QString loc, Response r)
{
  if ( r.kind() == Response::Kind::Checked ) {
    if ( r.result() == true )
      return true;
    else { // Fragment checking failed
      report_error(loc, r.message());
      return false;
      }
    }
  else {
    qDebug() << "Wrong response to check_fragment request: " << r.message();
    QMessageBox::critical(Globals::mainWindow, tr("Diagram checking"), r.message());
    return false;
    }
}

void Diagram::check_state(State *s)
{
  Q_ASSERT(states().contains(s)); 
}

QList<QPair<QString,QString>> Diagram::getLocalVars()
{
  QList<QPair<QString,QString>> r;
  for ( const auto v : vars )
    r.append(qMakePair(v->name,Iov::stringOfType(v->type)));
  return r;
}

bool Diagram::check_transition(Transition *t)
{
  qDebug() << "Checking transition: " << t->toString();
  check_state(t->getSrcState());
  check_state(t->getDstState());
  if ( ! t->isInitial() ) {
    QStringList modelEvents = enclosingModel()->getInpEvents() + enclosingModel()->getSharedEvents();
    if ( ! modelEvents.contains(t->getEvent()) ) {
      report_error("Transition " + t->toString(),  "the triggering event is not / no longer part of the enclosing model");
      return false;
      }
    }
  // TODO: following checks should be shared with those performed by the [transitionProperties] class
  if ( Globals::check_model ) { // TO REMOVE ?? 
    QList<QPair<QString,QString>> inps = model->getInputs();
    QList<QPair<QString,QString>> outps = model->getOutputs();
    QList<QPair<QString,QString>> vars = model->getShared() + getLocalVars(); 
    if ( ! t->isInitial() ) { // Check guards if non-initial transition
      foreach ( QString guard, t->getGuards()) {
        qDebug() << "Checking guard: " << guard;
        Fragment fragment(inps, outps, vars, "guard " + guard);
        Response r = Globals::compiler->checkFragment(fragment);
        qDebug() << "Got response: " << r.toString();
        if ( ! check_response("Guard " + guard, r) ) return false;
        }
      }
    foreach ( QString action, t->getActions()) { // Check actions 
        Fragment fragment(inps, outps, vars, "action " + action);
        Response r = Globals::compiler->checkFragment(fragment);
        if ( ! check_response("Action " + action, r) ) return false;
        }
    // TODO: check state valuations
    }
  return true;
}

bool Diagram::check()
{
  if ( name.isEmpty() ) {
    report_error("Current diagram", "No name specified for diagram");
    return false;
    }
  if ( ! initState() || ! initTransition() ) {
    report_error("Diagram " + name, "No initial state/transition");
    return false;
    }
  for ( Transition *t : transitions() ) 
    if ( ! check_transition(t) ) return false;
  return true;
}

// Reading and saving

Diagram* Diagram::fromJson(nlohmann::json& json, Model *model, QWidget *parent)
{
    qDebug() << "Reading diagram from JSON state";

    QString name = QString::fromStdString(json.at("name"));

    QMap<std::string, State*> states;
    for ( auto & json_state : json.at("states") ) {
      std::string id = json_state.at("id");
      std::string attrs = json_state.at("attr");
      qreal x =  json_state.at("x");
      qreal y =  json_state.at("y");
      State* state;
      if ( id == State::initPseudoId.toStdString() )
        state = new State(QPointF(x,y));
      else 
        state = new State(QString::fromStdString(id),
                          QString::fromStdString(attrs).split(",",SKIP_EMPTY_PARTS),
                          QPointF(x,y));
      states.insert(id, state);
      }   

    QList<Transition *> transitions;
    for ( auto & json_transition : json.at("transitions") ) {
      std::string src_state = json_transition.at("src_state");
      std::string dst_state = json_transition.at("dst_state");
      std::string event = json_transition.at("event");
      std::string guards = json_transition.at("guard");
      std::string actions = json_transition.at("actions");
      State::Location location;
      switch ( (int)json_transition.at("location") ) {
        case 1: location = State::North; break;
        case 2: location = State::South; break;
        case 3: location = State::East; break;
        case 4: location = State::West; break;
        default: location = State::None; break;
        }
      if ( ! states.contains(src_state) || ! states.contains(dst_state) )
        throw std::invalid_argument("Diagram::fromString: invalid state id");
      State *srcState = states.value(src_state);
      State *dstState = states.value(dst_state);
      Transition *transition = new Transition(srcState,
                                              dstState,
                                              QString::fromStdString(event),
                                              QString::fromStdString(guards).split(",",SKIP_EMPTY_PARTS),
                                              QString::fromStdString(actions).split(",",SKIP_EMPTY_PARTS),
                                              location);
      transitions.append(transition);
      }

    QList<Iov *> vars;
    for ( auto & json_var : json.at("vars") ) {
      std::string name = json_var.at("name");
      std::string type = json_var.at("type");
      Iov *var = new Iov(QString::fromStdString(name),
                        Iov::IoVar,
                        Iov::ioTypeOfString(QString::fromStdString(type)),
                        Stimulus("None"));
      vars.append(var);
      }

    // ... and, if (and only if) parsing succeeds, we update the model.
    return new Diagram(model, name, vars, states.values(), transitions, parent);
}

void Diagram::toJson(nlohmann::json& json_top)
{
    json_top["name"] = this->name.toStdString();

    json_top["vars"] = nlohmann::json::array();
    int cnt = 1;
    for ( const Iov* io: this->vars ) {
      nlohmann::json json;
      if ( io->name == "" ) {
        QMessageBox::warning(Globals::mainWindow, "Warning", tr("Var #%1").arg(cnt) + " has no name. Ignoring it");
        continue;
        }
      json["name"] = io->name.toStdString(); 
      json["type"] = Iov::stringOfType(io->type).toStdString(); 
      json_top["vars"].push_back(json);
      cnt++;
      }

    json_top["states"] = nlohmann::json::array();
    for ( const auto item: items() ) {
      if ( item->type() == State::Type ) {
        State* state = qgraphicsitem_cast<State *>(item);
        nlohmann::json json;
        json["id"] = state->getId().toStdString(); 
        json["attr"] = state->getAttrs().join(",").toStdString(); // Use "," as separator for compatibility with existing .fsd files
        json["x"] = state->scenePos().x(); 
        json["y"] = state->scenePos().y(); 
        json_top["states"].push_back(json);
        }
      }

    json_top["transitions"] = nlohmann::json::array();
    for ( const auto item: items() ) {
      if ( item->type() == Transition::Type ) {
        Transition* transition = qgraphicsitem_cast<Transition *>(item);
        nlohmann::json json;
        json["src_state"] = transition->getSrcState()->getId().toStdString();
        json["dst_state"] = transition->getDstState()->getId().toStdString();
        json["event"] = transition->getEvent().toStdString();
        json["guard"] = transition->getGuards().join(",").toStdString(); // Use "," as separator for compatibility with existing .fsd files
        json["actions"] = transition->getActions().join(",").toStdString(); // Use "," as separator for compatibility with existing .fsd files
        json["location"] = transition->getLocation();
        json_top["transitions"].push_back(json);
        }
      }
}

// DOT export

QString dotTransitionLabel(QString label, QString lrpad="")
{
  QStringList l = label.split("/",SKIP_EMPTY_PARTS);
  if ( l.length() != 2 ) return label;
  int n = std::max(l.at(0).length(), l.at(1).length());
  return lrpad + l.at(0) + lrpad
       + "\n" + lrpad + QString(n, '_') + lrpad  + "\n"
       + lrpad + l.at(1) + lrpad;
}

QString Diagram::qual_id(QString id) { return id + "_" + name; }

void Diagram::exportDot(QTextStream &os)
{
  if ( ! vars.isEmpty() ) 
    os << qual_id("_vars") << " [label=\"" << Iov::stringOfList(vars,false) << "\", shape=rect, style=rounded]\n";
  for ( const auto item: items() ) {
    if ( item->type() == State::Type ) {
      State* state = qgraphicsitem_cast<State *>(item);
      QString id = state->getId();
      if ( state->isPseudo() ) {
        os << qual_id(id) << " [shape=point]\n";
        }
      else {
        QStringList attrs = state->getAttrs();
        QString lbl = id;
        foreach ( QString attr, attrs) lbl += "\n" + attr;
        os << qual_id(id) << " [label=\"" << lbl <<  "\", shape=circle, style=solid]\n";
        }
      }
    }
  for ( const auto item: items() ) {
    if ( item->type() == Transition::Type ) {
      Transition* transition = qgraphicsitem_cast<Transition *>(item);
      QString src_id = transition->getSrcState()->getId();
      QString dst_id = transition->getDstState()->getId();
      QString label = dotTransitionLabel(transition->getLabel());
      os << qual_id(src_id) << " -> " << qual_id(dst_id) << " [label=\"" << label << "\"]\n";
      }
    }
}

// RFSM export

QString stringOfVarList(QList<Iov*> vs, QString sep=", ", bool withType=true) 
{
  QStringList rs;
  for (const auto& v : vs) {
    if ( withType )
      rs << v->name + ": " + Iov::stringOfType(v->type);
    else
      rs << v->name;
    }
  return rs.join(sep);
}

QString stringOfTransition(Transition *t, bool abbrev=false)
{
  QString ss;
  ss = t->getSrcState()->getId() + " -> " + t->getDstState()->getId();
  if ( ! abbrev ) {
    if ( ! t->getEvent().isEmpty() ) ss += " on " + t->getEvent();
    if ( ! t->getGuards().isEmpty() ) {
      QStringList guards = t->getGuards();
      if ( guards.length() > 1 ) {
        for ( int i = 0; i<guards.length(); i++ )
          guards.replace(i, "(" + guards.at(i) + ")"); 
        }
      ss += " when " + guards.join(".");
      }
    if ( ! t->getActions().isEmpty() ) ss += " with " + t->getActions().join(",");
    }
  return ss;
}


QString stringOfState(State *s)
{
  QString ss;
  ss = s->getId();
  QStringList attrs = s->getAttrs();
  if ( ! attrs.isEmpty() ) 
    ss += " where " + attrs.join(" and ");
  return ss;
}

QString stringOfIoKind(Iov::IoKind k)
{
  switch ( k ) {
  case Iov::IoIn: return QString("in");
  case Iov::IoOut: return QString("out");
  case Iov::IoVar: return QString("inout");
  }
}


// QList<Iov*> getActualIos(QList<Iov*> global_ios)
// // Extract, from the list [global_ios] those used in the guards and actions
// {
//   QList<Iov*> result;
//   for ( Transition* transition : transitions() ) {
//   }
// }

void Diagram::exportRfsmInstance(QTextStream& os, QList<Iov*>& global_ios)
{
    // TO FIX : not all global IOs should be used as instance parameters
    // Each instance model should be able to use a subset of the global IOs
    // This subset could be be computed from the rd/wr variable set derived from the transition rules
    os << "fsm " << name << " = " << name << "(";
    bool first = true;
    for(const auto io : global_ios) {
      if ( !first ) os << ", ";
      os << io->name;
      first = false;
      }
    os << ")\n";
}

void Diagram::exportRfsmModel(QTextStream& os, QList<Iov*>& global_ios)
{
    QString indent = QString(2, ' ');
    bool first;

    if ( check() == false ) return;
    // TODO : compute actual_ios using an extension of the fragment checker mechanism
    // For now, let's assume local_ios = global_ios (i.e. all diagrams take all IOs
    //QList<Iov*> actual_ios;
    os << "fsm model " << name << "(";
    if ( global_ios.length() > 0 ) {
      os << "\n";
      first = true;
      for(const auto io : global_ios) {
            if(!first) os << "," << "\n";
            os << indent;
            os << stringOfIoKind(io->kind) << " " << io->name << ": " << Iov::stringOfType(io->type);
            first = false;
        }
      os << "\n" << indent <<  ")" << "\n";
      }
    else
      os << ")" << "\n";

    os << "{" << "\n";

    os << indent << "states: ";
    first = true;
    for ( State* state : states()) {
        if ( state->isPseudo() ) continue;
        if(!first) os << ", " ;
        os << stringOfState(state);
        first = false;
        }
    os << ";" << "\n";

    if(!vars.isEmpty()) 
      os << indent << "vars: " << stringOfVarList(vars) <<  ";" << "\n";

    os << indent;
    os << "trans: ";
    first = true;

    for ( Transition* transition : transitions() ) {
        if ( transition->getSrcState()->isPseudo() ) continue;
        os << indent << "\n  | " << stringOfTransition(transition);
        first = false;
        }
    os << ";" << "\n";

    State* iState = initState();
    Transition* iTransition = initTransition();
    Q_ASSERT(iState);  // This has been checked by [check]
    Q_ASSERT(iTransition);
    os << indent << "itrans: " << "\n";
    os << indent << "| -> " << iState->getId();
    qDebug() << "iacts=" << iTransition->getActions();
    if ( ! iTransition->getActions().isEmpty() ) os << " with " << iTransition->getActions().join(",");
    os << ";" << "\n";
    os << "}\n";
}

void Diagram::dump() // For debug only
{
  qDebug() << "Diagram " << name;
  qDebug() << "  vars =";
  foreach ( Iov* var, vars )
    qDebug() << "    " <<  var->toString();
  qDebug() << "  states =";
  foreach ( State* s, states() )
    qDebug() << "    " << *s;
  qDebug() << "  transitions =";
  foreach ( Transition* t, transitions() )
    qDebug() << "    " << *t;
  if ( initState() ) qDebug() << "  initial state =" << *initState();
  if ( initTransition() ) qDebug() << "  initial transition =" << *initTransition();
}
