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

#include <QScrollArea>

QT_BEGIN_NAMESPACE
class QImage;
class QLabel;
QT_END_NAMESPACE

class ImageViewer : public QScrollArea
{
  Q_OBJECT

public:
  ImageViewer(const QPixmap& pixmap, QWidget *parent);
  ~ImageViewer();

  void scaleImage(double scaleFactor);

  bool isFittedToWindow(void);

public slots:
  void fitToWindow(const bool& bValue);
  void normalSize();

private:
    static const double minScaleFactor;
    static const double maxScaleFactor;
    bool fittedToWindow;
    QLabel *image;

    void adjustScrollBar(QScrollBar *scrollBar, double factor);
};

