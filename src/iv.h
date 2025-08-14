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

#include <QScrollArea>

QT_BEGIN_NAMESPACE
class QImage;
class QLabel;
class QMenu;
QT_END_NAMESPACE

class ImageViewer : public QScrollArea
{
  Q_OBJECT

public:
  ImageViewer(QString fname);
  ~ImageViewer();

  void scaleImage(double scaleFactor);
  bool isFittedToWindow(void);

public slots:
  void showContextMenu(QPoint pos);
  void zoomIn();
  void zoomOut();
  void normalSize();

protected:
    QAction *zoomInAction;
    QAction *zoomOutAction;
    QAction *normalSizeAction;
    QMenu *contextMenu;
private:
    static const double zoomInFactor;
    static const double zoomOutFactor;
    static const double minScaleFactor;
    static const double maxScaleFactor;
    double scaleFactor;
    QLabel *image;

    void adjustScrollBar(QScrollBar *scrollBar, double factor);
};

