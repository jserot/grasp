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

#include "transition.h"
#include "misc.h"
#include "compiler.h"
#include "response.h"
#include "globals.h"
#include "diagram.h"
#include "model.h"
#include <math.h>
#include <QPen>
#include <QPainter>
#include <QSet>
#include <QtDebug>
#include <QMessageBox>

#define POLYLINE_INTERSECT polyLine.intersects

const qreal Pi = 3.141592654;

QColor Transition::selectedColor = Qt::darkCyan;
QColor Transition::unSelectedColor = Qt::black;
double Transition::arrowSize = 20.0;

Transition::Transition(Diagram *d,
                       State *_srcState,
                       State *_dstState,
                       QString _event,
                       QStringList _guards,
                       QStringList _actions,
                       State::Location _location,
                       QGraphicsItem *parent)
    : QGraphicsPolygonItem(parent)
{
    enclosingDiagram = d;
    srcState = _srcState;
    dstState = _dstState;
    location = _location;
    event = _event;
    guards = _guards;
    actions = _actions;
    label = new QGraphicsSimpleTextItem(getLabel(), this);
    label->setFlag(QGraphicsItem::ItemIsSelectable, false);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setPen(QPen(unSelectedColor, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
}

Transition::~Transition() 
{
}

QString Transition::getLabel()
{
  QString r = event;
  if ( ! guards.isEmpty() ) r += "." + guards.join(".");
  if ( ! actions.isEmpty() ) r += "/" + actions.join(";");
  return r;
}

bool Transition::isInitial()
{
  return srcState ? srcState->isPseudo() : false;
}

// QRectF Transition::boundingRect() const
// {
//     qreal extra = (pen().width() + 20) / 2.0;

//     if ( srcState == dstState )
//       return
//         QRectF(polygon().at(0), polygon().at(2))
//         .normalized()
//         .adjusted(-extra, -extra, extra, extra);
//     else
//       return
//         QRectF(polygon().at(0),
//                QSizeF(polygon().at(1).x() - polygon().at(0).x(),
//                       polygon().at(1).y() - polygon().at(0).y()))
//         .normalized()
//         .adjusted(-extra, -extra, extra, extra);
// }

QPainterPath Transition::shape() const
{
    QPainterPath path = QGraphicsPolygonItem::shape();
    path.addPolygon(arrowHead);
    return path;
}

void Transition::updatePosition()
{
  QPolygonF p;
  if ( srcState == dstState ) {
    float w = State::boxSize.width();
    float h = State::boxSize.height();
    p << mapFromItem(srcState, 0, -0.25*h)
      << mapFromItem(srcState, 0, 0.25*h)
      << mapFromItem(srcState, w, 0.25*h)
      << mapFromItem(srcState, w, -0.25*h);
    }
  else
    p << mapFromItem(srcState, 0, 0)
      << mapFromItem(dstState, 0, 0);
  setPolygon(p);
}

void Transition::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    // qDebug() << "------------- Transition::paint";
    // qDebug() << "Drawing transition between state " << mySrcState->getId() << " and " << myDstState->getId();

    QPen myPen = pen();
    myPen.setColor(isSelected() ? selectedColor : unSelectedColor);
    painter->setPen(myPen);
    painter->setBrush(isSelected() ? selectedColor : unSelectedColor);

    QPolygonF points; // Drawing points
    double angle=0.0; // Of the last segment; for drawing the arrow head
    QPointF endPoint; // For anchoring the arrow head
    QPointF midPoint; // For drawing the caption

    float w = State::boxSize.width();
    float h = State::boxSize.height();

    if ( srcState == dstState ) { // Looping transition

      QPointF c = srcState->pos(); // Box center
      switch ( location ) {
      case State::East:
        points << c + QPointF(0, 0.25*h)
               << c + QPointF(w, 0.25*h)
               << c + QPointF(w, -0.25*h)
               << c + QPointF(0.5*w, -0.25*h);
        angle = 0;
        break;
      case State::West:
        points << c + QPointF(0, 0.25*h)
               << c + QPointF(-w, 0.25*h)
               << c + QPointF(-w, -0.25*h)
               << c + QPointF(-0.5*w, -0.25*h);
        angle = Pi;
        break;
      case State::North:
        points << c + QPointF(0.25*w, 0)
               << c + QPointF(0.25*w, -h)
               << c + QPointF(-0.25*w, -h)
               << c + QPointF(-0.25*w, -0.5*h);
        angle = Pi/2;
        break;
      case State::South:
      case State::None:
        points << c + QPointF(0.25*w, 0)
               << c + QPointF(0.25*w, h)
               << c + QPointF(-0.25*w, h)
               << c + QPointF(-0.25*w, 0.5*h);
        angle = -Pi/2;
        break;
      }
      endPoint = points.at(3);
      midPoint = (points.at(1) + points.at(2))/2;
    }

    else { // Normal transition

      if (srcState->collidesWithItem(dstState)) return; // Do not draw if start and end statees collide

      // Find the position where to draw the arrow head
      // This is where the line and the end state intersect

      QLineF centerLine(srcState->pos(), dstState->pos());
      QPolygonF endPolygon = dstState->polygon();
      QPointF p1 = endPolygon.first() + dstState->pos();  // Item to scene coordinates
      QPointF p2;
      QPointF intersectPoint;
      QLineF polyLine;
      int side;  // 1: North, 2: East, 3: South, 4: West
      for (side = 1; side < endPolygon.count(); ++side) {
        // Iterating over the end box sides to find the intersection (there must be exactly one)
        p2 = endPolygon.at(side) + dstState->pos();
        polyLine = QLineF(p1, p2);
        QLineF::IntersectType intersectType = POLYLINE_INTERSECT(centerLine, &intersectPoint);
        if (intersectType == QLineF::BoundedIntersection) break;
        p1 = p2;
      }
      // When there are several transitions between the start and end boxes, we don't want one to hide another.
      // To avoid this, we count them and assign a rank to each one, which will be used as an offset
      
      QList<Transition*> transitions = srcState->getTransitionsTo(dstState) + dstState->getTransitionsTo(srcState);
      // for ( auto a: transitions ) 
      //   qDebug() << "  " << a->srcState()->getId() << " -> " << a->dstState()->getId();
      transitions = remove_duplicates(transitions); 
      std::sort(transitions.begin(), transitions.end()); // Sorting ensures that the order does not depend on the start state
      int rank = -1;
      for ( int i=0; i<transitions.length() && rank<0; i++ )
        if ( transitions.at(i) == this ) rank = i;
      if ( rank == -1 ) throw std::invalid_argument(std::string("Transition::paint:compute_rank"));

      int n = transitions.length();
      QPointF offset;
      switch ( side ) {
      case 1: case 3: 
        offset = QPointF((rank+1)*w/(n+1)-w/2, 0); break;
      case 2: case 4: 
        offset = QPointF(0, (rank+1)*w/(n+1)-w/2); break;
      default:
        offset = QPoint(0,0); break; // should not happen 
      }

      QLineF line = QLineF(intersectPoint+offset, srcState->pos()+offset);
      points << line.p1() << line.p2();

      angle = ::acos(line.dx() / line.length());
      if (line.dy() >= 0) angle = (Pi * 2) - angle;
      endPoint = line.p1();
      midPoint = (line.p1() + line.p2())/2;
    }

    // setPolygon(points);  
    
    // Build arrow head  

    QPointF arrowP1 = endPoint + QPointF(sin(angle + Pi/3) * arrowSize, cos(angle + Pi/3) * arrowSize);
    QPointF arrowP2 = endPoint + QPointF(sin(angle + Pi - Pi/3) * arrowSize, cos(angle + Pi - Pi/3) * arrowSize);
    arrowHead.clear();
    arrowHead << endPoint << arrowP1 << arrowP2;

    setPolygon(points + arrowHead); // The recorded polygon now includes the arrow head
    // QRectF box = boundingRect();
    // QPen pen = painter->pen();
    // QBrush brush = painter->brush();
    // painter->setPen(QPen(Qt::blue));
    // painter->setBrush(Qt::NoBrush);
    // painter->drawRect(box);
    // painter->setPen(pen);
    // painter->setBrush(brush);

    // Draw all
    
    painter->drawPolyline(points);
    painter->drawPolygon(arrowHead);

    label->setText(getLabel());
    label->setPos(midPoint);
}

QString Transition::toString()
{
  return srcState->getId() + "->" + dstState->getId() + " [" + getLabel() + "]";
}

QDebug operator<<(QDebug d, Transition& t)
{
  d << t.srcState->getId() << "->" + t.dstState->getId() << "[" << t.guards << "/" << t.actions << "]";
  return d;
}

Response Transition::scan_guard(Fragment::Context ctx, QString guard)
{
  Fragment fragment(ctx, "guard " + guard);
  Response r = Globals::compiler->checkFragment(fragment);
  return r;
}

bool Transition::check_guard(Fragment::Context ctx, QString guard)
{
  Response r = scan_guard(ctx,guard);
  if ( ! Globals::compiler->handle_response("Transition checking", "Guard " + guard, r) ) return false;
  return true;
}

Response Transition::scan_action(Fragment::Context ctx, QString action)
{
  Fragment fragment(ctx, "action " + action);
  Response r = Globals::compiler->checkFragment(fragment);
  return r;
}

bool Transition::check_action(Fragment::Context ctx, QString action)
{
  Response r = scan_action(ctx,action);
  if ( ! Globals::compiler->handle_response("Transition checking", "Action " + action, r) ) return false;
  // Check that an output modified by an action is not assigned in the target state 
  QString lhs = action.split(":=").at(0);
  QStringList svals = dstState->getAttrs();
  for ( int i = 0; i<svals.length(); i++ ) { // It's a pity QList does not have a [map] operator ..
    QString lhs = svals.at(i).split("=").at(0);
    svals.replace(i, lhs);
    }
  if ( svals.contains(lhs) ) {
    QMessageBox::warning(Globals::mainWindow, "", "Action \"" + action + "\" sets output \"" + lhs + "\", which is already assigned in the target state"); 
        return false;
        }
  // TODO : check that an output/variable is not assigned more than once by the action (?)
  return true;
}

bool Transition::check_guards(QStringList guards)
{
  Fragment::Context ctx = {
    enclosingDiagram->potentialInputs(),// Inputs, including global variables
    enclosingDiagram->potentialOutputs(), // Outputs, including global variables
    enclosingDiagram->localVars() // Local variables
    };
  foreach ( QString guard, guards )
    if ( ! check_guard(ctx, guard) ) return false;
  return true;
}

bool Transition::check_actions(QStringList actions)
{
  Fragment::Context ctx = {
    enclosingDiagram->potentialInputs(),
    enclosingDiagram->potentialOutputs(),
    enclosingDiagram->localVars()
    };
  foreach ( QString action, actions )
    if ( ! check_action(ctx, action) ) return false;
  return true;
}

bool Transition::check()
{
  qDebug() << "Checking transition: " << toString();
  Q_ASSERT(enclosingDiagram->states().contains(getSrcState())); 
  Q_ASSERT(enclosingDiagram->states().contains(getDstState())); 
  if ( ! isInitial() ) {
    QStringList modelEvents =
        enclosingDiagram->enclosingModel()->getInpEvents()
      + enclosingDiagram->enclosingModel()->getSharedEvents();
    if ( ! modelEvents.contains(getEvent()) ) {
      Globals::compiler->report_error("Diagram checking",
                                      "Transition " + toString(),
                                      "the triggering event is not part of the enclosing model");
      return false;
      }
    }
  if ( ! isInitial() )
    if ( ! check_guards(getGuards()) ) return false;
  if ( ! check_actions(getActions()) ) return false;
  return true;
}

QPair<QMap<QString,QString>,QMap<QString,QString>> Transition::varsOf()
{
  qDebug() << "Getting vars of transition " << toString();
  //Fragment::Context ctx = enclosingDiagram->build_context(); 
  Fragment::Context ctx = {
    enclosingDiagram->potentialInputs(),
    enclosingDiagram->potentialOutputs(),
    enclosingDiagram->localVars()
    };
  QMap<QString,QString> rds, wrs;
  if ( ! isInitial() ) rds.insert(event,"event");
  foreach ( QString guard, guards ) {
    Response r = scan_guard(ctx, guard);
    if ( Globals::compiler->handle_response("Transition scanning", "Guard " + guard, r) ) {
      foreach ( QString s, r.rds() )
        if ( ! ctx.vars.contains(s) ) rds.insert(s, ctx.inps.value(s)); // Do not add local variables
      }  
    }
  foreach ( QString action, actions ) {
    Response r = scan_action(ctx, action);
    if ( Globals::compiler->handle_response("Transition scanning", "Action " + action, r) ) {
      foreach ( QString s, r.rds() )
        if ( ! ctx.vars.contains(s) ) rds.insert(s, ctx.inps.value(s));  
      foreach ( QString s, r.wrs() ) 
        if ( ! ctx.vars.contains(s) ) wrs.insert(s, ctx.outps.value(s)); 
      }  
    }
  return qMakePair(rds,wrs);
}
