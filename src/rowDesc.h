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

// Common to model{Inps,Outps,Var}

class Iov;
class QHBoxLayout;
class QLineEdit;
class QComboBox;
class QPushButton;

class RowDesc {
public:
  RowDesc( Iov* io, QHBoxLayout *row_layout, QLineEdit* name_selector, QComboBox* type_selector) :
    io(io), row_layout(row_layout), name_selector(name_selector), type_selector(type_selector), committedName(io->name)
    {}
  ~RowDesc() {}
    
  Iov* io;
  QHBoxLayout *row_layout;
  QLineEdit* name_selector;
  QComboBox* type_selector;
  QString committedName; // Name of the IO when last validated (and used by the transitions it triggers)
  QPushButton* stim_button = nullptr; // Tells whether a stimulus is attached and opens the stimuli editor (model inputs only)
};
