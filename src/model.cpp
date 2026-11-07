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
#include <memory>
#include "iov.h"
#include "include/nlohmann_json.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QPair>
#include <QGuiApplication>
#include <optional>

const QString Model::defaultName = "main";
const QString Model::diagramPrefix = "A";

Model::Model()
{
  name = Model::defaultName;
  ginps.clear();
  goutps.clear();
  gvars.clear();
}

Iov* Model::addInput(const QString name, const IoType type, const Stimulus stim)
{
  Iov* io = new Input(name, type, stim);
  ginps.append(io);
  return io;
}

Iov* Model::addOutput(const QString name, const IoType type)
{
  Iov* io = new Output(name, type);
  goutps.append(io);
  return io;
}

Iov* Model::addGvar(const QString name, const IoType type)
{
  Iov* io = new Variable(name, type);
  gvars.append(io);
  return io;
}

void Model::removeIo(Iov *io)
{
  switch ( io->kind() ) {
  case IoKind::IoIn:
    removeInput(io);
    break;
  case IoKind::IoOut:
    removeOutput(io);
    break;
  case IoKind::IoVar:
    removeGvar(io);
    break;
  }
  delete io;
}

// Iov* Model::addIo(const QString name, const IoKind kind, const IoType type)
// {
//   Iov* io;
//   switch ( kind ) {
//   case IoKind::IoIn:
//     io = new Input(name, type, Stimulus::None);
//     ginps.append(io);
//     return io;
//   case IoKind::IoOut:
//     io = new Output(name, type);
//     goutps.append(io);
//     return io;
//   case IoKind::IoVar:
//     io = new Variable(name, type);
//     gvars.append(io);
//     return io;
//   }
// }

void Model::clear(void)
{
  name = "";
  qDeleteAll(ginps);
  ginps.clear();
  qDeleteAll(goutps);
  goutps.clear();
  qDeleteAll(gvars);
  gvars.clear();
  qDeleteAll(diagrams);
  diagrams.clear();
}

void Model::update(void)
{
  for ( Diagram* a: diagrams ) a->update();
}

void Model::addDiagram(Diagram *diagram)
{
  if ( diagram->getName().isEmpty() )
    diagram->setName(freshDiagramName(diagramPrefix));
  qDebug () << "Model::addDiagram" << diagram->getName();
  diagrams.append(diagram);
}

bool Model::isDiagramNameUsed(const QString &name, const Diagram *except) const
{
  for ( const Diagram *d : diagrams )
    if ( d != except && d->getName() == name ) return true;
  return false;
}

QString Model::freshDiagramName(const QString &prefix) const
{
  int n = 1;
  while ( isDiagramNameUsed(prefix + QString::number(n)) ) n++;
  return prefix + QString::number(n);
}

void Model::removeDiagram(Diagram *diagram)
{
  qDebug () << "Model::removeDiagram" << diagram->getName();
  diagrams.removeOne(diagram);
}

static QMap<QString,QString> build_io_map(QList<Iov*> ios)
{
  QMap<QString,QString> r;
  for ( const auto io : ios )
    r.insert(io->name,toString(io->type));
  return r;
}

static QStringList build_io_list(QList<Iov*> ios, bool event_typed = false)
{
  QStringList r;
  for ( const auto io : ios )
    if ( ! event_typed || io->type == IoType::TyEvent )
      r.append(io->name);
  return r;
}

QMap<QString,QString> Model::getInputs() { return build_io_map(ginps); }
QMap<QString,QString> Model::getOutputs() { return build_io_map(goutps); }
QMap<QString,QString> Model::getShared() { return build_io_map(gvars); }

QStringList Model::getInputNames() { return build_io_list(ginps); }
QStringList Model::getOutputNames() { return build_io_list(goutps); }
QStringList Model::getSharedNames() { return build_io_list(gvars); }

QStringList Model::getInputEvents() { return build_io_list(ginps, true); }
QStringList Model::getOutputEvents() { return build_io_list(goutps, true); }
QStringList Model::getSharedEvents() { return build_io_list(gvars, true); }

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
  QStringList inpEvents = getInputEvents();
  if ( inpEvents.length() == 0 ) {
    report_error("There should be at least one input with type event");
    return false;
    }
  if ( withStimuli ) {
    for( Iov* io : ginps ) {
      if ( static_cast<Input*>(io)->stim.kind == Stimulus::None )  {
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
  // Deleted (with the diagrams already read) if reading fails
  std::unique_ptr<Model> model = std::make_unique<Model>();
  try {
    QFile file(fname);
    qDebug() << "Reading model from file" << file.fileName();
    (void)file.open(QIODevice::ReadOnly);
    if ( file.error() != QFile::NoError ) {
      QMessageBox::warning(Globals::mainWindow, "","Cannot open file " + file.fileName());
      return nullptr;
      }
    QTextStream is(&file);
    QString txt = is.readAll();
  
    auto json = nlohmann::json::parse(txt.toStdString()); 

    QString name = QString::fromStdString(json.at("name"));

    for ( const auto & json_io : json.at("ios") ) {
      QString name = QString::fromStdString(json_io.at("name"));
      std::optional<IoKind> kind = ioKindFromString(QString::fromStdString(json_io.at("kind")));
      if ( ! kind ) throw std::invalid_argument("Model::readFromFile:: invalid kind");
      std::optional<IoType> type = ioTypeFromString(QString::fromStdString(json_io.at("type")));
      if ( ! type ) throw std::invalid_argument("Model::readFromFile:: invalid type");
      QString stim = QString::fromStdString(json_io.at("stim"));
      switch ( kind.value() ) {
      case IoKind::IoIn: 
        model->addInput(name, type.value(), Stimulus(stim));
        break;
      case IoKind::IoOut: 
        model->addOutput(name, type.value());
        break;
      case IoKind::IoVar: 
        model->addGvar(name, type.value());
        break;
      }
      }
    QList<Diagram*>& diagrams = model->getDiagrams();
    for ( auto & json_diagram : json.at("diagrams") ) {
      Diagram *a = Diagram::fromJson(json_diagram, model.get(), Globals::mainWindow);
      qDebug() << "Read diagram from JSON";
      a->dump();
      diagrams.append(a);
      }   

    model->setName(name);

    qDebug() << "Done";
    return model.release();
    }
  catch(const std::exception& e) {
    QMessageBox::warning(Globals::mainWindow, "Error", "Error when reading file " + fname + ": " + QString(e.what()));
    return nullptr;
  }
}

bool Model::saveToFile(QString fname)
{
    QFile file(fname);
    qDebug() << "Saving model to file" << file.fileName();
    (void)file.open(QIODevice::WriteOnly | QIODevice::Text);
    if ( file.error() != QFile::NoError ) {
      QMessageBox::warning(Globals::mainWindow, "","Cannot open file " + file.fileName());
      return false;
      }

    nlohmann::json json_top;

    json_top["name"] = name.toStdString();

    json_top["ios"] = nlohmann::json::array();
    int cnt = 1;
    QList<Iov*> ios = ginps + goutps + gvars;
    for ( const Iov* io: ios ) {
      nlohmann::json json;
      if ( io->name == "" ) {
        QMessageBox::warning(Globals::mainWindow, "Warning", "IO #" + QString::number(cnt) + " has no name. Ignoring it");
        continue;
        }
      json["name"] = io->name.toStdString(); 
      json["kind"] = toString(io->kind()).toStdString(); 
      json["type"] = toString(io->type).toStdString(); 
      json["stim"] = io->kind() == IoKind::IoIn ? static_cast<const Input*>(io)->stim.toString().toStdString() : "None";  // For backward compatibility
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
    os.flush();
    file.close();
    if ( os.status() != QTextStream::Ok || file.error() != QFile::NoError ) {
      QMessageBox::warning(Globals::mainWindow, "","Error when writing file " + file.fileName());
      return false;
      }
    qDebug () << "Done";
    return true;
}

// DOT export

QString Model::exportSingleDot(Diagram *diagram, QString basename, QStringList options)
{
  QString fname = basename + "_" + diagram->getName() + ".dot";
  QFile file(fname);
  qDebug() << "Opening file" << file.fileName() << "for writing";
  (void)file.open(QIODevice::WriteOnly | QIODevice::Text);
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
  QList<Iov*> ios = ginps + goutps + gvars;
  if ( ! ios.isEmpty() && withIoDesc ) 
    os << "_ios [label=\"" << toString(ios) << "\", shape=rect, style=solid]\n";
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
  (void)file.open(QIODevice::WriteOnly | QIODevice::Text);
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
  QList<Iov*> ios = ginps + goutps + gvars;
  if ( withIoDesc ) 
    os << "_ios [label=\"" << toString(ios) << "\", shape=rect, style=solid]\n";
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
    QString ss;
    for ( const auto io : ginps ) {
      ss = export_rfsm_stim(static_cast<Input*>(io)->stim);
      if ( ss != "" )  {
            os << "input " << io->name << " : " << toString(io->type) << " = " << ss;;
            os << "\n";
            }
      else {
            QMessageBox::warning(Globals::mainWindow, "","No stimulus for input " + io->name);
            return;
            }
      }
    for ( const auto io : goutps ) {
      os << "output " << io->name << " : " << toString(io->type);
      os << "\n";
      }
    for ( const auto io : gvars ) {
        os << "shared " << io->name << " : " << toString(io->type);
        os << "\n";
      }
}

void Model::exportRfsm(QString fname, bool withTestbench)
{
  QFile file(fname);
  (void)file.open(QIODevice::WriteOnly | QIODevice::Text);
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
  QList<Iov*> ios = ginps + goutps + gvars;
  qDebug() << "  ios =";
  foreach ( Iov* io, ios )
    qDebug() << "    " <<  io->toString();
  qDebug() << "  diagrams =";
  for ( const auto a: diagrams ) 
    a->dump();
}

Model::~Model()
{
  foreach ( Iov* io, ginps ) delete io;
  foreach ( Iov* io, goutps ) delete io;
  foreach ( Iov* io, gvars ) delete io;
  for ( const auto a: diagrams ) delete a;
}
  
