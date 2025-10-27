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

#define QSET_FROM_LIST(type,qlist) (QSet<type> (qlist.constBegin(), qlist.constEnd()))

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

template <typename K, typename V>
QMap<K,V> map_union(QMap<K,V> m1, QMap<K,V> m2)
{
  QMap<K,V> r = m1;
  r.insert(m2);
  return r;
}

template <typename K, typename V>
QMap<K,V> operator+(QMap<K,V> m1, QMap<K,V> m2)
{
  return map_union(m1,m2);
}
