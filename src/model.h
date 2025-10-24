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

#include <QStringListModel>
#include <QTextStream>
#include <QGraphicsScene>
#include <QList>

#include "diagram.h"
#include "iov.h"
#include "include/nlohmann_json.h"

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
class QPointF;
class QGraphicsLineItem;
class QFont;
class QColor;
QT_END_NAMESPACE

class Model
{
public:
    explicit Model();

    ~Model();

    void setName(QString n) { name = n; }
    QString getName() const { return name; }

    void clear(void);
    void update(void);

    Iov* addIo(const QString name, const Iov::IoKind kind, const Iov::IoType type, const Stimulus stim);
    void removeIo(Iov *io);

    void addDiagram(Diagram *diagram);
    void removeDiagram(Diagram *diagram);

    QList<Iov*>& getIos() { return ios; };
    QList<QPair<QString,QString>> getInputs();
    QList<QPair<QString,QString>> getOutputs();
    QList<QPair<QString,QString>> getShared();
    QStringList getInputNames();
    QStringList getOutputNames();
    QStringList getSharedNames();
    QStringList getInpEvents();
    QStringList getOutpEvents();
    QStringList getSharedEvents();
    //QStringList getInpNonEvents();
    //QStringList getOutpNonEvents();
    QList<Diagram*>& getDiagrams() { return diagrams; };

    static Model* readFromFile(QString fname);
    void saveToFile(QString fname);

    void report_error(QString msg);
    bool check(bool withStimuli);

    void dump(); // for debug only

    QStringList exportDots(QString basename, QStringList options);
    void exportDot(QString fname, QStringList options);
    void exportRfsm(QString fname, bool withTestbench = false);

protected:
    QList<QPair<QString,QString>> getIovs(Iov::IoKind kind);
    QStringList getIovNames(Iov::IoKind kind);
    QStringList getEvents(Iov::IoKind kind);
    void export_rfsm_ios(QTextStream& os);
    QString exportSingleDot(Diagram *diagram, QString basename, QStringList options);

private:
    QString name;
    QList<Iov*> ios;
    QList<Diagram*> diagrams;
    const static QString diagramPrefix;
    const static QString defaultName;
};

