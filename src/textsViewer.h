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

#include <QTabWidget>
#include "textViewer.h"

class TextsViewer : public QTabWidget
{
public:
  TextsViewer(QStringList fnames, QWidget *parent = nullptr);
  ~TextsViewer();
protected:
private:
};

