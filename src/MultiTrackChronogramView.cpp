#include "MultiTrackChronogramView.h"

#include <QPainter>
#include <QMouseEvent>
#include <QTimer>
#include <cmath>

MultiTrackChronogramView::MultiTrackChronogramView(QWidget *parent)
    : QWidget(parent)
{
}

void MultiTrackChronogramView::setTracks(const QVector<StimulusTrack *> &tracks)
{
    m_tracks = tracks;
    updateGeometry();
    adjustSize(); // sans layout, seul un appel explicite redimensionne le widget
    update();
}

void MultiTrackChronogramView::setHighlightedTrack(StimulusTrack *track)
{
    m_highlightedTrack = track;
    update();
}

/// Teinte de fond d'une piste mise en évidence (couleur de sélection, translucide pour rester
/// lisible en thème clair comme en thème sombre)
QColor MultiTrackChronogramView::highlightTint(const QPalette &palette)
{
    QColor c = palette.highlight().color();
    c.setAlpha(60);
    return c;
}

void MultiTrackChronogramView::setPixelsPerUnit(double ppu)
{
    m_pixelsPerUnit = qMax(0.1, ppu);
    updateGeometry();
    adjustSize();
    update();
}

void MultiTrackChronogramView::setDuration(double duration)
{
    m_duration = qMax(1.0, duration);
    updateGeometry();
    adjustSize();
    update();
}

void MultiTrackChronogramView::setRowHeight(int rowHeight)
{
    m_rowHeight = qMax(20, rowHeight);
    updateGeometry();
    adjustSize();
    update();
}

void MultiTrackChronogramView::setSnapStep(double step)
{
    m_snapStep = qMax(0.0, step);
}

double MultiTrackChronogramView::snapDate(double date) const
{
    if (m_snapStep <= 0.0)
        return date;
    return std::round(date / m_snapStep) * m_snapStep;
}

QSize MultiTrackChronogramView::sizeHint() const
{
    return QSize(dateToX(m_duration) + 40, kAxisHeight + m_tracks.size() * m_rowHeight);
}

double MultiTrackChronogramView::xToDate(int x) const
{
    return x / m_pixelsPerUnit;
}

int MultiTrackChronogramView::dateToX(double date) const
{
    return static_cast<int>(std::round(date * m_pixelsPerUnit));
}

void MultiTrackChronogramView::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), palette().base());

    drawTimeAxis(painter);

    for (int i = 0; i < m_tracks.size(); ++i) {
        StimulusTrack *track = m_tracks[i];
        const int bandTop = kAxisHeight + i * m_rowHeight;

        if (i % 2 == 1)
            painter.fillRect(0, bandTop, width(), m_rowHeight, palette().alternateBase());
        if (track == m_highlightedTrack)
            painter.fillRect(0, bandTop, width(), m_rowHeight, highlightTint(palette()));

        painter.setPen(QPen(palette().mid().color()));
        painter.drawLine(0, bandTop + m_rowHeight, width(), bandTop + m_rowHeight);

        if (track->type() == TrackStimulusType::Event)
            drawEventTrack(painter, track, bandTop);
        else
            drawValueTrack(painter, track, bandTop);
    }
}

void MultiTrackChronogramView::drawTimeAxis(QPainter &painter)
{
    const int axisY = kAxisHeight;
    painter.setPen(QPen(palette().mid().color()));
    painter.drawLine(0, axisY, width(), axisY);

    // Pas de graduation "rond" proche de 80 pixels
    const double approxStep = 80.0 / m_pixelsPerUnit;
    const double magnitude = std::pow(10.0, std::floor(std::log10(qMax(approxStep, 1e-6))));
    double step = magnitude;
    if (approxStep / magnitude >= 5.0) step = magnitude * 5.0;
    else if (approxStep / magnitude >= 2.0) step = magnitude * 2.0;

    painter.setPen(QPen(palette().text().color()));
    for (double t = 0.0; t <= m_duration; t += step) {
        const int x = dateToX(t);
        painter.drawLine(x, axisY - 4, x, axisY);
        painter.drawText(QRect(x - 25, 2, 50, axisY - 6), Qt::AlignHCenter | Qt::AlignBottom,
                          QString::number(t, 'g', 6));
    }
}

void MultiTrackChronogramView::drawEventTrack(QPainter &painter, StimulusTrack *track, int bandTop)
{
    const int top = bandTop + 4;
    const int baseline = bandTop + m_rowHeight - 10;

    for (int i = 0; i < track->count(); ++i) {
        const auto *ev = static_cast<const EventStimulus *>(track->at(i));
        const int x = dateToX(ev->date());

        painter.setPen(QPen(palette().highlight().color(), 2));
        painter.drawLine(x, baseline, x, top);
        painter.drawLine(x, top, x - 4, top + 6);
        painter.drawLine(x, top, x + 4, top + 6);
    }
}

void MultiTrackChronogramView::drawValueTrack(QPainter &painter, StimulusTrack *track, int bandTop)
{
    const int top = bandTop + 18;
    const int bottom = bandTop + m_rowHeight - 8;
    const int plotHeight = bottom - top;
    if (plotHeight <= 0 || track->count() == 0)
        return;

    double minV = 0.0, maxV = 1.0;
    const bool numeric = track->numericRange(minV, maxV);
    if (numeric && qFuzzyCompare(minV + 1.0, maxV + 1.0)) {
        minV -= 1.0;
        maxV += 1.0;
    }

    auto valueToY = [&](const QVariant &v) -> int {
        if (!numeric)
            return top + plotHeight / 2;
        bool ok = false;
        const double d = v.toDouble(&ok);
        if (!ok)
            return top + plotHeight / 2;
        const double ratio = (d - minV) / (maxV - minV);
        return top + static_cast<int>(std::round((1.0 - ratio) * plotHeight));
    };

    painter.setPen(QPen(palette().highlight().color(), 2));

    int prevX = 0;
    int prevY = -1;
    for (int i = 0; i < track->count(); ++i) {
        const auto *vc = static_cast<const ValueChangeStimulus *>(track->at(i));
        const int x = dateToX(vc->date());
        const int y = valueToY(vc->value());

        if (prevY >= 0) {
            painter.drawLine(prevX, prevY, x, prevY);
            painter.drawLine(x, prevY, x, y);
        }

        painter.drawEllipse(QPoint(x, y), 3, 3);
        painter.setPen(palette().text().color());
        painter.drawText(x + 4, y - 4, vc->value().toString());
        painter.setPen(QPen(palette().highlight().color(), 2));

        prevX = x;
        prevY = y;
    }

    if (prevY >= 0)
        painter.drawLine(prevX, prevY, dateToX(m_duration), prevY);
}

bool MultiTrackChronogramView::hitTest(const QPoint &pos, StimulusTrack **outTrack,
                                        int *outIndex, double *outDate) const
{
    if (pos.y() < kAxisHeight)
        return false;

    const int bandIndex = (pos.y() - kAxisHeight) / m_rowHeight;
    if (bandIndex < 0 || bandIndex >= m_tracks.size())
        return false;

    StimulusTrack *track = m_tracks[bandIndex];
    const double date = xToDate(pos.x());
    const int idx = track->indexNear(date, kClickToleranceUnits);

    if (outTrack) *outTrack = track;
    if (outIndex) *outIndex = idx;
    if (outDate) *outDate = date;
    return true;
}

void MultiTrackChronogramView::mousePressEvent(QMouseEvent *event)
{
    StimulusTrack *track = nullptr;
    int idx = -1;
    double date = 0.0;
    if (!hitTest(event->pos(), &track, &idx, &date) || !track)
        return;

    // On diffère l'émission (via QTimer::singleShot(0, ...)) plutôt que d'émettre
    // directement ici : les slots connectés ouvrent des boîtes de dialogue modales
    // (exec()), et le faire de façon synchrone en plein traitement de
    // mousePressEvent perturbe le grab implicite de la souris maintenu par Qt
    // entre l'appui et le relâchement du bouton, ce qui peut laisser l'IHM dans
    // un état incohérent (boutons ne répondant plus) une fois le dialogue fermé.
    if (event->button() == Qt::LeftButton) {
        if (idx >= 0) {
            QTimer::singleShot(0, this, [this, track, idx]() {
                emit editStimulusRequested(track, idx);
            });
        } else {
            const double snapped = snapDate(qMax(0.0, date));
            QTimer::singleShot(0, this, [this, track, snapped]() {
                emit addStimulusRequested(track, snapped);
            });
        }
    } else if (event->button() == Qt::RightButton) {
        if (idx >= 0) {
            QTimer::singleShot(0, this, [this, track, idx]() {
                emit removeStimulusRequested(track, idx);
            });
        } else if (track->type() == TrackStimulusType::Event) {
            const double clicked = qMax(0.0, date);
            QTimer::singleShot(0, this, [this, track, clicked]() {
                emit periodicFillRequested(track, clicked);
            });
        }
    }
}

void MultiTrackChronogramView::mouseDoubleClickEvent(QMouseEvent *event)
{
    // Qt envoie appui, relâchement, double-clic, relâchement : le premier appui a déjà été
    // traité par mousePressEvent, et le double-clic n'est émis que si le second clic est
    // tout près du premier (il vise donc la même chose). Le retraiter ouvrirait un second
    // dialogue (doublons à l'ajout, suppression du mauvais stimulus...). On l'ignore donc.
    event->accept();
}
