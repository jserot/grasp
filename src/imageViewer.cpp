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
#include "imageViewer.h"

#include <QtGui>
#include <QPainter>

const double ImageViewer::zoomInFactor = 1.25;
const double ImageViewer::zoomOutFactor = 0.8;
const double ImageViewer::minScaleFactor = 0.2;
const double ImageViewer::maxScaleFactor = 2.0;

ImageViewer::ImageViewer(QString fname, QWidget *parent) : QScrollArea(parent)
{
  image = new QLabel;
  image->setBackgroundRole(QPalette::Base);
  image->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
  QImage img(fname);
  image->setPixmap(QPixmap::fromImage(img));
  image->setScaledContents(true);
  setBackgroundRole(QPalette::Dark);
  setWidget(image);
  setWindowFlag(Qt::Window, true);  // Required to make the window toplevel
  setAttribute(::Qt::WA_DeleteOnClose);
  setContextMenuPolicy(Qt::CustomContextMenu);
  QFileInfo f(fname);
  setWindowTitle(f.fileName());
  scaleFactor = 1.0;

  zoomInAction = new QAction(tr("Zoom In"), this);
  zoomInAction->setShortcut(tr("Ctrl++"));
  zoomInAction->setEnabled(true);
  connect(zoomInAction, SIGNAL(triggered()), this, SLOT(zoomIn()));

  zoomOutAction = new QAction(tr("Zoom Out"), this);
  zoomOutAction->setShortcut(tr("Ctrl+-"));
  zoomOutAction->setEnabled(true);
  connect(zoomOutAction, SIGNAL(triggered()), this, SLOT(zoomOut()));

  normalSizeAction = new QAction(tr("Normal size (100%)"), this);
  normalSizeAction->setEnabled(true);
  connect(normalSizeAction, SIGNAL(triggered()), this, SLOT(normalSize()));

  contextMenu = new QMenu(tr("Context menu"), this);
  contextMenu->addAction(zoomInAction);
  contextMenu->addAction(zoomOutAction);
  contextMenu->addAction(normalSizeAction);

  connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
}

void ImageViewer::showContextMenu(QPoint pos)
{
  qDebug() << "ImageViewer::contextMenu()";
  Q_ASSERT(contextMenu);
  contextMenu->exec(mapToGlobal(pos));
}

void ImageViewer::zoomIn()
{
  scaleImage(zoomInFactor);
}

void ImageViewer::zoomOut()
{
  scaleImage(zoomOutFactor);
}

void ImageViewer::normalSize()
{
  image->adjustSize();
  scaleFactor = 1.0;
}

void ImageViewer::scaleImage(double factor)
{
  scaleFactor *= factor;
  image->resize(scaleFactor * image->pixmap().size());
  adjustScrollBar(this->horizontalScrollBar(), factor);
  adjustScrollBar(this->verticalScrollBar(), factor);
  zoomInAction->setEnabled(scaleFactor < maxScaleFactor);
  zoomOutAction->setEnabled(scaleFactor > minScaleFactor);
  update();
  updateGeometry();
}

void ImageViewer::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
  scrollBar->setValue(int(factor * scrollBar->value() + ((factor - 1) * scrollBar->pageStep()/2)));
}

ImageViewer::~ImageViewer()
{
  qDebug() << "ImageViewer::delete";
  delete zoomInAction;
  delete zoomOutAction;
  delete normalSizeAction;
  delete contextMenu;
}
