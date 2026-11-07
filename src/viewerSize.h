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

#include <QSize>
#include <QScreen>
#include <QGuiApplication>

// Size of a result viewer window (text, image) showing a content of size [content] : the content size, bounded by
// a fraction of the available screen space (scroll bars are then shown) and by a minimal size.
inline QSize boundedViewerSize(QSize content)
{
  static const double maxScreenFraction = 0.8;
  static const QSize minSize(300, 150);
  QScreen *screen = QGuiApplication::primaryScreen();
  QSize available = screen ? screen->availableGeometry().size() : QSize(1024, 768);
  QSize maxSize(int(available.width() * maxScreenFraction), int(available.height() * maxScreenFraction));
  return content.boundedTo(maxSize).expandedTo(minSize);
}
