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

#include <QStringListModel>
#include <QTextStream>
#include <QGraphicsScene>

#include "state.h"
#include "iov.h"
#include "include/nlohmann_json.h"

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
class QPointF;
class QGraphicsLineItem;
class QFont;
class QColor;
#ifdef USE_QGV
class QGVScene;
#endif
class Model;
QT_END_NAMESPACE

class Automaton : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit Automaton(Model *enclosingModel, QWidget *parent = 0);
    explicit Automaton(Model *enclosingModel, QString name, QList<Iov*> vars, QList<State*> states, QList<Transition*> transitions, QWidget *parent);
    ~Automaton();

    QString getName() const { return name; }
    void setName(QString n) { name = n; }
    QGraphicsView* getView() const { return view; }
    void setView(QGraphicsView* v) { view = v; }
    Model *enclosingModel() { return model; }

    Automaton *duplicate();

    void clear(void);

    Iov* addVar(const QString name, const Iov::IoType type);
    void removeVar(Iov *io);
    QList<Iov*> getVars();
    QStringList getVarNames();

    QList<State*> states();
    QList<Transition*> transitions();
    State* initState();
    Transition* initTransition();

    State* getState(QString id);
    bool hasPseudoState();

    void removeState(State *state);
    void removeTransition(Transition *transition);

    void save(nlohmann::json json_res);

    bool check(QList<Iov*>& global_ios);

    void dump(); // for debug only

#ifdef USE_QGV
    void renderDot(QGVScene *scene, QMap<QString,QGVNode*> nodes);
#endif
    void exportDot(QTextStream &os);
    void exportRfsmModel(QTextStream& os, QList<Iov*>& global_ios);
    void exportRfsmInstance(QTextStream& os, QList<Iov*>& global_ios);

    static Automaton* fromJson(nlohmann::json& json, Model *model, QWidget *parent);
    void toJson(nlohmann::json& json);

signals:
    void mouseEnter(void);
    void mouseLeave(void);
    void modelModified(void);

protected:
    QString qual_id(QString id);
    bool event(QEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    void editItem(QGraphicsItem *item);
//  void contextMenuEvent(QGraphicsSceneContextMenuEvent *contextMenuEvent) override;


    void addState(State *state);
    void addTransition(Transition *transition);
    void editState(State *state);
    void editTransition(Transition *transition);
    void check_state(State* s);
    bool check_transition(Transition *t, QList<Iov*>& global_ios);
    void report_error(QString msg);

    void export_rfsm_model(QTextStream& os);
    void export_rfsm_testbench(QTextStream& os);
    
    static int stateCounter;
    static QString statePrefix;

private:
    bool isItemChange(int type);
    State* addState(QPointF pos, QString id, QStringList attrs);
    State* addPseudoState(QPointF pos);
    Transition* addTransition(State* srcState, State* dstState,
                              QString event, QStringList guards, QStringList actions,
                              State::Location location);

    QString name;
    Model *model; 
    QGraphicsView *view; 
    QList<Iov*> vars; // Local variables (IOs and global vars are part of the enclosing model)
    
    QGraphicsLineItem *line;  // Line being drawn
    State *startState;

    QWidget *parent;
    //QWidget *mainWindow;

    static QColor lineColor;
    static QColor boxColor;
    static const int canvas_width;
    static const int canvas_height;
};

