#pragma once

#include <QString>
#include <QProcess>

class CommandExec;

class Compiler : QObject
{
  Q_OBJECT

  // static const QString name;
  
public:
  Compiler(QString path);
  ~Compiler();

  void setPath(QString path);
  
  bool run(QString srcFile, QStringList args, QString wDir);
  QStringList getOutputs();
  QStringList getErrors();
  QStringList getOutputFiles(QString target, QString wDir, QString modelName);
private:
  QString path;
  CommandExec* executor;
};
