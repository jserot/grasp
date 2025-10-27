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

#include <QGraphicsPolygonItem>
#include <QGraphicsSimpleTextItem>
#include "state.h"

QT_BEGIN_NAMESPACE
class QGraphicsPolygonItem;
class QGraphicsLineItem;
class QGraphicsScene;
class QRectF;
class QGraphicsSceneMouseEvent;
class QPainterPath;
class Diagram;
QT_END_NAMESPACE

class Transition : public QGraphicsPolygonItem
{
public:
    enum { Type = UserType + 4 };

  Transition(Diagram *diagram, State *srcState, State *dstState,
               QString event, QStringList guards, QStringList actions,
               State::Location location, QGraphicsItem *parent=0);
    ~Transition();

    int type() const override { return Type; }
    //QRectF boundingRect() const override;
    QPainterPath shape() const override;
    State *getSrcState() const { return srcState; }
    State *getDstState() const { return dstState; }
    QString getEvent() const { return event; }
    QStringList getGuards() const { return guards; }
    QStringList getActions() const { return actions; }
    QString getLabel();
    void setSrcState(State *s) { srcState = s; }
    void setDstState(State *s) { dstState = s; }
    void setEvent(QString s) { event = s; }
    void setGuards(QStringList ss) { guards = ss; }
    void setActions(QStringList ss) { actions = ss; }
    void setDiagram(Diagram *diagram) { this->enclosingDiagram = diagram; }
    State::Location getLocation() const { return location; }
    bool isInitial();

    void updatePosition();

    QString toString();
    
    bool check_actions(QStringList actions);
    bool check_guards(QStringList guards);
    bool check_action(Fragment::Context ctx, QString action);
    bool check_guard(Fragment::Context ctx, QString guard);
    bool check();

    QPair<QMap<QString,QString>,QMap<QString,QString>> varsOf();
      // Returns vars/inps (resp. vars/outps) read (resp. written) by the transition
    static QColor selectedColor;
    static QColor unSelectedColor;
    static double arrowSize;

    friend QDebug operator<<(QDebug d, Transition& t);

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override;

    Response scan_guard(Fragment::Context ctx, QString guard);
    Response scan_action(Fragment::Context ctx, QString action);

private:
    Diagram *enclosingDiagram;
    State *srcState;
    State *dstState;
    QString event;
    QStringList guards;
    QStringList actions;
    QPolygonF arrowHead;
    QGraphicsSimpleTextItem *label;
    State::Location location;
};

