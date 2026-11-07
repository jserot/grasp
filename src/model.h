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
    Q_DISABLE_COPY(Model) // Owns its IOs and diagrams

public:
    explicit Model();

    ~Model();

    void setName(QString n) { name = n; }
    QString getName() const { return name; }

    void clear(void);
    void update(void);

  // Iov* addIo(const QString name, const IoKind kind, const IoType type);
    Iov* addInput(const QString name, const IoType type, const Stimulus stim = Stimulus(Stimulus::None));
    Iov* addOutput(const QString name, const IoType type);
    Iov* addGvar(const QString name, const IoType type);
    void removeIo(Iov *io);

    void addDiagram(Diagram *diagram);
    bool isDiagramNameUsed(const QString &name, const Diagram *except = nullptr) const;
    QString freshDiagramName(const QString &prefix = diagramPrefix) const; // First name "<prefix><n>" not used by a diagram
    void removeDiagram(Diagram *diagram);

    QList<Iov*>& inputs() { return ginps; };
    QList<Iov*>& outputs() { return goutps; };
    QList<Iov*>& globalVars() { return gvars; };

    QMap<QString,QString> getInputs();
    QMap<QString,QString> getOutputs();
    QMap<QString,QString> getShared();
    QStringList getInputNames();
    QStringList getOutputNames();
    QStringList getSharedNames();
    QStringList getInputEvents();
    QStringList getOutputEvents();
    QStringList getSharedEvents();
    QList<Diagram*>& getDiagrams() { return diagrams; };

    static Model* readFromFile(QString fname);
    bool saveToFile(QString fname); // Returns false (after warning the user) if the file could not be written

    void report_error(QString msg);
    bool check(bool withStimuli);

    void dump(); // for debug only

    QStringList exportDots(QString basename, QStringList options);
    void exportDot(QString fname, QStringList options);
    bool exportRfsm(QString fname, bool withTestbench = false); // Returns false (and writes nothing) on error

protected:
    // QMap<QString,QString> getIovs(IoKind kind);
    // QStringList getIovNames(IoKind kind);
    // QStringList getEvents(IoKind kind);
    void removeInput(Iov *inp) { ginps.removeOne(inp); }   // These do _not_ delete the IO (see removeIo)
    void removeOutput(Iov *outp) { goutps.removeOne(outp); }
    void removeGvar(Iov *var) { gvars.removeOne(var); }
    bool export_rfsm_ios(QTextStream& os);
    QString exportSingleDot(Diagram *diagram, QString basename, QStringList options);

private:
    QString name;
    QList<Iov*> ginps;
    QList<Iov*> goutps;
    QList<Iov*> gvars;
    QList<Diagram*> diagrams;
    const static QString diagramPrefix;
    const static QString defaultName;
};

