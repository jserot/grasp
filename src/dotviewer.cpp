/***********************************************************************/
/*                                                                     */
/*       This file is part of the RFSM Light software package          */
/*                                                                     */
/*  Copyright (c) 2019-present, Jocelyn SEROT (jocelyn.serot@uca.fr)   */
/*                       All rights reserved.                          */
/*                                                                     */
/*    This source code is licensed under the license found in the      */
/*      LICENSE file in the root directory of this source tree.        */
/*                                                                     */
/***********************************************************************/


#include <QtWidgets>
#include "dotviewer.h"
#include "QGVScene.h"
#include "model.h"

const int DotViewer::minCanvasWidth = 200;
const int DotViewer::minCanvasHeight = 400;

DotViewer::DotViewer(Model *model, int width, int height, QWidget *parent) : QGraphicsView(parent)
{
  scene = new QGVScene("DOT", this);
  scene->setSceneRect(QRectF(0, 0, width, height));
  setScene(scene);
  setMinimumWidth(minCanvasWidth);
  setMinimumHeight(minCanvasHeight);
  model->renderDot(scene);
  scene->applyLayout();
  //view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
  ensureVisible(scene->itemsBoundingRect());
}

DotViewer::~DotViewer()
{
  if ( scene ) delete scene;
}
