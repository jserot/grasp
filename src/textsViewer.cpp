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
#include "viewerSize.h"

TextsViewer::TextsViewer(QString title, QStringList fnames, QWidget *parent) : QTabWidget(parent)
{
  QSize content(0, 0);
  foreach ( QString fname, fnames ) {
    QFile file(fname);
    QFileInfo f(file);
    qDebug() << "TextsViewer::adding tab for file" << fname;
    TextViewer *viewer = new TextViewer(fname);
    addTab(viewer, f.fileName());
    content = content.expandedTo(viewer->contentSize());
    }
  // The window is sized after the largest file, within limits (scroll bars are shown beyond)
  resize(boundedViewerSize(content + QSize(0, tabBar()->sizeHint().height())));
  setMovable(true);
  setAttribute(::Qt::WA_DeleteOnClose);
  setWindowTitle(title);
  setWindowFlag(Qt::Window, true);  // Required to make the window toplevel
}

TextsViewer::~TextsViewer()
{
  qDebug() << "TextsViewer::delete";
}
