#ifndef CHRONOGRAMEDITORWIDGET_H
#define CHRONOGRAMEDITORWIDGET_H

#include <QWidget>
#include "StimulusScenario.h"

class QPushButton;
class QDoubleSpinBox;
class QScrollArea;
class QVBoxLayout;
class QHBoxLayout;
class MultiTrackChronogramView;

/**
 * @brief Widget d'édition graphique d'un ensemble de pistes fourni par
 *        l'application hôte : à gauche le nom et le type de chaque piste
 *        (lecture seule), à droite les chronogrammes correspondants,
 *        alignés ligne à ligne.
 *
 * Les pistes elles-mêmes (nom, type) sont définies en amont par
 * l'application hôte via scenario() ; ce widget ne permet que d'ajouter,
 * modifier ou supprimer les STIMULI de chaque piste, pas les pistes elles-mêmes.
 *
 * Conçu pour être intégré dans un QDialog modal : le bouton "Fermer" n'agit
 * pas directement sur une fenêtre, il émet closeRequested() pour laisser le
 * conteneur (QDialog::accept()/reject(), QMainWindow::close()...) décider.
 */
class ChronogramEditorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ChronogramEditorWidget(QWidget *parent = nullptr);

    StimulusScenario &scenario() { return m_scenario; }
    const StimulusScenario &scenario() const { return m_scenario; }

    /// Reconstruit entièrement les deux panneaux à partir du scénario actuel.
    /// À appeler après toute modification du scénario faite par programme
    /// (ajout/suppression de pistes en amont, avant affichage typiquement).
    void rebuildPanels();

    /// Taille permettant d'afficher toutes les pistes et toute la durée [0, Tmax]
    /// sans défilement (le conteneur la borne ensuite à l'écran).
    QSize sizeHint() const override;

    /// Met en évidence la piste [track] (nom en gras, fond teinté) et fait défiler la vue
    /// pour la rendre visible. nullptr : aucune piste mise en évidence.
    void highlightTrack(StimulusTrack *track);

signals:
    void closeRequested();
    void cancelRequested();

private slots:
    void onTmaxChanged(double tmax);
    void onZoomInClicked();
    void onZoomOutClicked();
    void onZoomFitClicked();
    void onPrecisionChanged(double precision);
    void onCloseClicked();
    void onCancelClicked();
    void onPeriodicFillRequested(StimulusTrack *track, double clickedDate);
    void onAddStimulusRequested(StimulusTrack *track, double date);
    void onEditStimulusRequested(StimulusTrack *track, int index);
    void onRemoveStimulusRequested(StimulusTrack *track, int index);

private:
    void setupUi();
    bool eventFilter(QObject *watched, QEvent *event) override;
    QWidget *buildTrackRow(StimulusTrack *track);
    void requestClearTrack(StimulusTrack *track);

    StimulusScenario m_scenario;

    QDoubleSpinBox *m_tmaxSpin = nullptr;
    QPushButton *m_zoomInButton = nullptr;
    QPushButton *m_zoomOutButton = nullptr;
    QPushButton *m_zoomFitButton = nullptr;
    QDoubleSpinBox *m_precisionSpin = nullptr;
    QPushButton *m_closeButton = nullptr;
    QPushButton *m_cancelButton = nullptr;

    QHBoxLayout *m_toolbarLayout = nullptr;
    QHBoxLayout *m_bottomLayout = nullptr;

    QScrollArea *m_leftScrollArea = nullptr;
    QWidget *m_leftContent = nullptr;
    QVBoxLayout *m_leftContentLayout = nullptr;

    QScrollArea *m_rightScrollArea = nullptr;
    MultiTrackChronogramView *m_multiView = nullptr;
};

#endif // CHRONOGRAMEDITORWIDGET_H
