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

#include <QList>

struct Periodic_stim
{
  Periodic_stim() : period(0), start_time(0), end_time(0) { };
  Periodic_stim(int p, int t1, int t2) : period(p), start_time(t1), end_time(t2) { };
  int period;
  int start_time;
  int end_time;
};

struct Sporadic_stim
{
  Sporadic_stim() : dates(QList<int>()) { };
  Sporadic_stim(QList<int> &values) : dates(values) { };
  QList<int> dates;
};

struct ValueChanges_stim
{
  ValueChanges_stim() : vcs(QList<QPair<int,int>>()) { };
  ValueChanges_stim(QList<QPair<int,int>> &values) : vcs(values) { };
  QList<QPair<int,int>> vcs;
};

class Stimulus
{
public:
  enum Kind { None=0, Periodic, Sporadic, ValueChanges };
  Kind kind;
  struct Desc {  // TO FIX: this should really be a _union_ 
      Periodic_stim periodic;
      Sporadic_stim sporadic;  
      ValueChanges_stim valueChanges;
      };
  Desc desc;

  Stimulus(Kind kind, QList<int> params=QList<int>());
  Stimulus(QString s);
  ~Stimulus() {};

  QString toString() const ;
};

