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

#include "vcdViewer.h"

#include <QFile>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QMenu>
#include <QTextStream>
#include <QMessageBox>
#include <QNativeGestureEvent>
#include <QPainter>
#include <QPainterPath>
#include <QRegularExpression>
#include <QScrollBar>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWheelEvent>
#include <QtDebug>
#include <algorithm>
#include <cmath>

#include "viewerSize.h"

// Colors of the traces, readable on both light and dark backgrounds
static const QColor eventColor("#e07b24");
static const QColor boolColor("#3aa35c");
static const QColor valueColor("#3b82d0");

static QString gtkwSignalName(QString line); // See below

// VCD data

QString VcdSignal::valueAt(qint64 time) const
{
  auto after = std::upper_bound(changes.begin(), changes.end(), time,
                                [](qint64 t, const Change &c) { return t < c.time; });
  if ( after == changes.begin() ) return QString();
  const Change &c = *(after - 1);
  if ( kind == Event ) return c.time == time ? QString("event") : QString();
  return c.value;
}

// Value of an integer signal, written in binary (with [width] bits, the leading zeros being possibly omitted)
static QString intValue(const QString &bits, int width)
{
  if ( bits.contains(QRegularExpression("[^01]")) ) return "x";
  bool ok;
  qulonglong v = bits.toULongLong(&ok, 2);
  if ( ! ok ) return "?";
  if ( width > 1 && width < 64 && bits.length() == width && bits[0] == '1' ) // Negative (two's complement)
    return QString::number(qlonglong(v) - (qlonglong(1) << width));
  if ( width == 64 ) return QString::number(qlonglong(v));
  return QString::number(v);
}

bool VcdData::read(const QString &fname, QString *error)
{
  QFile file(fname);
  if ( ! file.open(QIODevice::ReadOnly | QIODevice::Text) ) {
    *error = "Cannot open file " + fname;
    return false;
    }
  // A VCD file is a sequence of tokens separated by white spaces
  const QStringList tokens = QString::fromUtf8(file.readAll()).split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
  QHash<QString, int> index; // Signal index of each VCD identifier
  QStringList scopes;
  qint64 time = 0;
  bool inHeader = true;
  int i = 0;
  auto skipToEnd = [&]() { while ( i < tokens.size() && tokens[i] != "$end" ) i++; i++; };
  auto addChange = [&](const QString &id, const QString &value) {
    if ( ! index.contains(id) ) return;
    VcdSignal &s = tracks[index[id]];
    if ( ! s.changes.isEmpty() && s.changes.last().time == time ) { // Several values at the same date : the last one
      if ( s.kind != VcdSignal::Event ) s.changes.last().value = value;
      }
    else if ( s.kind == VcdSignal::Event || s.changes.isEmpty() || s.changes.last().value != value )
      s.changes.append({ time, value });
    };
  while ( i < tokens.size() ) {
    const QString &tok = tokens[i];
    if ( inHeader ) {
      if ( tok == "$timescale" ) {
        QString ts;
        for ( i++; i < tokens.size() && tokens[i] != "$end"; i++ ) ts += tokens[i];
        i++;
        QRegularExpressionMatch m = QRegularExpression("^(\\d+)\\s*([a-z]*)$").match(ts);
        if ( m.hasMatch() ) { timeScale = m.captured(1).toLongLong(); timeUnit = m.captured(2); }
        }
      else if ( tok == "$scope" ) {
        if ( i + 2 < tokens.size() ) scopes << tokens[i+2];
        skipToEnd();
        }
      else if ( tok == "$upscope" ) {
        if ( ! scopes.isEmpty() ) scopes.removeLast();
        skipToEnd();
        }
      else if ( tok == "$var" ) { // $var <type> <size> <id> <name> [<range>] $end
        if ( i + 4 >= tokens.size() ) break;
        VcdSignal s;
        QString type = tokens[i+1];
        s.width = tokens[i+2].toInt();
        s.id = tokens[i+3];
        QStringList path = scopes.mid(1); // The top-level scope is omitted
        path << tokens[i+4];
        s.name = path.join(".");
        if ( type == "event" ) s.kind = VcdSignal::Event;
        else if ( type == "real" ) s.kind = VcdSignal::Real; // Becomes State if its values are strings (see below)
        else s.kind = s.width == 1 ? VcdSignal::Bool : VcdSignal::Int;
        if ( ! index.contains(s.id) ) { // Aliases (same identifier) are shown once
          index.insert(s.id, tracks.size());
          tracks.append(s);
          }
        skipToEnd();
        }
      else if ( tok == "$enddefinitions" ) {
        skipToEnd();
        inHeader = false;
        }
      else if ( tok.startsWith("$") ) skipToEnd(); // $date, $comment, $version, ...
      else i++;
      continue;
      }
    // Value changes
    if ( tok.startsWith("#") ) {
      time = tok.mid(1).toLongLong();
      endTime = std::max(endTime, time);
      i++;
      }
    else if ( tok.startsWith("$") ) i++; // $dumpvars, $end, ...
    else if ( tok[0] == 'b' || tok[0] == 'B' || tok[0] == 'r' || tok[0] == 'R' || tok[0] == 's' || tok[0] == 'S' ) {
      if ( i + 1 >= tokens.size() ) break;
      QString id = tokens[i+1];
      QString v = tok.mid(1);
      if ( index.contains(id) ) {
        VcdSignal &s = tracks[index[id]];
        if ( tok[0] == 's' || tok[0] == 'S' ) s.kind = VcdSignal::State;
        else if ( tok[0] == 'b' || tok[0] == 'B' ) v = s.kind == VcdSignal::Bool ? v.right(1) : intValue(v, s.width);
        }
      addChange(id, v);
      i += 2;
      }
    else { // Scalar : <value><id>
      addChange(tok.mid(1), tok.left(1));
      i++;
      }
    }
  if ( inHeader ) {
    *error = "Invalid VCD file " + fname + " (no $enddefinitions)";
    return false;
    }
  endTime += std::max<qint64>(1, endTime / 20); // The last values are shown on a short duration
  // Default order : top-level signals (events first) then the signals of each instance (state first), in the
  // reverse order of their declarations (which is the order of the model declarations, with rfsmc)
  std::reverse(tracks.begin(), tracks.end());
  auto rank = [](const VcdSignal &s) {
    int dot = s.name.indexOf('.');
    if ( dot < 0 ) return s.kind == VcdSignal::Event ? 0 : 1;
    return s.name.mid(dot + 1) == "state" ? 2 : 3;
    };
  auto instance = [](const VcdSignal &s) { int dot = s.name.indexOf('.'); return dot < 0 ? QString() : s.name.left(dot); };
  QStringList instances;
  for ( const VcdSignal &s : tracks ) if ( ! instances.contains(instance(s)) ) instances << instance(s);
  std::stable_sort(tracks.begin(), tracks.end(), [&](const VcdSignal &a, const VcdSignal &b) {
    int ia = instances.indexOf(instance(a)), ib = instances.indexOf(instance(b));
    bool ta = instance(a).isEmpty(), tb = instance(b).isEmpty();
    if ( ta != tb ) return ta;
    if ( ia != ib ) return ia < ib;
    return rank(a) < rank(b);
    });
  return true;
}

void VcdData::reorder(const QString &gtkwFile)
{
  QFile file(gtkwFile);
  if ( ! file.open(QIODevice::ReadOnly | QIODevice::Text) ) return;
  // Signal lines look like "top.\gensig.k[7:0]" (the other lines start with '[', '*', '@' or '-')
  QStringList names;
  while ( ! file.atEnd() ) {
    QString name = gtkwSignalName(QString::fromUtf8(file.readLine()));
    if ( ! name.isEmpty() ) names << name;
    }
  QList<VcdSignal> ordered;
  for ( const QString &name : names )
    for ( int k = 0; k < tracks.size(); k++ )
      if ( tracks[k].name == name ) { ordered.append(tracks.takeAt(k)); break; }
  tracks = ordered + tracks; // The signals not listed in the file come last
}

// Name of a signal in a gtkwave save file ("top.h", "top.\\A0.state", "top.n1[15:0]", ...)
static QString gtkwName(const VcdSignal &s)
{
  QString name = "top." + (s.name.contains('.') ? "\\" + s.name : s.name);
  if ( s.kind == VcdSignal::Int ) name += "[" + QString::number(s.width - 1) + ":0]";
  return name;
}

// Name of the signal on a line of a gtkwave save file, or an empty string if the line does not describe a signal
static QString gtkwSignalName(QString line)
{
  line = line.trimmed();
  if ( line.isEmpty() || QString("[*@-").contains(line[0]) ) return QString();
  line.remove('\\');
  line.remove(QRegularExpression("\\[[^\\]]*\\]$"));
  if ( line.startsWith("top.") ) line = line.mid(4);
  return line;
}

static QString gtkwFileOf(const QString &vcdFile)
{
  QFileInfo fi(vcdFile);
  return fi.path() + "/" + fi.completeBaseName() + ".gtkw";
}

bool VcdData::saveLayout(const QString &gtkwFile, const QString &vcdFile, QString *error) const
{
  // An existing file is split in three parts : the lines before the signal list, the signal list (with the
  // display flags, written on '@' lines, which are kept) and the lines after it
  QStringList before, after;
  QHash<QString, QPair<QString, QString>> known; // Flags and line of each signal of the existing file
  QFile in(gtkwFile);
  if ( in.open(QIODevice::ReadOnly | QIODevice::Text) ) {
    int part = 0;
    QString flags;
    while ( ! in.atEnd() ) {
      QString line = QString::fromUtf8(in.readLine());
      line.chop(line.endsWith("\n") ? 1 : 0);
      QString name = gtkwSignalName(line);
      bool inList = ! name.isEmpty() || line.startsWith("@") || line.startsWith("-");
      if ( part == 0 && inList ) part = 1;
      else if ( part == 1 && ! inList ) part = 2;
      if ( part == 0 ) before << line;
      else if ( part == 2 ) after << line;
      else if ( line.startsWith("@") ) flags = line;
      else if ( ! name.isEmpty() ) known.insert(name, { flags, line });
      }
    in.close();
    }
  else
    before << "[*]" << "[*] Written by Grasp" << "[*]" << "[dumpfile] \"" + QFileInfo(vcdFile).absoluteFilePath() + "\""
           << "[timestart] 0";
  QFile out(gtkwFile);
  if ( ! out.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate) ) {
    *error = "Cannot write file " + gtkwFile;
    return false;
    }
  QTextStream os(&out);
  for ( const QString &line : before ) os << line << "\n";
  QString current;
  for ( const VcdSignal &s : tracks ) {
    QString flags = s.kind == VcdSignal::Int ? "@24" : "@28"; // Decimal / binary (and strings)
    QString line = gtkwName(s);
    if ( known.contains(s.name) ) {
      if ( ! known[s.name].first.isEmpty() ) flags = known[s.name].first;
      line = known[s.name].second;
      }
    if ( flags != current ) { os << flags << "\n"; current = flags; }
    os << line << "\n";
    }
  for ( const QString &line : after ) os << line << "\n";
  return true;
}

// Wave view

VcdWaveView::VcdWaveView(VcdData *data, QWidget *parent) : QAbstractScrollArea(parent), data(data)
{
  setFocusPolicy(Qt::StrongFocus);
  viewport()->setMouseTracking(false);
  QFontMetrics fm(font());
  namesWidth = fm.horizontalAdvance("Signal") + 2 * margin;
  for ( const VcdSignal &s : data->tracks ) namesWidth = std::max(namesWidth, fm.horizontalAdvance(s.name) + 2 * margin);
  updateValuesWidth();
  horizontalScrollBar()->setSingleStep(20);
  verticalScrollBar()->setSingleStep(rowHeight);
}

void VcdWaveView::updateValuesWidth()
{
  QFontMetrics fm(font());
  int w = fm.horizontalAdvance("Value");
  for ( const VcdSignal &s : data->tracks )
    for ( const VcdSignal::Change &c : s.changes ) w = std::max(w, fm.horizontalAdvance(c.value));
  valuesWidth = std::min(w, 160) + 2 * margin;
}

int VcdWaveView::traceWidth() const
{
  return std::max(1, viewport()->width() - leftWidth() - 2 * margin);
}

double VcdWaveView::minScale() const
{
  return double(traceWidth()) / double(std::max<qint64>(1, data->endTime));
}

int VcdWaveView::timeToX(qint64 time) const
{
  return int(std::lround(leftWidth() + margin + time * scale - horizontalScrollBar()->value()));
}

qint64 VcdWaveView::xToTime(int x) const
{
  double t = (x - leftWidth() - margin + horizontalScrollBar()->value()) / scale;
  return std::clamp<qint64>(std::llround(t), 0, data->endTime);
}

QString VcdWaveView::timeText(qint64 time) const
{
  return QString::number(time * data->timeScale) + (data->timeUnit.isEmpty() ? "" : " " + data->timeUnit);
}

void VcdWaveView::updateScrollBars()
{
  int contentWidth = int(std::ceil(data->endTime * scale)) + 2 * margin;
  int visibleWidth = std::max(1, viewport()->width() - leftWidth());
  horizontalScrollBar()->setPageStep(visibleWidth);
  horizontalScrollBar()->setRange(0, std::max(0, contentWidth - visibleWidth));
  int contentHeight = axisHeight + int(data->tracks.size()) * rowHeight;
  verticalScrollBar()->setPageStep(viewport()->height());
  verticalScrollBar()->setRange(0, std::max(0, contentHeight - viewport()->height()));
}

void VcdWaveView::setScale(double newScale, int anchorX)
{
  // The date under [anchorX] stays in place
  double t = (anchorX - leftWidth() - margin + horizontalScrollBar()->value()) / scale;
  scale = std::clamp(newScale, minScale(), std::max(minScale(), maxScale()));
  fitted = scale <= minScale();
  updateScrollBars();
  horizontalScrollBar()->setValue(int(std::lround(leftWidth() + margin + t * scale - anchorX)));
  viewport()->update();
}

void VcdWaveView::zoomIn() { setScale(scale * 1.5, leftWidth() + traceWidth() / 2); }
void VcdWaveView::zoomOut() { setScale(scale / 1.5, leftWidth() + traceWidth() / 2); }

void VcdWaveView::zoomFit()
{
  scale = minScale();
  fitted = true;
  updateScrollBars();
  horizontalScrollBar()->setValue(0);
  viewport()->update();
}

void VcdWaveView::resizeEvent(QResizeEvent *event)
{
  QAbstractScrollArea::resizeEvent(event);
  if ( fitted ) zoomFit();
  else setScale(scale, leftWidth());
}

void VcdWaveView::wheelEvent(QWheelEvent *event)
{
  if ( event->modifiers() & Qt::ControlModifier ) { // [Cmd] on macOS
    double factor = std::pow(1.0015, event->angleDelta().y());
    setScale(scale * factor, int(event->position().x()));
    event->accept();
    return;
    }
  QAbstractScrollArea::wheelEvent(event);
}

bool VcdWaveView::viewportEvent(QEvent *event)
{
  if ( event->type() == QEvent::NativeGesture ) { // Pinch on a trackpad
    QNativeGestureEvent *g = static_cast<QNativeGestureEvent*>(event);
    if ( g->gestureType() == Qt::ZoomNativeGesture ) {
      setScale(scale * (1.0 + g->value()), int(g->position().x()));
      return true;
      }
    }
  return QAbstractScrollArea::viewportEvent(event);
}

void VcdWaveView::setCursorAt(int x)
{
  if ( x < leftWidth() ) return;
  qint64 t = xToTime(x);
  // The cursor snaps to the nearest change of a signal, if it is close enough (events last only an instant)
  qint64 best = -1;
  int bestDist = 6;
  for ( const VcdSignal &s : data->tracks )
    for ( const VcdSignal::Change &c : s.changes ) {
      int d = std::abs(timeToX(c.time) - x);
      if ( d <= bestDist ) { bestDist = d; best = c.time; }
      }
  cursor = best >= 0 ? best : t;
  viewport()->update();
  emit cursorMoved(cursor);
}

int VcdWaveView::rowAt(int y) const
{
  if ( y < axisHeight ) return -1;
  int row = (y - axisHeight + verticalScrollBar()->value()) / rowHeight;
  return row < data->tracks.size() ? row : -1;
}

int VcdWaveView::insertionIndexAt(int y) const
{
  double pos = double(y - axisHeight + verticalScrollBar()->value()) / rowHeight;
  return std::clamp(int(std::lround(pos)), 0, int(data->tracks.size()));
}

void VcdWaveView::moveSignal(int from, int to)
{
  to = std::clamp(to, 0, int(data->tracks.size()) - 1);
  if ( from < 0 || from == to ) return;
  data->tracks.move(from, to);
  selectedRow = to;
  // The view is scrolled to show the moved signal
  int top = axisHeight + to * rowHeight - verticalScrollBar()->value();
  if ( top < axisHeight ) verticalScrollBar()->setValue(to * rowHeight);
  else if ( top + rowHeight > viewport()->height() )
    verticalScrollBar()->setValue(axisHeight + (to + 1) * rowHeight - viewport()->height());
  viewport()->update();
  emit layoutChanged();
}

void VcdWaveView::mousePressEvent(QMouseEvent *event)
{
  int x = int(event->position().x()), y = int(event->position().y());
  if ( event->button() != Qt::LeftButton ) { QAbstractScrollArea::mousePressEvent(event); return; }
  if ( x < leftWidth() ) { // Selects the signal, which can be dragged to another place
    selectedRow = dragRow = rowAt(y);
    dropIndex = -1;
    viewport()->update();
    }
  else
    setCursorAt(x);
}

void VcdWaveView::mouseMoveEvent(QMouseEvent *event)
{
  if ( ! (event->buttons() & Qt::LeftButton) ) { QAbstractScrollArea::mouseMoveEvent(event); return; }
  if ( dragRow >= 0 ) {
    int y = int(event->position().y());
    // Scrolling when dragging beyond the top or the bottom of the view
    if ( y < axisHeight ) verticalScrollBar()->setValue(verticalScrollBar()->value() - rowHeight / 3);
    else if ( y > viewport()->height() ) verticalScrollBar()->setValue(verticalScrollBar()->value() + rowHeight / 3);
    dropIndex = insertionIndexAt(y);
    viewport()->setCursor(Qt::ClosedHandCursor);
    viewport()->update();
    }
  else if ( int(event->position().x()) >= leftWidth() )
    setCursorAt(int(event->position().x()));
}

void VcdWaveView::mouseReleaseEvent(QMouseEvent *event)
{
  if ( event->button() == Qt::LeftButton && dragRow >= 0 ) {
    if ( dropIndex >= 0 ) moveSignal(dragRow, dropIndex > dragRow ? dropIndex - 1 : dropIndex);
    dragRow = dropIndex = -1;
    viewport()->unsetCursor();
    viewport()->update();
    return;
    }
  QAbstractScrollArea::mouseReleaseEvent(event);
}

void VcdWaveView::contextMenuEvent(QContextMenuEvent *event)
{
  int row = rowAt(event->pos().y());
  if ( event->pos().x() >= leftWidth() || row < 0 ) return;
  selectedRow = row;
  viewport()->update();
  int last = int(data->tracks.size()) - 1;
  QMenu menu(this);
  menu.addAction("Move up", [=]() { moveSignal(row, row - 1); })->setEnabled(row > 0);
  menu.addAction("Move down", [=]() { moveSignal(row, row + 1); })->setEnabled(row < last);
  menu.addAction("Move to top", [=]() { moveSignal(row, 0); })->setEnabled(row > 0);
  menu.addAction("Move to bottom", [=]() { moveSignal(row, last); })->setEnabled(row < last);
  menu.exec(event->globalPos());
}

void VcdWaveView::keyPressEvent(QKeyEvent *event)
{
  switch ( event->key() ) {
  case Qt::Key_Left:
  case Qt::Key_Right: { // Moves the cursor to the previous / next change
    bool next = event->key() == Qt::Key_Right;
    qint64 from = cursor < 0 ? (next ? -1 : data->endTime + 1) : cursor;
    qint64 target = -1;
    for ( const VcdSignal &s : data->tracks )
      for ( const VcdSignal::Change &c : s.changes )
        if ( next ? c.time > from && (target < 0 || c.time < target) : c.time < from && c.time > target )
          target = c.time;
    if ( target < 0 ) return;
    cursor = target;
    // The view is scrolled to show the cursor
    int x = timeToX(cursor);
    if ( x < leftWidth() + margin || x > viewport()->width() - margin )
      horizontalScrollBar()->setValue(horizontalScrollBar()->value() + x - leftWidth() - traceWidth() / 2);
    viewport()->update();
    emit cursorMoved(cursor);
    break;
    }
  case Qt::Key_Up:
  case Qt::Key_Down: { // Selects the previous / next signal ([Alt] : moves the selected signal)
    int delta = event->key() == Qt::Key_Up ? -1 : 1;
    if ( event->modifiers() & Qt::AltModifier ) {
      if ( selectedRow >= 0 ) moveSignal(selectedRow, selectedRow + delta);
      }
    else if ( ! data->tracks.isEmpty() ) {
      selectedRow = std::clamp(selectedRow < 0 ? 0 : selectedRow + delta, 0, int(data->tracks.size()) - 1);
      viewport()->update();
      }
    break;
    }
  case Qt::Key_Escape:
    cursor = -1;
    viewport()->update();
    emit cursorMoved(cursor);
    break;
  case Qt::Key_Plus:
  case Qt::Key_Equal: zoomIn(); break;
  case Qt::Key_Minus: zoomOut(); break;
  default: QAbstractScrollArea::keyPressEvent(event);
  }
}

void VcdWaveView::drawAxis(QPainter &painter)
{
  QRect r(0, 0, viewport()->width(), axisHeight);
  painter.fillRect(r, palette().window());
  painter.setPen(palette().text().color());
  QFont bold = painter.font();
  bold.setBold(true);
  painter.setFont(bold);
  painter.drawText(QRect(margin, 0, namesWidth - margin, axisHeight), Qt::AlignVCenter | Qt::AlignLeft, "Signal");
  painter.drawText(QRect(namesWidth + margin, 0, valuesWidth - margin, axisHeight), Qt::AlignVCenter | Qt::AlignLeft,
                   "Value");
  painter.setFont(font());
  // Ticks : steps of 1, 2 or 5 x 10^k time units, at least 80 pixels apart
  double minStep = 80.0 / scale;
  qint64 step = 1;
  while ( step < minStep ) {
    if ( step * 2 >= minStep ) { step *= 2; break; }
    if ( step * 5 >= minStep ) { step *= 5; break; }
    step *= 10;
    }
  painter.save();
  painter.setClipRect(QRect(leftWidth(), 0, viewport()->width() - leftWidth(), viewport()->height()));
  qint64 first = (xToTime(leftWidth()) / step) * step;
  QColor grid = palette().mid().color();
  grid.setAlpha(70);
  for ( qint64 t = first; t <= data->endTime; t += step ) {
    int x = timeToX(t);
    if ( x > viewport()->width() ) break;
    painter.setPen(palette().text().color());
    painter.drawLine(x, axisHeight - 6, x, axisHeight);
    painter.drawText(x + 3, axisHeight - 9, timeText(t));
    painter.setPen(grid);
    painter.drawLine(x, axisHeight, x, viewport()->height());
    }
  painter.restore();
  painter.setPen(palette().mid().color());
  painter.drawLine(0, axisHeight - 1, viewport()->width(), axisHeight - 1);
}

void VcdWaveView::drawSignal(QPainter &painter, const VcdSignal &signal, int top)
{
  int hi = top + 7, lo = top + rowHeight - 7, mid = (hi + lo) / 2;
  int right = viewport()->width();
  int xEnd = timeToX(data->endTime);
  painter.setRenderHint(QPainter::Antialiasing, true);
  switch ( signal.kind ) {
  case VcdSignal::Event:
    painter.setPen(QPen(eventColor, 2));
    painter.setBrush(eventColor);
    for ( const VcdSignal::Change &c : signal.changes ) {
      int x = timeToX(c.time);
      if ( x < leftWidth() - 5 || x > right + 5 ) continue;
      painter.drawLine(x, lo, x, hi + 4);
      QPolygon head({ QPoint(x - 4, hi + 5), QPoint(x + 4, hi + 5), QPoint(x, hi - 1) });
      painter.drawPolygon(head);
      }
    break;
  case VcdSignal::Bool: {
    painter.setPen(QPen(boolColor, 2));
    painter.setBrush(Qt::NoBrush);
    QPainterPath path;
    for ( int k = 0; k < signal.changes.size(); k++ ) {
      const VcdSignal::Change &c = signal.changes[k];
      int x1 = timeToX(c.time);
      int x2 = k + 1 < signal.changes.size() ? timeToX(signal.changes[k+1].time) : xEnd;
      int y = c.value == "1" ? hi : c.value == "0" ? lo : mid;
      if ( k == 0 ) path.moveTo(x1, y); else path.lineTo(x1, y);
      path.lineTo(x2, y);
      }
    painter.drawPath(path);
    break;
    }
  default: { // Int, State, Real : a band per value, labelled with the value
    QFontMetrics fm(font());
    for ( int k = 0; k < signal.changes.size(); k++ ) {
      const VcdSignal::Change &c = signal.changes[k];
      int x1 = timeToX(c.time);
      int x2 = k + 1 < signal.changes.size() ? timeToX(signal.changes[k+1].time) : xEnd;
      if ( x2 < leftWidth() || x1 > right ) continue;
      int d = std::min(4, (x2 - x1) / 2);
      QPolygon band({ QPoint(x1, mid), QPoint(x1 + d, hi), QPoint(x2 - d, hi), QPoint(x2, mid), QPoint(x2 - d, lo),
                      QPoint(x1 + d, lo) });
      QColor fill = valueColor;
      fill.setAlpha(40);
      painter.setPen(QPen(valueColor, 1.5));
      painter.setBrush(fill);
      painter.drawPolygon(band);
      // The label is kept visible when the start of the band is scrolled out
      int lx1 = std::max(x1 + d + 3, leftWidth() + 3), lx2 = std::min(x2 - d - 3, right);
      if ( lx2 - lx1 >= fm.horizontalAdvance("0") ) {
        painter.setPen(palette().text().color());
        QString text = fm.elidedText(c.value, Qt::ElideRight, lx2 - lx1);
        painter.drawText(QRect(lx1, hi, lx2 - lx1, lo - hi), Qt::AlignVCenter | Qt::AlignLeft, text);
        }
      }
    break;
    }
  }
  painter.setRenderHint(QPainter::Antialiasing, false);
}

void VcdWaveView::paintEvent(QPaintEvent *)
{
  QPainter painter(viewport());
  int w = viewport()->width(), h = viewport()->height();
  painter.fillRect(0, 0, w, h, palette().base());
  int vscroll = verticalScrollBar()->value();
  QFontMetrics fm(font());
  painter.save();
  painter.setClipRect(0, axisHeight, w, h - axisHeight);
  for ( int k = 0; k < data->tracks.size(); k++ ) {
    const VcdSignal &s = data->tracks[k];
    int top = axisHeight + k * rowHeight - vscroll;
    if ( top + rowHeight < axisHeight ) continue;
    if ( top > h ) break;
    if ( k % 2 ) painter.fillRect(0, top, w, rowHeight, palette().alternateBase());
    if ( k == selectedRow ) {
      QColor tint = palette().highlight().color();
      tint.setAlpha(60);
      painter.fillRect(0, top, w, rowHeight, tint);
      }
    painter.setPen(palette().text().color());
    painter.drawText(QRect(margin, top, namesWidth - margin, rowHeight), Qt::AlignVCenter | Qt::AlignLeft,
                     fm.elidedText(s.name, Qt::ElideRight, namesWidth - 2 * margin));
    if ( cursor >= 0 ) {
      QString v = s.valueAt(cursor);
      painter.drawText(QRect(namesWidth + margin, top, valuesWidth - margin, rowHeight), Qt::AlignVCenter | Qt::AlignLeft,
                       fm.elidedText(v, Qt::ElideRight, valuesWidth - 2 * margin));
      }
    painter.save();
    painter.setClipRect(leftWidth(), std::max(top, axisHeight), w - leftWidth(), rowHeight);
    drawSignal(painter, s, top);
    painter.restore();
    }
  if ( dragRow >= 0 && dropIndex >= 0 ) { // Where the dragged signal will be inserted
    int y = axisHeight + dropIndex * rowHeight - vscroll;
    painter.setPen(QPen(palette().highlight().color(), 3));
    painter.drawLine(0, y, w, y);
    }
  painter.restore();
  drawAxis(painter);
  // Separators of the name and value columns
  painter.setPen(palette().mid().color());
  painter.drawLine(namesWidth, 0, namesWidth, h);
  painter.drawLine(leftWidth() - 1, 0, leftWidth() - 1, h);
  // Cursor
  if ( cursor >= 0 ) {
    int x = timeToX(cursor);
    if ( x >= leftWidth() && x <= w ) {
      QColor c = palette().highlight().color();
      painter.setPen(QPen(c, 1));
      painter.drawLine(x, axisHeight, x, h);
      QString text = timeText(cursor);
      QRect box(x - fm.horizontalAdvance(text) / 2 - 4, 2, fm.horizontalAdvance(text) + 8, axisHeight - 4);
      if ( box.left() < leftWidth() ) box.moveLeft(leftWidth());
      painter.fillRect(box, c);
      painter.setPen(palette().highlightedText().color());
      painter.drawText(box, Qt::AlignCenter, text);
      }
    }
}

// Viewer window

VcdViewer* VcdViewer::open(const QString &fname, QWidget *parent)
{
  VcdData *data = new VcdData();
  QString error;
  if ( ! data->read(fname, &error) ) {
    delete data;
    QMessageBox::warning(parent, "Simulation results", error);
    return nullptr;
    }
  data->reorder(gtkwFileOf(fname));
  qDebug() << "VcdViewer: read" << data->tracks.size() << "signals from" << fname << ", end time =" << data->endTime;
  VcdViewer *viewer = new VcdViewer(data, fname, parent);
  viewer->show();
  return viewer;
}

VcdViewer::VcdViewer(VcdData *data, const QString &fname, QWidget *parent) : QWidget(parent), data(data), vcdFile(fname)
{
  setWindowFlag(Qt::Window, true); // Top-level window
  setAttribute(Qt::WA_DeleteOnClose);
  setWindowTitle(QFileInfo(fname).fileName() + "[*]"); // [*] : marks a signal order not saved

  view = new VcdWaveView(data, this);

  auto button = [this](const QString &text, const QString &tip) {
    QToolButton *b = new QToolButton(this);
    b->setText(text);
    b->setToolTip(tip);
    return b;
    };
  QToolButton *zoomInButton = button("Zoom in", "Zoom in ([Cmd]+wheel or pinch on the chronograms)");
  QToolButton *zoomOutButton = button("Zoom out", "Zoom out ([Cmd]+wheel or pinch on the chronograms)");
  QToolButton *fitButton = button("Fit", "Show the whole simulation");
  connect(zoomInButton, &QToolButton::clicked, view, &VcdWaveView::zoomIn);
  connect(zoomOutButton, &QToolButton::clicked, view, &VcdWaveView::zoomOut);
  connect(fitButton, &QToolButton::clicked, view, &VcdWaveView::zoomFit);
  QToolButton *saveButton = button("Save layout", "Save the order of the signals in " + QFileInfo(gtkwFileOf(fname)).fileName()
                                   + " (read when these results are displayed again, and by gtkwave)");
  connect(saveButton, &QToolButton::clicked, this, &VcdViewer::saveLayout);
  connect(view, &VcdWaveView::layoutChanged, this, [this]() { setWindowModified(true); });

  static const QString hint = "Click to show the values at a given time";
  cursorLabel = new QLabel(hint);
  connect(view, &VcdWaveView::cursorMoved, this, [this](qint64 time) {
    cursorLabel->setText(time < 0 ? hint : "Values at t = " + QString::number(time * this->data->timeScale) + " "
                         + this->data->timeUnit);
    });

  QHBoxLayout *bar = new QHBoxLayout();
  bar->addWidget(zoomInButton);
  bar->addWidget(zoomOutButton);
  bar->addWidget(fitButton);
  bar->addWidget(saveButton);
  bar->addSpacing(12);
  bar->addWidget(cursorLabel, 1);
  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->addLayout(bar);
  layout->addWidget(view, 1);

  int height = 26 + int(data->tracks.size()) * 30 + 2 * view->frameWidth() + view->horizontalScrollBar()->sizeHint().height()
               + zoomInButton->sizeHint().height() + 3 * layout->contentsMargins().top();
  resize(boundedViewerSize(QSize(1000, height)));
  view->setFocus();
}

void VcdViewer::saveLayout()
{
  QString error;
  QString gtkwFile = gtkwFileOf(vcdFile);
  if ( ! data->saveLayout(gtkwFile, vcdFile, &error) ) {
    QMessageBox::warning(this, "Save layout", error);
    return;
    }
  qDebug() << "VcdViewer: saved layout in" << gtkwFile;
  setWindowModified(false);
}

VcdViewer::~VcdViewer()
{
  delete data;
}
