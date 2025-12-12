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

#include <QString>
#include <QApplication>
#include <QStandardPaths>
#include <QDir>
#include "globals.h"

class AppFiles {
 public:
  static QString appDir() {
    QString dir = QApplication::applicationDirPath();
#ifdef Q_OS_LINUX
    dir.replace("bin","share/grasp");
#endif
    return dir;
  }

  static QString optionsSpecFile() {
    // return ":/config/options_spec.txt";
    return appDir() + "/options_spec.txt"; 
  }
    
  static QString iniFile() {
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir().mkpath(dir);
    QString iniPath = dir + "/grasp.ini";
    if ( ! QFile::exists(iniPath) ) {
      QString defaultIni = QCoreApplication::applicationDirPath() + "/grasp.ini";
      if (QFile::exists(defaultIni)) {
        qDebug() << "Copying" << defaultIni << "file to" << iniPath;
        QFile::copy(defaultIni, iniPath);
        QFile::setPermissions(iniPath,
                              QFileDevice::ReadOwner  | QFileDevice::WriteOwner |
                              QFileDevice::ReadUser   | QFileDevice::WriteUser  |
                              QFileDevice::ReadGroup  | QFileDevice::ReadOther);
      }
    }
    return iniPath;
    // QString dir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    // QDir().mkpath(dir);
    // return dir + "/grasp.ini";
    // return appDir() + "/grasp.ini";
  }

static QString logFile() {
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);
    return dir + "/grasp.log";
  }
};
