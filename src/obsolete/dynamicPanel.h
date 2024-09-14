#pragma once

#include <QGroupBox>
#include <QMap>

class QPushButton;
class QBoxLayout;
class QVBoxLayout;
class QHBoxLayout;

class DynamicPanel : public QGroupBox
{
  Q_OBJECT

protected:
  QVBoxLayout *layout;
  QPushButton *add_button;
  QPushButton *clear_button;
  QPushButton *done_button;
  QHash<QPushButton*, QBoxLayout*> mButtonToLayoutMap;

  void delete_row(QLayout *row_layout);

public:
  DynamicPanel(QString title);
  ~DynamicPanel();

public slots:
  void clear();
  void addRow(QString value);
  void init(QStringList& values);

signals:
  void editingDone(QStringList& values); 
    
protected slots:
  void addNewRow();
  void deleteRow();
  void done();
  // Panel-specific methods
  virtual void addRowFields(QHBoxLayout *row_layout, QString& v) = 0; // Fields shown _before_ the [Delete] button on the row
  virtual void rowEdited() = 0; 
  virtual QStringList retrieve() = 0;
};
