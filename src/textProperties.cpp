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

#include "textProperties.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFontComboBox>
#include <QComboBox>
#include <QtDebug>

#include "textviewer.h"

TextProperties::TextProperties(TextViewer *viewer, QWidget *parent)
  : QDialog(parent)
{
  setWindowTitle("Code font");

  this->viewer = viewer;

  QVBoxLayout *layout = new QVBoxLayout();

  QHBoxLayout *row_layout1 = new QHBoxLayout();
  QLabel* font_name_label = new QLabel("Font");
  row_layout1->addWidget(font_name_label);
  fontCombo = new QFontComboBox();
  QFont font = viewer->getFont();
  fontCombo->setCurrentFont(font);
  // fontCombo->setFontFilters(QFontComboBox::ScalableFonts);
  connect(fontCombo, &QFontComboBox::currentFontChanged, this, &TextProperties::fontChanged);
  row_layout1->addWidget(fontCombo);
  layout->addLayout(row_layout1);

  QHBoxLayout *row_layout2 = new QHBoxLayout();
  QLabel* font_size_label = new QLabel("Size");
  row_layout2->addWidget(font_size_label);
  fontSizeCombo = new QComboBox;
  fontSizeCombo->setEditable(true);
  for (int i = 8; i < 24; i = i + 2)
    fontSizeCombo->addItem(QString().setNum(i));
  QIntValidator *validator = new QIntValidator(2, 64, this);
  fontSizeCombo->setValidator(validator);
  fontSizeCombo->setCurrentText(QString().setNum(font.pointSize()));
  row_layout2->addWidget(fontSizeCombo);
  connect(fontSizeCombo, &QComboBox::currentTextChanged, this, &TextProperties::fontChanged);
  layout->addLayout(row_layout2);
  setLayout(layout);

  // close_button = new QPushButton("Close");
  // connect(close_button, &QPushButton::clicked, this, &TextProperties::close);
  // connect(accept_button, &QPushButton::clicked, this, &TextProperties::accept);
}

void TextProperties::fontChanged()
{
  QFont font = fontCombo->currentFont();
  qDebug() << "TextProperties::fontChanged: " << font;
  font.setPointSize(fontSizeCombo->currentText().toInt());
  viewer->setFont(font);
  QDialog::done(Accepted);
}

void TextProperties::close()
{
  QDialog::done(Accepted);
}

TextProperties::~TextProperties()
{
}
