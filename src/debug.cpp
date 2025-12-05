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

#include <QtGlobal>
#include <QtDebug>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <stdio.h>
#include "globals.h"

#define QT_ENDL Qt::endl

void debugMessageHandler(QtMsgType type, const QMessageLogContext &, const QString & msg)
{
    if ( ! Globals::traceMode ) return;
    QString txt;
    switch  ( type ) {
      case QtDebugMsg: txt = QString("Debug: %1").arg(msg); break;
      case QtInfoMsg: txt = QString("Info: %1").arg(msg); break;
      case QtWarningMsg: txt = QString("Warning: %1").arg(msg); break;
      case QtCriticalMsg: txt = QString("Critical: %1").arg(msg); break;
      case QtFatalMsg: txt = QString("Fatal: %1").arg(msg); break;
      }
    if ( Globals::traceFile ) {
      QTextStream ts(Globals::traceFile);
      ts << "[" << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") << "]" << txt << QT_ENDL;
      }
    else
      fprintf(stderr, "%s\n", txt.toStdString().c_str());
}
