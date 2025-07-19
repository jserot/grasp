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
#include <QGuiApplication>
#ifdef USE_QGV
#include "QGVScene.h"
#include "QGVNode.h"
#include "QGVEdge.h"
#endif
#include "qt_compat.h"

const QString Model::automatonPrefix = "A";

Model::Model(QString name, QWidget *parent)
{
    Q_UNUSED(parent); // Kept for future use ?
    this->name = name;
}

Iov* Model::addIo(const QString name, const Iov::IoKind kind, const Iov::IoType type, const Stimulus stim)
{
  qDebug () << "Model::addIo" << name << kind << type << stim.toString() ;
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
  for ( Automaton* a: automatons ) a->clear();
  automatons.clear();
}

void Model::update(void)
{
  for ( Automaton* a: automatons ) a->update();
}

void Model::addAutomaton(Automaton *automaton)
{
  if ( automaton->getName().isEmpty() )
    automaton->setName(automatonPrefix + QString::number(automatons.length()));
  qDebug () << "Model::addAutomaton" << automaton->getName();
  automatons.append(automaton);
}

void Model::removeAutomaton(Automaton *automaton)
{
  qDebug () << "Model::removeAutomaton" << automaton->getName();
  automatons.removeOne(automaton);
}

QStringList Model::getInputs()
{
  QStringList r;
  for ( const auto io : ios )
    if ( io->kind == Iov::IoIn ) r.append(io->name);
  return r;
}

QStringList Model::getInpEvents()
{
  QStringList r;
  for ( const auto io : ios )
    if ( io->kind == Iov::IoIn && io->type == Iov::TyEvent ) r.append(io->name);
  return r;
}

QStringList Model::getSharedEvents()
{
  QStringList r;
  for ( const auto io : ios )
    if ( io->kind == Iov::IoVar && io->type == Iov::TyEvent ) r.append(io->name);
  return r;
}

QStringList Model::getInpNonEvents()
{
  QStringList r;
  for ( const auto io : ios )
    if ( io->kind == Iov::IoIn && io->type != Iov::TyEvent ) r.append(io->name);
  return r;
}

QStringList Model::getOutputs()
{
  QStringList r;
  for ( const auto io : ios )
    if ( io->kind == Iov::IoOut ) r.append(io->name);
  return r;
}

QStringList Model::getOutpNonEvents()
{
  QStringList r;
  for ( const auto io : ios )
    if ( io->kind == Iov::IoOut && io->type != Iov::TyEvent ) r.append(io->name);
  return r;
}

QStringList Model::getShared()
{
  QStringList r;
  for ( const auto io : ios )
    if ( io->kind == Iov::IoVar ) r.append(io->name);
  return r;
}

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
  for ( Automaton* a: automatons )
    if ( ! a->check(ios) ) return false;
  return true;
}

// Reading and saving

void Model::readFromFile(QString fname)
{
    QFile file(fname);
    qDebug() << "Reading model from file" << file.fileName();
    file.open(QIODevice::ReadOnly);
    if ( file.error() != QFile::NoError ) {
      QMessageBox::warning(Globals::mainWindow, "","Cannot open file " + file.fileName());
      return;
      }
    QTextStream is(&file);
    QString txt = is.readAll();
  
    auto json = nlohmann::json::parse(txt.toStdString()); 

    // We cannot directly update the model, because an error can occur when reading the JSON file !
    // Instead, we build lists of IOs, states and transitions by parsing the JSON  file ...

    QString name = QString::fromStdString(json.at("name"));

    QList<Iov *> ios;
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

    QList<Automaton*> sub_models;
    for ( auto & json_automaton : json.at("automatons") ) {
      Automaton *a = Automaton::fromJson(json_automaton, this, Globals::mainWindow);
      qDebug() << "Read automaton from JSON";
      a->dump();
      sub_models.append(a);
      }   

    // ... and, if (and only if) parsing succeeds, we update the model.

    clear();
    this->name = name;

    foreach ( Iov* io, ios) {
      qDebug () << "Model::readFromFile: adding IO" << io->name << io->kind << io->type << io->stim.toString() ;
      this->ios.append(io);
      }
    foreach ( Automaton *a, sub_models ) {
      qDebug () << "Model::readFromFile: adding automaton" << a->getName();
      addAutomaton(a);
      }
    //update();
    qDebug() << "Done";
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

    json_top["automatons"] = nlohmann::json::array();
    for ( Automaton* a: automatons ) {
      nlohmann::json json;
      json["name"] = "main";
      a->toJson(json);
      json_top["automatons"].push_back(json);
      }

    QTextStream os(&file);
    os << QString::fromStdString(json_top.dump(2));
    file.close();
    qDebug () << "Done";
}

// DOT export

#ifdef USE_QGV
// Direct DOT rendering using QGV library (since 1.3.0)

void Model::renderDot(QGVScene *dotScene)
{
  dotScene->setGraphAttribute("rankdir", "UD");
  dotScene->setGraphAttribute("nodesep", "0.55");
  dotScene->setGraphAttribute("ranksep", "0.95");
  dotScene->setGraphAttribute("fontsize", "14");
  dotScene->setGraphAttribute("mindist", "1.0");
  dotScene->setNodeAttribute("shape", "circle");
  dotScene->setNodeAttribute("style", "solid");

  QMap<QString,QGVNode*> nodes;

  for ( const auto a: automatons ) 
    a->renderDot(dotScene, nodes);
}
#endif


QString Model::exportSingleDot(Automaton *automaton, QString basename, QStringList options)
{
  //QString fname = basename + "_" + automaton->getName() + ".dot";
  Q_UNUSED(basename);
  QString fname = automaton->getName() + ".dot";
  QFile file(fname);
  file.open(QIODevice::WriteOnly | QIODevice::Text);
  if ( file.error() != QFile::NoError ) {
    QMessageBox::warning(Globals::mainWindow, "","Cannot open file " + file.fileName());
    return QString();
    }
  QTextStream os(&file);
  os << "digraph " << automaton->getName() << " {\n";
  os << "layout = dot\n";
  os << "rankdir = UD\n";
  os << "size = \"8.5,11\"\n";
  os << "center = 1\n";
  os << "nodesep = \"0.350000\"\n";
  os << "ranksep = \"0.400000\"\n";
  os << "fontsize = 14\n";
  os << "mindist=1.0\n";
  bool withIoDesc = ! options.contains("-dot_no_captions");
  if ( ! ios.isEmpty() && withIoDesc ) 
    os << "_ios [label=\"" << Iov::stringOfList(ios) << "\", shape=rect, style=solid]\n";
  automaton->exportDot(os);
  os << "}\n";
  file.close();
  return fname;
}

QStringList Model::exportDots(QString basename, QStringList options)
{
  QStringList fnames;
  for ( Automaton* automaton: automatons) {
    QString fname = exportSingleDot(automaton,basename,options);
    if ( ! fname.isEmpty() ) fnames.append(fname);
    }
  return fnames;
}

#ifndef USE_QGV
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
  bool withIoDesc = ! options.contains("-dot_no_captions");
  if ( withIoDesc ) 
    os << "_ios [label=\"" << Iov::stringOfList(ios) << "\", shape=rect, style=solid]\n";
  foreach ( Automaton *a, automatons) {
	os << "subgraph cluster_" << a->getName() << " {\n";
    os << "label = " << a->getName() << "\n";
    a->exportDot(os);
    os << "}\n";
    }
  os << "}\n";
  file.close();
}
#endif

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
  // FSM models (automatons)
  for ( const auto automaton : automatons ) {
    automaton->exportRfsmModel(os,ios);
    os << "\n";
    }
  // IOs with stimuli
  export_rfsm_ios(os);
  os << "\n";
  // FSM instances
  if ( withTestbench ) {
      os << "\n\n";
      for ( const auto automaton : automatons )
        automaton->exportRfsmInstance(os,ios);
      }
  file.close();
}

void Model::dump() // For debug only
{
  qDebug() << "Model " << name;
  qDebug() << "  ios =";
  foreach ( Iov* io, ios )
    qDebug() << "    " <<  io->toString();
  qDebug() << "  automatons =";
  for ( const auto a: automatons ) 
    a->dump();
}
