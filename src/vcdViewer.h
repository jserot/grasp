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

#include <QAbstractScrollArea>
#include <QList>
#include <QString>
#include <QWidget>

class QLabel;

// Built-in viewer of the simulation results (.vcd files written by rfsmc), replacing an external viewer such as
// gtkwave (which can still be used, see the -vcd_external_viewer option)

// Contents of a VCD file
struct VcdSignal
{
  enum Kind { Event, Bool, Int, State, Real };
  struct Change { qint64 time; QString value; };
  QString id;   // VCD identifier
  QString name; // Hierarchical name ("A0.state", for ex.)
  Kind kind;
  int width;
  QList<Change> changes; // Sorted by time (events : one change per occurrence)
  QString valueAt(qint64 time) const; // Empty if the signal has no value at [time] (events : except at their date)
};

struct VcdData
{
  QList<VcdSignal> tracks;
  qint64 endTime = 0; // Date of the last change, plus a margin (to show the last values)
  qint64 timeScale = 1;  // Factor of the time unit
  QString timeUnit = ""; // ns, us, ...
  bool read(const QString &fname, QString *error);
  void reorder(const QString &gtkwFile); // Uses the signal order of a gtkwave save file, if it exists
  // Writes the signal order in a gtkwave save file (only the signal list of an existing file is replaced)
  bool saveLayout(const QString &gtkwFile, const QString &vcdFile, QString *error) const;
};

// The chronograms, with the names and the values (at the cursor) of the signals on the left and a time axis on top
class VcdWaveView : public QAbstractScrollArea
{
  Q_OBJECT

public:
  VcdWaveView(VcdData *data, QWidget *parent = nullptr);
  void zoomIn();
  void zoomOut();
  void zoomFit();

signals:
  void cursorMoved(qint64 time); // -1 if there's no cursor
  void layoutChanged();          // The signal order has been changed

protected:
  void paintEvent(QPaintEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;
  void wheelEvent(QWheelEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void contextMenuEvent(QContextMenuEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;
  bool viewportEvent(QEvent *event) override;

private:
  VcdData *data;
  double scale = 1.0; // Pixels per time unit
  bool fitted = true; // Showing the whole simulation (kept when the window is resized)
  qint64 cursor = -1;
  int selectedRow = -1; // Signal selected by a click on its name (to be moved)
  int dragRow = -1;     // Signal being dragged by its name
  int dropIndex = -1;   // Where it will be inserted (0 : before the first signal, ...)
  int namesWidth = 0;
  int valuesWidth = 0;
  static const int rowHeight = 30;
  static const int axisHeight = 26;
  static const int margin = 8;

  int leftWidth() const { return namesWidth + valuesWidth; }
  int traceWidth() const;
  double minScale() const;
  double maxScale() const { return 200.0; }
  int timeToX(qint64 time) const;   // In viewport coordinates
  qint64 xToTime(int x) const;
  void setScale(double newScale, int anchorX);
  void updateScrollBars();
  void updateValuesWidth();
  void setCursorAt(int x);
  int rowAt(int y) const; // -1 if there's no signal at [y]
  int insertionIndexAt(int y) const;
  void moveSignal(int from, int to); // [to] : new index of the signal
  QString timeText(qint64 time) const;
  void drawAxis(QPainter &painter);
  void drawSignal(QPainter &painter, const VcdSignal &signal, int top);
};

// The viewer window
class VcdViewer : public QWidget
{
  Q_OBJECT

public:
  // Returns nullptr (after displaying the error) if [fname] cannot be read
  static VcdViewer* open(const QString &fname, QWidget *parent);
  ~VcdViewer();

private:
  VcdViewer(VcdData *data, const QString &fname, QWidget *parent);
  void saveLayout();
  VcdData *data;
  QString vcdFile;
  VcdWaveView *view;
  QLabel *cursorLabel;
};
