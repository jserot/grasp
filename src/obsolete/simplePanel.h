#pragma once

#include "panel.h"

class SimplePanel : public Panel
{
  Q_OBJECT

public:
  SimplePanel(QString title);
  ~SimplePanel();

protected slots:
  void addRowFields(QHBoxLayout *row_layout);
  void retrieve();
  void rowEdited();
};
