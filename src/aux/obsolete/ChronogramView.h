#ifndef CHRONOGRAMVIEW_H
#define CHRONOGRAMVIEW_H

#include <QWidget>
#include "StimulusTrack.h"

/**
 * @brief Zone graphique affichant le chronogramme d'UNE piste et permettant
 *        d'ajouter un stimulus par simple clic à la date correspondante.
 *
 * - Piste Event : chaque stimulus est un repère vertical (flèche) sur l'axe
 *   des temps, avec le nom de l'événement affiché au-dessus.
 * - Piste ValueChange : les valeurs numériques sont tracées comme un signal
 *   en escalier ; une valeur non numérique est affichée à mi-hauteur avec
 *   son texte en étiquette.
 *
 * Clic gauche dans une zone vide -> addStimulusRequested(date)
 * Clic gauche (ou double-clic) sur un stimulus existant -> editStimulusRequested(index)
 * Clic droit sur un stimulus existant -> removeStimulusRequested(index)
 */
class ChronogramView : public QWidget
{
    Q_OBJECT
public:
    explicit ChronogramView(QWidget *parent = nullptr);

    /// Piste actuellement affichée (non possédée par la vue, peut être nullptr)
    void setTrack(StimulusTrack *track);
    StimulusTrack *track() const { return m_track; }

    /// Échelle horizontale, en pixels par unité de temps
    void setPixelsPerUnit(double ppu);
    double pixelsPerUnit() const { return m_pixelsPerUnit; }

    /// Durée totale visible (dimensionne la largeur du widget)
    void setDuration(double duration);

    QSize sizeHint() const override;

signals:
    void addStimulusRequested(double date);
    void editStimulusRequested(int index);
    void removeStimulusRequested(int index);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    double xToDate(int x) const;
    int dateToX(double date) const;
    void drawTimeAxis(QPainter &painter, int height);
    void drawEventTrack(QPainter &painter, int height);
    void drawValueTrack(QPainter &painter, int height);

    StimulusTrack *m_track = nullptr;
    double m_pixelsPerUnit = 4.0;
    double m_duration = 200.0;

    static constexpr int kAxisHeight = 24;
    static constexpr int kMarginTop = 20;
    static constexpr double kClickToleranceUnits = 3.0; // tolérance de clic, en unités de temps
};

#endif // CHRONOGRAMVIEW_H
