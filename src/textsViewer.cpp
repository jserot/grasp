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


#include <QtWidgets>
#include <QTabWidget>
#include "textsViewer.h"

TextsViewer::TextsViewer(QString title, QStringList fnames, QWidget *parent) : QTabWidget(parent)
{
  foreach ( QString fname, fnames ) {
    QFile file(fname);
    QFileInfo f(file);
    qDebug() << "TextsViewer::adding tab for file" << fname;
    TextViewer *viewer = new TextViewer(fname);
    addTab(viewer, f.fileName());
    }
  setMovable(true);
  setAttribute(::Qt::WA_DeleteOnClose);
  setWindowTitle(title);
  setWindowFlag(Qt::Window, true);  // Required to make the window toplevel
}

TextsViewer::~TextsViewer()
{
  qDebug() << "TextsViewer::delete";
}
