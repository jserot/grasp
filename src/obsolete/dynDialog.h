#pragma once

#include <QDialog>
#include <QMap>

class QPushButton;
class QBoxLayout;
class QVBoxLayout;
class QHBoxLayout;

class DynDialog : public QDialog
{
  Q_OBJECT

public:
  explicit DynDialog(QString title, QStringList *values, QWidget *parent = Q_NULLPTR);
  ~DynDialog();

protected:
  QVBoxLayout *layout;
  QPushButton *add_button;
  QPushButton *clear_button;
  QPushButton *cancel_button;
  QPushButton *accept_button;
  QHash<QPushButton*, QBoxLayout*> mButtonToLayoutMap;
  QStringList *values;

  void delete_row(QLayout *row_layout);

public slots:
  void clear();
  void addRow(QString value);

signals:
    
protected slots:
  void addNewRow();
  void deleteRow();
  void accept();
  void cancel();
  // The following methods will be defined in the derived classes 
  virtual void addRowFields(QHBoxLayout *row_layout, QString& v) = 0; // Fields shown _before_ the [Delete] button on the row
  virtual void rowEdited() = 0; 
  virtual QStringList retrieve() = 0;
};
