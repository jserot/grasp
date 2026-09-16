#ifndef MULTITRACKCHRONOGRAMVIEW_H
#define MULTITRACKCHRONOGRAMVIEW_H

#include <QWidget>
#include <QVector>
#include "StimulusTrack.h"

/**
 * @brief Affiche l'ensemble des pistes d'un scénario, empilées verticalement,
 *        chaque piste occupant une bande de hauteur fixe (rowHeight).
 *
 * Cette hauteur de ligne (et la hauteur de la bande d'axe en haut) doit être
 * répliquée dans le panneau de liste affiché à gauche pour que les deux
 * restent alignés ligne à ligne.
 *
 * Clic gauche dans la bande d'une piste, en dehors d'un point existant ->
 * ajoute un stimulus à la date correspondante sur cette piste. Clic gauche
 * (ou double-clic) sur un point existant -> édition. Clic droit -> suppression.
 */
class MultiTrackChronogramView : public QWidget
{
    Q_OBJECT
public:
    explicit MultiTrackChronogramView(QWidget *parent = nullptr);

    /// Pistes à afficher, dans l'ordre (non possédées par la vue)
    void setTracks(const QVector<StimulusTrack *> &tracks);

    void setPixelsPerUnit(double ppu);
    double pixelsPerUnit() const { return m_pixelsPerUnit; }

    void setDuration(double duration);

    void setRowHeight(int rowHeight);
    int rowHeight() const { return m_rowHeight; }
    int axisHeight() const { return kAxisHeight; }

    /// Pas d'accrochage temporel utilisé lors de l'AJOUT d'un stimulus par clic
    /// (la date cliquée est arrondie au multiple le plus proche). 0 = pas d'accrochage.
    void setSnapStep(double step);
    double snapStep() const { return m_snapStep; }

    QSize sizeHint() const override;

signals:
    void addStimulusRequested(StimulusTrack *track, double date);
    void editStimulusRequested(StimulusTrack *track, int index);
    void removeStimulusRequested(StimulusTrack *track, int index);
    /// Clic droit dans une zone vide d'une piste de type Événement : propose un remplissage périodique
    void periodicFillRequested(StimulusTrack *track, double date);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    double xToDate(int x) const;
    int dateToX(double date) const;
    double snapDate(double date) const;
    void drawTimeAxis(class QPainter &painter);
    void drawEventTrack(class QPainter &painter, StimulusTrack *track, int bandTop);
    void drawValueTrack(class QPainter &painter, StimulusTrack *track, int bandTop);
    bool hitTest(const QPoint &pos, StimulusTrack **outTrack, int *outIndex, double *outDate) const;

    QVector<StimulusTrack *> m_tracks;
    double m_pixelsPerUnit = 4.0;
    double m_duration = 200.0;
    int m_rowHeight = 70;
    double m_snapStep = 5.0;

    static constexpr int kAxisHeight = 30;
    static constexpr double kClickToleranceUnits = 3.0;
};

#endif // MULTITRACKCHRONOGRAMVIEW_H
