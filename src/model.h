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

#include "automaton.h"
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
QT_END_NAMESPACE

class Model
{
public:
    explicit Model(QString name, QWidget *parent = 0);

    QString getName() const { return name; }
    void setName(QString n) { name = n; }

    //Automaton *currentAutomaton(void) { return focus; }

    void clear(void);
    void update(void);

    Iov* addIo(const QString name, const Iov::IoKind kind, const Iov::IoType type, const Stimulus stim);
    void removeIo(Iov *io);

    void addAutomaton(Automaton *automaton);
    void removeAutomaton(Automaton *automaton);

    QList<Iov*> getIos() { return ios; };
    QStringList getInputs();
    QStringList getOutputs();
    QStringList getShared();
    QStringList getSharedEvents();
    QStringList getInpEvents();
    QStringList getInpNonEvents();
    QStringList getOutpNonEvents();
    QList<Automaton*> getAutomatons() { return automatons; };

    void readFromFile(QString fname);
    void saveToFile(QString fname);

    void report_error(QString msg);
    bool check(bool withStimuli);

    void dump(); // for debug only

#ifdef USE_QGV
    void renderDot(QGVScene *scene);
#endif
    QStringList exportDots(QString basename, QStringList options);
#ifndef USE_QGV
    void exportDot(QString fname, QStringList options);
#endif
    void exportRfsm(QString fname, bool withTestbench = false);

protected:
    void export_rfsm_ios(QTextStream& os);
    QString exportSingleDot(Automaton *automaton, QString basename, QStringList options);

private:
    QString name;
    QList<Iov*> ios;
    QList<Automaton*> automatons;
    //Automaton *focus;
    const static QString automatonPrefix;
};

