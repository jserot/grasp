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

#include "mainwindow.h"

#include <QApplication>
#include "appFiles.h"
#include "debug.h"

int main(int argv, char *args[])
{
    // QCoreApplication::setOrganizationName("GraspSoftware");
    QCoreApplication::setApplicationName("grasp");
    Globals::traceFile = new QFile(AppFiles::logFile());
    //fprintf(stderr, "logFile=%s\n", AppFiles::logFile().toStdString().c_str());
    if ( ! Globals::traceFile->open(QIODevice::WriteOnly) ) {
      fprintf(stderr, "Cannot open log file %s. Logging to stderr\n", AppFiles::logFile().toStdString().c_str());
      delete Globals::traceFile;
      Globals::traceFile = nullptr;
      }
    qInstallMessageHandler(debugMessageHandler); 

    QApplication app(argv, args);
    int result;
    {
      MainWindow mainWindow;
      mainWindow.setGeometry(100, 100, 900, 700);
      mainWindow.show();
      result = app.exec();
    } // The main window (and the model) are deleted here, possibly emitting trace messages

    if ( Globals::traceFile ) {
      Globals::traceFile->close();
      delete Globals::traceFile;
      Globals::traceFile = nullptr; // Later messages, if any, go to stderr
      }
    return result;
}
