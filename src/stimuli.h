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

#include <QDialog>
 #include "iov.h"

class QVBoxLayout;
class QHBoxLayout;
class QComboBox;
class QFrame;
class QPushButton;
class QSpacerItem;
class QComboBox;

class Stimuli: public QDialog {
    Q_OBJECT

public:
  explicit Stimuli(Stimulus::Kind kind, Iov* inp, QWidget *parent = Q_NULLPTR);
  ~Stimuli();

private:
  const int maxTime = 1000;

  QHash<QPushButton*, QHBoxLayout*> mButtonToLayoutMap;
  QList<QHBoxLayout*> rows;
  Stimulus::Kind selectedKind;
  Iov *selectedInp;

private:
  QWidget *centralWidget;
  QVBoxLayout* formLayout;
  QFrame *form;
  QVBoxLayout *verticalLayout;
  QVBoxLayout *verticalLayout_2;
  QSpacerItem *verticalSpacer;
  QFrame *line;

protected:
  void addPeriodicRow(QString name, int val, int step, int lo, int hi);

private slots:
  void clearForm();
  void _addSporadicRow(int t);
  void addSporadicRow();
  void _addValueChangesRow(QPair<int,int> vc);
  void addValueChangesRow();
  void deleteDynamicRow();
  void acceptChanges();
  void cancelChanges();
};

