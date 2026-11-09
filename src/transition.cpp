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
double Transition::hitWidth = 12.0;

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
    label->setFlag(QGraphicsItem::ItemStacksBehindParent, true); // A click on the label selects the transition (see shape)
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

QRectF Transition::boundingRect() const
{
    return QGraphicsPolygonItem::boundingRect().united(hitShape.boundingRect());
}

QPainterPath Transition::shape() const
{
    // The "active" (clickable) zone of the transition : the drawn line (with a tolerance), the arrow head and the label
    return hitShape.isEmpty() ? QGraphicsPolygonItem::shape() : hitShape;
}

// Updates the geometry of all the transitions between [a] and [b], whose positions depend on each other (see computeGeometry)
static void updateTransitionsBetween(State *a, State *b)
{
  QList<Transition*> ts = remove_duplicates(a->getTransitionsTo(b) + b->getTransitionsTo(a));
  for ( Transition *t : ts ) t->updatePosition();
}

void Transition::setStates(State *src, State *dst)
{
  Q_ASSERT(src && dst);
  if ( src == srcState && dst == dstState ) return;
  State *oldSrc = srcState, *oldDst = dstState;
  srcState->removeTransition(this);
  dstState->removeTransition(this);
  srcState = src;
  dstState = dst;
  srcState->addTransition(this);
  if ( dstState != srcState ) dstState->addTransition(this); // Do _not_ add self-transitions twice (see Diagram::addTransition)
  updateTransitionsBetween(oldSrc, oldDst); // The remaining ones
  updateTransitionsBetween(srcState, dstState); // Including this one
}

void Transition::updateSiblings()
{
  updateTransitionsBetween(srcState, dstState);
}

// Computes the drawn line, the arrow head and the label position.
// Note: when there are several transitions between two states, they are drawn with an offset, depending on their rank.
void Transition::computeGeometry()
{
    drawnPoints.clear();
    arrowHead.clear();

    double angle=0.0; // Of the last segment; for drawing the arrow head
    QPointF endPoint; // For anchoring the arrow head
    QPointF midPoint; // For drawing the caption

    QPolygonF points; // Drawing points

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

      if (srcState->collidesWithItem(dstState)) return; // Nothing is drawn if the start and end states collide

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
      if ( rank == -1 ) rank = 0; // Should not happen (the transition lists of the states are inconsistent)

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

    // Build arrow head  

    QPointF arrowP1 = endPoint + QPointF(sin(angle + Pi/3) * arrowSize, cos(angle + Pi/3) * arrowSize);
    QPointF arrowP2 = endPoint + QPointF(sin(angle + Pi - Pi/3) * arrowSize, cos(angle + Pi - Pi/3) * arrowSize);
    arrowHead << endPoint << arrowP1 << arrowP2;
    drawnPoints = points;
    labelPos = midPoint;
}

void Transition::updatePosition()
{
  prepareGeometryChange();
  computeGeometry();
  setPolygon(drawnPoints + arrowHead);
  label->setText(getLabel());
  label->setPos(labelPos);
  // Active zone
  hitShape = QPainterPath();
  if ( drawnPoints.isEmpty() ) return;
  QPainterPath line;
  line.addPolygon(drawnPoints); // Open polyline
  QPainterPathStroker stroker;
  stroker.setWidth(hitWidth);
  stroker.setCapStyle(Qt::RoundCap);
  stroker.setJoinStyle(Qt::RoundJoin);
  // Note: the parts must be united. Simply adding them to the same path leaves holes where they overlap
  // (the arrow head and the line, for ex.), whatever the fill rule
  QPainterPath arrow;
  arrow.addPolygon(arrowHead);
  arrow.closeSubpath();
  hitShape = stroker.createStroke(line).united(arrow);
  if ( ! label->text().isEmpty() ) {
    QPainterPath labelZone;
    labelZone.addRect(label->mapRectToParent(label->boundingRect()));
    hitShape = hitShape.united(labelZone);
    }
}

void Transition::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    // Note: the geometry is computed by [updatePosition], not here
    if ( drawnPoints.isEmpty() ) return; // Start and end states collide
    QPen myPen = pen();
    myPen.setColor(isSelected() ? selectedColor : unSelectedColor);
    painter->setPen(myPen);
    painter->setBrush(isSelected() ? selectedColor : unSelectedColor);
    painter->drawPolyline(drawnPoints);
    painter->drawPolygon(arrowHead);
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

static Fragment::Context checkContext(Diagram *diagram)
{
  return Fragment::Context {
    diagram->potentialInputs(), // Inputs, including global variables
    diagram->potentialOutputs(), // Outputs, including global variables
    diagram->localVars() // Local variables
    };
}

QString Transition::guard_error(QString guard)
{
  return Globals::compiler->fragment_error(scan_guard(checkContext(enclosingDiagram), guard));
}

QString Transition::action_error(QString action, State *dst)
{
  QString error = Globals::compiler->fragment_error(scan_action(checkContext(enclosingDiagram), action));
  if ( ! error.isEmpty() ) return error;
  // An output modified by an action must not be assigned in the target state
  QString lhs = action.split(":=").at(0).trimmed();
  for ( const QString &valuation: dst->getAttrs() )
    if ( valuation.split("=").at(0).trimmed() == lhs )
      return "it sets output " + lhs + ", which is already assigned in the target state";
  return QString();
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
  if ( ! isInitial() ) {
    QStringList modelEvents =
        enclosingDiagram->enclosingModel()->getInputEvents()
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
