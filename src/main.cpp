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
#include "debug.h"

int main(int argv, char *args[])
{
    if ( Globals::traceMode ) { // TO FIX: we should be able to set traceMode when launching the app 
      Globals::traceFile = new QFile(Globals::traceFilePath);
      Globals::traceFile->open(QIODevice::WriteOnly | QIODevice::Append);
      }
    qInstallMessageHandler(debugMessageHandler); 

    QApplication app(argv, args);
    MainWindow mainWindow;
    mainWindow.setGeometry(100, 100, 900, 700);
    
    mainWindow.show();

    return app.exec();
}
