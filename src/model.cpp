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
#include "include/nlohmann_json.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QPair>
#include <QGuiApplication>

const QString Model::defaultName = "main";
const QString Model::diagramPrefix = "A";

Model::Model()
{
  name = Model::defaultName;
  ios.clear();
}

Iov* Model::addIo(const QString name, const Iov::IoKind kind, const Iov::IoType type, const Stimulus stim)
{
  Iov *io = new Iov(name, kind, type, stim);
  ios.append(io);
  return io;
}

void Model::removeIo(Iov *io)
{
  ios.removeOne(io);
}

void Model::clear(void)
{
  name = "";
  ios.clear();
  for ( Diagram* a: diagrams ) a->clear();
  diagrams.clear();
}

void Model::update(void)
{
  for ( Diagram* a: diagrams ) a->update();
}

void Model::addDiagram(Diagram *diagram)
{
  if ( diagram->getName().isEmpty() )
    diagram->setName(diagramPrefix + QString::number(diagrams.length()));
  qDebug () << "Model::addDiagram" << diagram->getName();
  diagrams.append(diagram);
}

void Model::removeDiagram(Diagram *diagram)
{
  qDebug () << "Model::removeDiagram" << diagram->getName();
  diagrams.removeOne(diagram);
}

QMap<QString,QString> Model::getIovs(Iov::IoKind kind)
{
  QMap<QString,QString> r;
  for ( const auto io : ios )
    if ( io->kind == kind ) r.insert(io->name,Iov::stringOfType(io->type));
  return r;
}

QStringList Model::getIovNames(Iov::IoKind kind)
{
  QStringList r;
  for ( const auto io : ios )
    if ( io->kind == kind ) r.append(io->name);
  return r;
}

QStringList Model::getEvents(Iov::IoKind kind)
{
  QStringList r;
  for ( const auto io : ios )
    if ( io->kind == kind && io->type == Iov::TyEvent ) r.append(io->name);
  return r;
}

QMap<QString,QString> Model::getInputs() { return getIovs(Iov::IoIn); }
QMap<QString,QString> Model::getOutputs() { return getIovs(Iov::IoOut); }
QMap<QString,QString> Model::getShared() { return getIovs(Iov::IoVar); }
QStringList Model::getInputNames() { return getIovNames(Iov::IoIn); }
QStringList Model::getOutputNames() { return getIovNames(Iov::IoOut); }
QStringList Model::getSharedNames() { return getIovNames(Iov::IoVar); }
QStringList Model::getInpEvents() { return getEvents(Iov::IoIn); }
QStringList Model::getOutpEvents() { return getEvents(Iov::IoOut); }
QStringList Model::getSharedEvents() { return getEvents(Iov::IoVar); }

// Basic model checking

void Model::report_error(QString msg)
{
  QMessageBox::warning(Globals::mainWindow, "", msg);
}

bool Model::check(bool withStimuli)
{
  if ( name.isEmpty() ) {
    report_error("No name specified for model");
    return false;
    }
  QStringList inpEvents = getInpEvents();
  if ( inpEvents.length() == 0 ) {
    report_error("There should be at least one input with type event");
    return false;
    }
  if ( withStimuli ) {
    for( Iov* io : ios ) {
      if ( io->kind == Iov::IoIn &&  io->stim.kind == Stimulus::None )  {
        report_error("No stimulus for input " + io->name);
        return false;
        }  
      }
    }
  for ( Diagram* a: diagrams )
    if ( ! a->check() ) return false; 
  return true;
}

// Reading and saving

Model* Model::readFromFile(QString fname)
{
  Model *model = new Model();
  try {
    QFile file(fname);
    qDebug() << "Reading model from file" << file.fileName();
    file.open(QIODevice::ReadOnly);
    if ( file.error() != QFile::NoError ) {
      QMessageBox::warning(Globals::mainWindow, "","Cannot open file " + file.fileName());
      return NULL;
      }
    QTextStream is(&file);
    QString txt = is.readAll();
  
    auto json = nlohmann::json::parse(txt.toStdString()); 

    QString name = QString::fromStdString(json.at("name"));

    QList<Iov*>& ios = model->getIos();
    for ( const auto & json_io : json.at("ios") ) {
      std::string name = json_io.at("name");
      std::string kind = json_io.at("kind");
      std::string type = json_io.at("type");
      std::string stim = json_io.at("stim");
      Iov *io = new Iov(QString::fromStdString(name),
                        Iov::ioKindOfString(QString::fromStdString(kind)),
                        Iov::ioTypeOfString(QString::fromStdString(type)),
                        Stimulus(QString::fromStdString(stim)));
      ios.append(io);
      }

    QList<Diagram*>& diagrams = model->getDiagrams();
    for ( auto & json_diagram : json.at("diagrams") ) {
      Diagram *a = Diagram::fromJson(json_diagram, model, Globals::mainWindow);
      qDebug() << "Read diagram from JSON";
      a->dump();
      diagrams.append(a);
      }   

    model->setName(name);

    qDebug() << "Done";
    return model;
    }
  catch(const std::exception& e) {
    QMessageBox::warning(Globals::mainWindow, "Error", "Error when reading file " + fname + ": " + QString(e.what()));
    delete model;
    return NULL;
  }
}

void Model::saveToFile(QString fname)
{
    QFile file(fname);
    qDebug() << "Saving model to file" << file.fileName();
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    if ( file.error() != QFile::NoError ) {
      QMessageBox::warning(Globals::mainWindow, "","Cannot open file " + file.fileName());
      return;
      }

    nlohmann::json json_top;

    json_top["name"] = name.toStdString();

    json_top["ios"] = nlohmann::json::array();
    int cnt = 1;
    for ( const Iov* io: ios ) {
      nlohmann::json json;
      if ( io->name == "" ) {
        QMessageBox::warning(Globals::mainWindow, "Warning", "IO #" + QString::number(cnt) + " has no name. Ignoring it");
        continue;
        }
      json["name"] = io->name.toStdString(); 
      json["kind"] = Iov::stringOfKind(io->kind).toStdString(); 
      json["type"] = Iov::stringOfType(io->type).toStdString(); 
      json["stim"] = io->stim.toString().toStdString(); 
      json_top["ios"].push_back(json);
      cnt++;
      }

    json_top["diagrams"] = nlohmann::json::array();
    for ( Diagram* a: diagrams ) {
      nlohmann::json json;
      json["name"] = "main";
      a->toJson(json);
      json_top["diagrams"].push_back(json);
      }

    QTextStream os(&file);
    os << QString::fromStdString(json_top.dump(2));
    file.close();
    qDebug () << "Done";
}

// DOT export

QString Model::exportSingleDot(Diagram *diagram, QString basename, QStringList options)
{
  QString fname = basename + "_" + diagram->getName() + ".dot";
  QFile file(fname);
  qDebug() << "Opening file" << file.fileName() << "for writing";
  file.open(QIODevice::WriteOnly | QIODevice::Text);
  if ( file.error() != QFile::NoError ) {
    QMessageBox::warning(Globals::mainWindow, "","Cannot open file " + file.fileName());
    return QString();
    }
  QTextStream os(&file);
  os << "digraph " << diagram->getName() << " {\n";
  os << "layout = dot\n";
  os << "rankdir = UD\n";
  os << "size = \"8.5,11\"\n";
  os << "center = 1\n";
  os << "nodesep = \"0.350000\"\n";
  os << "ranksep = \"0.400000\"\n";
  os << "fontsize = 14\n";
  os << "mindist=1.0\n";
  bool withIoDesc = options.contains("-dot_captions");
  if ( ! ios.isEmpty() && withIoDesc ) 
    os << "_ios [label=\"" << Iov::stringOfList(ios,false) << "\", shape=rect, style=solid]\n";
  diagram->exportDot(os);
  os << "}\n";
  file.close();
  return fname;
}

QStringList Model::exportDots(QString basename, QStringList options)
{
  QStringList fnames;
  for ( Diagram* diagram: diagrams) {
    QString fname = exportSingleDot(diagram,basename,options);
    if ( ! fname.isEmpty() ) fnames.append(fname);
    }
  return fnames;
}

void Model::exportDot(QString fname, QStringList options)
{
  QFile file(fname);
  file.open(QIODevice::WriteOnly | QIODevice::Text);
  if ( file.error() != QFile::NoError ) {
    QMessageBox::warning(Globals::mainWindow, "","Cannot open file " + file.fileName());
    return;
  }
  QTextStream os(&file);

  QString name = this->name.isEmpty() ? "main" : this->name;
  os << "digraph " << name << " {\n";
  os << "layout = dot\n";
  os << "rankdir = UD\n";
  os << "size = \"8.5,11\"\n";
  os << "center = 1\n";
  os << "nodesep = \"0.350000\"\n";
  os << "ranksep = \"0.400000\"\n";
  os << "fontsize = 14\n";
  os << "mindist=1.0\n";
  bool withIoDesc = options.contains("-dot_captions");
  if ( withIoDesc ) 
    os << "_ios [label=\"" << Iov::stringOfList(ios,false) << "\", shape=rect, style=solid]\n";
  foreach ( Diagram *a, diagrams) {
	os << "subgraph cluster_" << a->getName() << " {\n";
    os << "label = " << a->getName() << "\n";
    a->exportDot(os);
    os << "}\n";
    }
  os << "}\n";
  file.close();
}

// RFSM export

QString export_rfsm_stim(Stimulus &st) 
// We cannot use Stimulus::toString() since the syntax is different :(
{
  QString r;
  switch ( st.kind ) {
  case Stimulus::None: return "";
  case Stimulus::Periodic:
    r = "periodic(";
    r +=      QString::number(st.desc.periodic.period);
    r += "," + QString::number(st.desc.periodic.start_time);
    r += "," + QString::number(st.desc.periodic.end_time) + ")";
    break;
  case Stimulus::Sporadic:
    r = "sporadic(";
    for ( const auto t: st.desc.sporadic.dates ) {
      r += " " + QString::number(t);
      r += ",";
      }
    if ( r.endsWith(",") ) r.chop(1);
    r += ")";
    break;
  case Stimulus::ValueChanges: 
    r = "value_changes(";
    for ( const QPair<int,int> &vc: st.desc.valueChanges.vcs ) {
      r += QString::number(vc.first) + ":" + QString::number(vc.second);
      r += ",";
      }
    if ( r.endsWith(",") ) r.chop(1);
    r += ")";
    break;
  }
  return r;
}
void Model::export_rfsm_ios(QTextStream& os)
{
    // QList<Iov*> gios;
    QString ss;
    for ( const auto io : ios ) {
      switch ( io->kind ) {
        case Iov::IoIn:
          ss = export_rfsm_stim(io->stim);
          if ( ss != "" )  {
            os << "input " << io->name << " : " << Iov::stringOfType(io->type) << " = " << ss;;
            // gios.append(io);
            os << "\n";
            }
          else {
            QMessageBox::warning(Globals::mainWindow, "","No stimulus for input " + io->name);
            return;
            }
          break;
      case Iov::IoOut:
        os << "output " << io->name << " : " << Iov::stringOfType(io->type);
        // gios.append(io);
        os << "\n";
        break;
      case Iov::IoVar:
        os << "shared " << io->name << " : " << Iov::stringOfType(io->type);
        // gios.append(io);
        os << "\n";
        break;
      }
    }
}

void Model::exportRfsm(QString fname, bool withTestbench)
{
  QFile file(fname);
  file.open(QIODevice::WriteOnly | QIODevice::Text);
  if ( file.error() != QFile::NoError ) {
    QMessageBox::warning(Globals::mainWindow, "","Cannot open file " + file.fileName());
    return;
  }
  QTextStream os(&file);
  // FSM models (diagrams)
  for ( const auto diagram : diagrams ) {
    diagram->exportRfsmModel(os);
    os << "\n";
    }
  if ( withTestbench ) {
    // IOs with stimuli
    export_rfsm_ios(os);
    os << "\n";
    // FSM instances
    if ( withTestbench ) {
      os << "\n\n";
      for ( const auto diagram : diagrams )
        diagram->exportRfsmInstance(os);
      }
    }
  file.close();
}

void Model::dump() // For debug only
{
  qDebug() << "Model " << name;
  qDebug() << "  ios =";
  foreach ( Iov* io, ios )
    qDebug() << "    " <<  io->toString();
  qDebug() << "  diagrams =";
  for ( const auto a: diagrams ) 
    a->dump();
}

Model::~Model()
{
  foreach ( Iov* io, ios )
    delete io;
  for ( const auto a: diagrams ) 
    delete a;
}
  
