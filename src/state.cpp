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

#include "state.h"
#include "transition.h"

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QtDebug>

QSize State::dskSize = QSize(15,15);
QSize State::boxSize = QSize(100,70);
QColor State::boxBackground = Qt::white;
QColor State::selectedColor = Qt::darkCyan;
QColor State::unSelectedColor = Qt::black;
QString State::initPseudoId = "_init";

void State::init(QString id, QStringList attrs, QSize sz)
{
    QPainterPath path;
    myPolygon << QPointF(-sz.width()/2, -sz.height()/2) // P1-----P2
              << QPointF(sz.width()/2, -sz.height()/2)  //  |     |
              << QPointF(sz.width()/2, sz.height()/2)   //  |     |
              << QPointF(-sz.width()/2, sz.height()/2)  // P4-----P3
              << QPointF(-sz.width()/2, -sz.height()/2);
    setPolygon(myPolygon);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    this->id = id;
    this->attrs = attrs;
}

State::State(QString id, QStringList attrs, QGraphicsItem *parent)
    : QGraphicsPolygonItem(parent)
{
  init(id, attrs, boxSize);
  isPseudoState = false;
}

State::State(QString id, QStringList attrs, QPointF pos, QGraphicsItem *parent)
    : QGraphicsPolygonItem(parent)
{
  init(id, attrs, boxSize);
  isPseudoState = false;
  setPos(pos);
}
  
State::State(QGraphicsItem *parent)
    : QGraphicsPolygonItem(parent)
{
  init(initPseudoId, attrs, dskSize);
  isPseudoState = true;
}

State::State(QPointF pos, QGraphicsItem *parent)
    : QGraphicsPolygonItem(parent)
{
  init(initPseudoId, attrs, dskSize);
  isPseudoState = true;
  setPos(pos);
}

void State::removeTransition(Transition *transition)
{
    int index = transitions.indexOf(transition);

    if (index != -1)
        transitions.removeAt(index);
}

void State::removeTransitions()
{
    qDebug() << "Removing" << transitions.length() <<  "transition(s) to/from state" << getId();
    foreach (Transition *transition, transitions) {
      State *srcState = transition->getSrcState();
      State *dstState = transition->getDstState();
      Q_ASSERT(srcState);
      Q_ASSERT(dstState);
      srcState->removeTransition(transition);
      dstState->removeTransition(transition);
      scene()->removeItem(transition);
      delete transition;
    }
}

void State::addTransition(Transition *transition)
{
    transitions.append(transition);
}

void State::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
  painter->setRenderHint(QPainter::Antialiasing);
  if ( isPseudoState ) {
    painter->setBrush(Qt::black);
    painter->drawEllipse(myPolygon.boundingRect());
    }
  else {
    painter->setPen(QPen(isSelected() ? selectedColor : unSelectedColor, 1));
    painter->setBrush(boxBackground);
    // painter->drawPolygon(myPolygon);
    // painter->setRenderHint(QPainter::Antialiasing);
    QPainterPath path;
    path.addRoundedRect(myPolygon.boundingRect(), 10, 10);
    painter->drawPath(path); 
    QString lbl = id;
    foreach ( QString attr, attrs)
      lbl += "\n" + attr;
    painter->drawText(boundingRect(), Qt::AlignHCenter | Qt::AlignVCenter, lbl);
    }
}

QVariant State::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemPositionChange) {
        foreach (Transition *transition, transitions) {
            transition->updatePosition();
        }
    }
    return value;
}

QList<Transition*> State::getTransitionsTo(State* dstState)
{
  QList<Transition *> res;
  for ( auto a : transitions ) 
    if ( a->getDstState() == dstState ) res.append(a);
  return res;
}

QList<Transition*> State::getTransitionsFrom(State* srcState)
{
  QList<Transition *> res;
  for ( auto a : transitions ) 
    if ( a->getSrcState() == srcState ) res.append(a);
  return res;
}

QList<Transition*> State::getTransitionsOut()
{
  QList<Transition *> res;
  for ( auto a : transitions ) 
    if ( a->getSrcState() == this ) res.append(a);
  return res;
}

QList<Transition*> State::getTransitionsIn()
{
  QList<Transition *> res;
  for ( auto a : transitions ) 
    if ( a->getDstState() == this ) res.append(a);
  return res;
}

State::Location State::locateEvent(QGraphicsSceneMouseEvent *event)
{
  QPointF p = event->scenePos();
  QPointF c = mapToScene(0,0);
  QPolygonF r = mapToScene(boundingRect());
  QPolygonF north, east, south, west;
  north << c << r.at(0) << r.at(1) << c;
  east << c << r.at(1) << r.at(2) << c;
  south << c << r.at(2) << r.at(3) << c;
  west << c << r.at(3) << r.at(0) << c;
  if ( north.containsPoint(p,Qt::OddEvenFill) ) return State::North;
  else if ( east.containsPoint(p,Qt::OddEvenFill) ) return State::East;
  else if ( south.containsPoint(p,Qt::OddEvenFill) ) return State::South;
  else if ( west.containsPoint(p,Qt::OddEvenFill) ) return State::West;
  return State::None;
}

QDebug operator<<(QDebug d, const State& s)
{
  d << s.id << ", " << s.attrs;
  return d;
}
