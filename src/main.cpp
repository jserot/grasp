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
    if ( Globals::traceMode ) { 
      Globals::traceFile = new QFile(AppFiles::logFile());
      //fprintf(stderr, "logFile=%s\n", AppFiles::logFile().toStdString().c_str());
      Globals::traceFile->open(QIODevice::WriteOnly);
      }
    qInstallMessageHandler(debugMessageHandler); 

    QApplication app(argv, args);
    MainWindow mainWindow;
    mainWindow.setGeometry(100, 100, 900, 700);
    
    mainWindow.show();

    return app.exec();
}
