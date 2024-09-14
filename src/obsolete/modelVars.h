#pragma once

#include "dynamicPanel.h"
#include "rowDesc.h"

class Model;
class Iov;
class QRegularExpressionValidator;
class QLineEdit;
class QComboBox;
class RowDesc;

class ModelVars : public DynamicPanel
{
  Q_OBJECT

public:
  ModelVars(QString title, Model *model, QRegularExpressionValidator *name_validator);
  ~ModelVars();

  QStringList retrieve(); // Not used here

private:
  Model *model;
  QRegularExpressionValidator *name_validator;
  QHash<QWidget*,RowDesc*> widgetToRow; 

  void updateTypeChoices(RowDesc *row_desc);

signals:
  void modelModified();

protected slots:
  void addRowFields(QHBoxLayout *row_layout, QString &v);
  void deleteRowFields(QHBoxLayout *row_layout);
  void nameChanged();
  void nameEdited();
  void typeEdited();
};
