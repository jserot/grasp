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

#include "dynamicPanel.h"

class TransitionGuards : public DynamicPanel
{
  Q_OBJECT

public:
  TransitionGuards(QString title, QStringList& guards);
  ~TransitionGuards();

  QStringList retrieve();

protected slots:
  void addRowFields(QHBoxLayout *row_layout, void *row_data);
  void deleteRowFields(QHBoxLayout *row_layout);
};
