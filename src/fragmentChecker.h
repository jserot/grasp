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

#include <QString>

class Compiler;
class Diagram;
class QWidget;

class FragmentChecker
{
public:
  FragmentChecker(Compiler *compiler, Diagram *diagram, QWidget *parent);
  bool check_state_valuation(QString valuation);
  bool check_guard(QString guard);
  bool check_action(QString action);
  QStringList getErrors();
private:
  QWidget *parent;
  Compiler *compiler;
  Diagram *diagram;
  bool check(QString kind, QString frag);
};
