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

#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QString>

// Inline validation of dialog fields: instead of a message box shown when the dialog is accepted, an invalid
// field gets a red frame and the error is displayed, as soon as it is typed, by a label placed under the field.

static const char *const fieldErrorColor = "#e0443e"; // Readable on both light and dark backgrounds

// Label displaying the error of a field (hidden when the field is valid)
inline QLabel* newFieldErrorLabel()
{
  QLabel *label = new QLabel();
  label->setStyleSheet(QString("color: %1;").arg(fieldErrorColor));
  label->setWordWrap(true);
  label->hide();
  return label;
}

// Shows [error] for [field] in [label], or resets both if [error] is empty. [field] is a line edit or a widget
// containing one (a spin box, for instance), whose line edit gets the red frame
inline void showFieldError(QWidget *field, QLabel *label, const QString &error)
{
  if ( error.isEmpty() ) {
    field->setStyleSheet(QString()); // Back to the native look
    label->clear();
    label->setMinimumHeight(0);
    label->hide();
    }
  else {
    field->setStyleSheet(QString("QLineEdit { border: 2px solid %1; border-radius: 3px; padding: 1px 2px; }")
                         .arg(fieldErrorColor));
    label->setText(error);
    label->show();
    // The layouts do not give a word-wrapped label the height needed by its text at its actual width (the message
    // was then truncated) : once the label is placed, this height is set as its minimum height, which enlarges the
    // window if needed
    label->setMinimumHeight(0);
    if ( QWidget *window = label->window(); window && window->layout() ) window->layout()->activate();
    label->setMinimumHeight(label->heightForWidth(label->width()));
    }
}
