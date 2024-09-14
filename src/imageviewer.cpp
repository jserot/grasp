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
#include "imageviewer.h"
#include "qt_compat.h"

#include <QtGui>
#include <QPainter>


ImageViewer::ImageViewer(const QPixmap& pixmap, QWidget *parent) : QScrollArea(parent)
{
  image = new QLabel;
  image->setBackgroundRole(QPalette::Base);
  image->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
  image->setPixmap(pixmap);
  image->setScaledContents(true);
  setBackgroundRole(QPalette::Dark);
  setWidget(image);
  fittedToWindow = false;
}

void ImageViewer::scaleImage(double scaleFactor)
{
  image->resize(scaleFactor * PIXMAP_SIZE(image));
  adjustScrollBar(this->horizontalScrollBar(), scaleFactor);
  adjustScrollBar(this->verticalScrollBar(), scaleFactor);
  update();
  updateGeometry();
}

bool ImageViewer::isFittedToWindow()
{
    return fittedToWindow;
}

void ImageViewer::normalSize()
{
  image->adjustSize();
}

void ImageViewer::fitToWindow(const bool &t)
{
  setWidgetResizable(t);
  if ( !t ) normalSize();
  fittedToWindow = t;
}

void ImageViewer::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
  scrollBar->setValue(int(factor * scrollBar->value() + ((factor - 1) * scrollBar->pageStep()/2)));
}

ImageViewer::~ImageViewer()
{
}
