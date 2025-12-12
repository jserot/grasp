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
    
  static QString configFile() {
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir().mkpath(dir);
    QString configPath = dir + "/grasp.conf";
    if ( ! QFile::exists(configPath) ) {
      QString defaultConfPath = QCoreApplication::applicationDirPath() + "/grasp.conf";
      if (QFile::exists(defaultConfPath)) {
        qDebug() << "Copying" << defaultConfPath << "file to" << configPath;
        QFile::copy(defaultConfPath, configPath);
        QFile::setPermissions(configPath,
                              QFileDevice::ReadOwner  | QFileDevice::WriteOwner |
                              QFileDevice::ReadUser   | QFileDevice::WriteUser  |
                              QFileDevice::ReadGroup  | QFileDevice::ReadOther);
      }
    }
    return configPath;
  }

static QString logFile() {
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);
    return dir + "/grasp.log";
  }
};
