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

#include "stimulus.h"
#include <QtDebug>

Stimulus::Stimulus(Kind kind, QList<int> params)
{
  this->kind = kind;
  switch ( kind ) {
  case None:
    break;
  case Periodic:
    desc.periodic = Periodic_stim(params.at(0), params.at(1), params.at(2));
    break;
  case Sporadic:
    desc.sporadic = Sporadic_stim(params);
    break;
  case ValueChanges:
    QList<QPair<int,int>> vcs;
    while ( params.length() >= 2 ) {
      int t = params.takeFirst();
      int v = params.takeFirst();
      vcs.append(QPair<int,int>(t,v));
      }
    desc.valueChanges = ValueChanges_stim(vcs);
    break;
  }
}

Stimulus::Stimulus(QString txt)
{
  QStringList l = txt.split(" ");
  Q_ASSERT(l.length() >= 1 );
  if ( l.at(0).trimmed() == "None" ) {
    kind = None;
    }
  else if ( l.at(0).trimmed() == "Periodic" ) {
    Q_ASSERT(l.length() == 4 );
    kind = Periodic;
    desc.periodic.period = l.at(1).trimmed().toInt();
    desc.periodic.start_time = l.at(2).trimmed().toInt();
    desc.periodic.end_time = l.at(3).trimmed().toInt();
    }
  else if ( l.at(0).trimmed() == "Sporadic" ) {
    kind = Sporadic;
    desc.sporadic.dates.clear();
    for ( int i=1; i<l.length(); i++ )
      desc.sporadic.dates.append(l.at(i).trimmed().toInt());
    }
  else if ( l.at(0).trimmed() == "ValueChanges" ) {
    Q_ASSERT((l.length()-1) % 2 == 0 );
    kind = ValueChanges;
    desc.valueChanges.vcs.clear();
    for ( int i=1; i<l.length(); i+=2 )
      desc.valueChanges.vcs.append(QPair<int,int>(l.at(i).trimmed().toInt(),l.at(i+1).trimmed().toInt()));
    }
  else
    Q_ASSERT(false);
}

QString Stimulus::toString() const
{
  QString r;
  switch ( kind ) {
  case None: return "None";
  case Periodic:
    r = "Periodic";
    r += " " + QString::number(desc.periodic.period);
    r += " " + QString::number(desc.periodic.start_time);
    r += " " + QString::number(desc.periodic.end_time);
    break;
  case Sporadic:
    r = "Sporadic";
    for ( const auto t: desc.sporadic.dates ) 
      r += " " + QString::number(t);
    break;
  case ValueChanges: 
    r = "ValueChanges";
    for ( const QPair<int,int> &vc: desc.valueChanges.vcs ) {
      r += " " + QString::number(vc.first);
      r += " " + QString::number(vc.second);
      }
    break;
  }
  return r;
}

