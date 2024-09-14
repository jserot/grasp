#pragma once

#include "dynDialog.h"

class StateValuationsDialog : public DynDialog
{
  Q_OBJECT

public:
  StateValuationsDialog(QString title, QStringList *values, QWidget *parent);
  ~StateValuationsDialog();

protected slots:
  void addRowFields(QHBoxLayout *row_layout, QString &v);
  QStringList retrieve();
  void rowEdited();
};
