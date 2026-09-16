#include "ChronogramView.h"

#include <QPainter>
#include <QMouseEvent>
#include <cmath>

ChronogramView::ChronogramView(QWidget *parent)
    : QWidget(parent)
{
    setMinimumHeight(140);
}

void ChronogramView::setTrack(StimulusTrack *track)
{
    m_track = track;
    update();
}

void ChronogramView::setPixelsPerUnit(double ppu)
{
    m_pixelsPerUnit = qMax(0.1, ppu);
    updateGeometry();
    update();
}

void ChronogramView::setDuration(double duration)
{
    m_duration = qMax(1.0, duration);
    updateGeometry();
    update();
}

QSize ChronogramView::sizeHint() const
{
    return QSize(dateToX(m_duration) + 40, 160);
}

double ChronogramView::xToDate(int x) const
{
    return x / m_pixelsPerUnit;
}

int ChronogramView::dateToX(double date) const
{
    return static_cast<int>(std::round(date * m_pixelsPerUnit));
}

void ChronogramView::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), palette().base());

    const int h = height();
    drawTimeAxis(painter, h);

    if (!m_track)
        return;

    if (m_track->type() == TrackStimulusType::Event)
        drawEventTrack(painter, h);
    else
        drawValueTrack(painter, h);
}

void ChronogramView::drawTimeAxis(QPainter &painter, int h)
{
    const int axisY = h - kAxisHeight;
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
        painter.drawLine(x, axisY, x, axisY + 4);
        painter.drawText(QRect(x - 25, axisY + 5, 50, kAxisHeight - 5),
                          Qt::AlignHCenter | Qt::AlignTop, QString::number(t, 'g', 6));
    }
}

void ChronogramView::drawEventTrack(QPainter &painter, int h)
{
    const int axisY = h - kAxisHeight;

    for (int i = 0; i < m_track->count(); ++i) {
        const auto *ev = static_cast<const EventStimulus *>(m_track->at(i));
        const int x = dateToX(ev->date());

        painter.setPen(QPen(palette().highlight().color(), 2));
        painter.drawLine(x, kMarginTop, x, axisY);
        painter.drawLine(x, axisY, x - 4, axisY - 6);
        painter.drawLine(x, axisY, x + 4, axisY - 6);

        painter.setPen(palette().text().color());
        QString label = ev->eventName();
        if (ev->param().isValid())
            label += QString(" (%1)").arg(ev->param().toString());
        painter.drawText(x + 4, kMarginTop + 10, label);
    }
}

void ChronogramView::drawValueTrack(QPainter &painter, int h)
{
    const int axisY = h - kAxisHeight;
    const int top = kMarginTop + 14; // place pour les étiquettes
    const int plotHeight = axisY - top;
    if (plotHeight <= 0 || m_track->count() == 0)
        return;

    double minV = 0.0, maxV = 1.0;
    const bool numeric = m_track->numericRange(minV, maxV);
    if (numeric && qFuzzyCompare(minV + 1.0, maxV + 1.0)) {
        // valeur constante : on élargit la plage pour éviter une division par zéro
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
    for (int i = 0; i < m_track->count(); ++i) {
        const auto *vc = static_cast<const ValueChangeStimulus *>(m_track->at(i));
        const int x = dateToX(vc->date());
        const int y = valueToY(vc->value());

        if (prevY >= 0) {
            // maintien de la valeur précédente puis saut vertical (signal en escalier)
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

void ChronogramView::mousePressEvent(QMouseEvent *event)
{
    if (!m_track)
        return;

    const double date = xToDate(event->pos().x());
    const int idx = m_track->indexNear(date, kClickToleranceUnits);

    if (event->button() == Qt::LeftButton) {
        if (idx >= 0)
            emit editStimulusRequested(idx);
        else
            emit addStimulusRequested(qMax(0.0, date));
    } else if (event->button() == Qt::RightButton && idx >= 0) {
        emit removeStimulusRequested(idx);
    }
}

void ChronogramView::mouseDoubleClickEvent(QMouseEvent *event)
{
    mousePressEvent(event);
}
