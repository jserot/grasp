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

#include "dynamicPanel.h"
#include "iov.h"
#include "rowDesc.h"

class Model;

class InpStimPanel : public DynamicPanel
{
  Q_OBJECT

public:

  InpStimPanel(QString title, QString rowPrefix);
  ~InpStimPanel();

  QStringList retrieve(); // Not used here

signals:
  void modelModified();
  
private:
  QHash<QWidget*,RowDesc*> widgetToRow; 
  QString rowPrefix;

  void updateStimChoices(RowDesc *row_desc); 

protected slots:
  void addRowFields(QHBoxLayout *row_layout, void *data);
  void deleteRowFields(QHBoxLayout *row_layout);
  void stimEdited();

private:
};
