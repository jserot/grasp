#pragma once

#include "dynamicPanel.h"
#include "iov.h"
#include "rowDesc.h"

class Model;
class Iov;
class QRegularExpressionValidator;
class QLineEdit;
class QComboBox;

class ModelInps : public DynamicPanel
{
  Q_OBJECT

public:
  ModelInps(QString title, Model *model, QRegularExpressionValidator *name_validator);
  ~ModelInps();

  QStringList retrieve(); // Not used here

signals:
  void modelModified();
  
private:
  Model *model;
  QRegularExpressionValidator *name_validator;
  QHash<QWidget*,RowDesc*> widgetToRow; 

  void updateTypeChoices(RowDesc *row_desc);
  void updateStimChoices(RowDesc *row_desc);

protected slots:
  void addRowFields(QHBoxLayout *row_layout, QString &v);
  void deleteRowFields(QHBoxLayout *row_layout);
  void nameChanged();
  void nameEdited();
  void typeEdited();
  void stimEdited();
};
