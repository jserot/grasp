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

#pragma once

#include <QGraphicsView>

class Model;
class QGVScene;

class DotViewer : public QGraphicsView
{
  Q_OBJECT

  const static int minCanvasWidth;
  const static int minCanvasHeight;

public:
  DotViewer(Model *model, int width, int height, QWidget *parent);
  ~DotViewer();

private:
  QGVScene *scene;
};

