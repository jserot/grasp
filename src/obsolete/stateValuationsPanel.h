#pragma once

#include "dynamicPanel.h"

class StateValuationsPanel : public DynamicPanel
{
  Q_OBJECT

public:
  StateValuationsPanel(QString title);
  ~StateValuationsPanel();

protected slots:
  void addRowFields(QHBoxLayout *row_layout, QString &v);
  QStringList retrieve();
  void rowEdited();
};
