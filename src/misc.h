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

#include <QList>
#include <QSet>
#include "qt_compat.h"

template <typename T>
QSet<T> QListToQSet(const QList<T>& qlist)
{
  return QSET_FROM_LIST(T,qlist);
}

template <typename T>
QList<T> remove_duplicates(const QList<T>& l)
{
  return QListToQSet(l).values();
}
