#include "ChronogramEditorWidget.h"
#include "MultiTrackChronogramView.h"
#include "StimulusEditorDialog.h"

#include <QPushButton>
#include <QDoubleSpinBox>
#include <QScrollArea>
#include <QScrollBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QLabel>
#include <QFrame>
#include <QLayoutItem>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QCheckBox>
#include <QStyle>
#include <QTimer>
#include <cmath>

ChronogramEditorWidget::ChronogramEditorWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
}

void ChronogramEditorWidget::setupUi()
{
    // --- Barre d'outils : Tmax + zoom + précision temporelle ---
    m_tmaxSpin = new QDoubleSpinBox(this);
    m_tmaxSpin->setRange(1.0, 1e9);
    m_tmaxSpin->setDecimals(2);
    m_tmaxSpin->setValue(200.0);

    m_zoomInButton = new QPushButton(tr("Zoom +"), this);
    m_zoomOutButton = new QPushButton(tr("Zoom -"), this);
    m_zoomFitButton = new QPushButton(tr("Zoom Fit"), this);
    // Sous macOS, un QPushButton qui garde le focus clavier après un clic
    // s'affiche avec un halo bleu permanent. Ces boutons n'ont pas besoin
    // d'être navigables au clavier, donc on désactive ce focus.
    m_zoomInButton->setFocusPolicy(Qt::NoFocus);
    m_zoomOutButton->setFocusPolicy(Qt::NoFocus);
    m_zoomFitButton->setFocusPolicy(Qt::NoFocus);

    m_precisionSpin = new QDoubleSpinBox(this);
    m_precisionSpin->setRange(0.0, 1e6);
    m_precisionSpin->setDecimals(3);
    m_precisionSpin->setValue(5.0);
    m_precisionSpin->setToolTip(
        tr("Lors d'un ajout par clic, la date est arrondie au multiple le plus proche "
           "de cette valeur (0 = pas d'accrochage)."));

    m_toolbarLayout = new QHBoxLayout();
    m_toolbarLayout->addWidget(new QLabel(tr("Tmax :"), this));
    m_toolbarLayout->addWidget(m_tmaxSpin);
    m_toolbarLayout->addWidget(m_zoomInButton);
    m_toolbarLayout->addWidget(m_zoomOutButton);
    m_toolbarLayout->addWidget(m_zoomFitButton);
    m_toolbarLayout->addWidget(new QLabel(tr("Précision temporelle :"), this));
    m_toolbarLayout->addWidget(m_precisionSpin);
    m_toolbarLayout->addStretch();

    // --- Panneau de droite : chronogrammes empilés ---
    m_multiView = new MultiTrackChronogramView(this);

    m_rightScrollArea = new QScrollArea(this);
    m_rightScrollArea->setWidget(m_multiView);
    m_rightScrollArea->setWidgetResizable(false);

    // --- Panneau de gauche : nom + type de chaque piste, en lecture seule ---
    m_leftContent = new QWidget(this);
    m_leftContent->setFixedWidth(260);
    m_leftContentLayout = new QVBoxLayout(m_leftContent);
    m_leftContentLayout->setContentsMargins(4, 0, 4, 0);
    m_leftContentLayout->setSpacing(0);

    m_leftScrollArea = new QScrollArea(this);
    m_leftScrollArea->setWidget(m_leftContent);
    // Pas resizable : la hauteur du contenu (header + N x rowHeight) doit
    // rester identique à celle de m_multiView pour garder l'alignement.
    m_leftScrollArea->setWidgetResizable(false);
    m_leftScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_leftScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_leftScrollArea->setFixedWidth(280);
    m_leftScrollArea->setFrameShape(QFrame::NoFrame);

    // Synchronise le défilement vertical des deux panneaux
    connect(m_rightScrollArea->verticalScrollBar(), &QScrollBar::valueChanged,
            m_leftScrollArea->verticalScrollBar(), &QScrollBar::setValue);
    connect(m_leftScrollArea->verticalScrollBar(), &QScrollBar::valueChanged,
            m_rightScrollArea->verticalScrollBar(), &QScrollBar::setValue);

    auto *splitLayout = new QHBoxLayout();
    splitLayout->setSpacing(0);
    splitLayout->addWidget(m_leftScrollArea);
    splitLayout->addWidget(m_rightScrollArea, 1);

    // --- Barre de boutons en bas : Annuler / Fermer ---
    m_cancelButton = new QPushButton(tr("Annuler"), this);
    m_closeButton = new QPushButton(tr("Fermer"), this);

    m_bottomLayout = new QHBoxLayout();
    m_bottomLayout->addStretch();
    m_bottomLayout->addWidget(m_cancelButton);
    m_bottomLayout->addWidget(m_closeButton);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(m_toolbarLayout);
    mainLayout->addLayout(splitLayout, 1);
    mainLayout->addLayout(m_bottomLayout);

    connect(m_tmaxSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &ChronogramEditorWidget::onTmaxChanged);
    connect(m_zoomInButton, &QPushButton::clicked, this, &ChronogramEditorWidget::onZoomInClicked);
    connect(m_zoomOutButton, &QPushButton::clicked, this, &ChronogramEditorWidget::onZoomOutClicked);
    connect(m_zoomFitButton, &QPushButton::clicked, this, &ChronogramEditorWidget::onZoomFitClicked);
    connect(m_precisionSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &ChronogramEditorWidget::onPrecisionChanged);
    connect(m_closeButton, &QPushButton::clicked, this, &ChronogramEditorWidget::onCloseClicked);
    connect(m_cancelButton, &QPushButton::clicked, this, &ChronogramEditorWidget::onCancelClicked);
    connect(m_multiView, &MultiTrackChronogramView::addStimulusRequested,
            this, &ChronogramEditorWidget::onAddStimulusRequested);
    connect(m_multiView, &MultiTrackChronogramView::editStimulusRequested,
            this, &ChronogramEditorWidget::onEditStimulusRequested);
    connect(m_multiView, &MultiTrackChronogramView::removeStimulusRequested,
            this, &ChronogramEditorWidget::onRemoveStimulusRequested);
    connect(m_multiView, &MultiTrackChronogramView::periodicFillRequested,
            this, &ChronogramEditorWidget::onPeriodicFillRequested);

    m_multiView->setDuration(m_tmaxSpin->value());
    m_multiView->setPixelsPerUnit(4.0);
    m_multiView->setSnapStep(m_precisionSpin->value());

    rebuildPanels();
}

QWidget *ChronogramEditorWidget::buildTrackRow(StimulusTrack *track)
{
    auto *row = new QWidget(m_leftContent);
    row->setFixedHeight(m_multiView->rowHeight());

    auto *layout = new QHBoxLayout(row);
    layout->setContentsMargins(2, 4, 2, 4);

    auto *nameLabel = new QLabel(track->name(), row);
    if (track == m_multiView->highlightedTrack()) {
        QFont font = nameLabel->font();
        font.setBold(true);
        nameLabel->setFont(font);
        QPalette pal = row->palette();
        pal.setColor(QPalette::Window, MultiTrackChronogramView::highlightTint(pal));
        row->setPalette(pal);
        row->setAutoFillBackground(true);
    }
    auto *typeLabel = new QLabel(
        track->type() == TrackStimulusType::Event ? tr("Événement") : tr("Valeur"), row);
    typeLabel->setStyleSheet(QStringLiteral("color: palette(mid);"));

    auto *clearButton = new QPushButton(tr("Effacer"), row);
    clearButton->setFocusPolicy(Qt::NoFocus);
    connect(clearButton, &QPushButton::clicked, this, [this, track]() {
        requestClearTrack(track);
    });

    layout->addWidget(nameLabel, 1);
    layout->addWidget(typeLabel);
    layout->addWidget(clearButton);

    return row;
}

void ChronogramEditorWidget::rebuildPanels()
{
    // Vide le panneau de gauche
    QLayoutItem *item;
    while ((item = m_leftContentLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    // Espace réservé en haut, aligné avec la bande de l'axe des temps à droite
    auto *header = new QWidget(m_leftContent);
    header->setFixedHeight(m_multiView->axisHeight());
    auto *headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(2, 0, 2, 0);
    headerLayout->addWidget(new QLabel(tr("Nom"), header), 1);
    headerLayout->addWidget(new QLabel(tr("Type"), header));
    m_leftContentLayout->addWidget(header);

    QVector<StimulusTrack *> tracks = m_scenario.tracks();
    for (StimulusTrack *t : tracks)
        m_leftContentLayout->addWidget(buildTrackRow(t));

    m_leftContentLayout->addStretch();

    // On force le recalcul du layout puis on impose explicitement la même
    // hauteur que le panneau de droite, plutôt que de compter sur le calcul
    // implicite d'adjustSize().
    m_leftContentLayout->activate();
    const int totalHeight = m_multiView->axisHeight() + tracks.size() * m_multiView->rowHeight();
    m_leftContent->resize(m_leftContent->width(), totalHeight);
    m_leftContent->show();

    m_multiView->setTracks(tracks); // redimensionne déjà le widget en interne
}

void ChronogramEditorWidget::highlightTrack(StimulusTrack *track)
{
    m_multiView->setHighlightedTrack(track);
    rebuildPanels();
    const int index = m_scenario.tracks().indexOf(track);
    if (index < 0)
        return;
    // Le défilement n'a d'effet qu'une fois la fenêtre affichée et les zones de défilement
    // dimensionnées : on le diffère au retour dans la boucle d'événements.
    const int center = m_multiView->axisHeight() + index * m_multiView->rowHeight() + m_multiView->rowHeight() / 2;
    QTimer::singleShot(0, this, [this, center]() {
        m_rightScrollArea->ensureVisible(0, center, 0, m_multiView->rowHeight());
    });
}

QSize ChronogramEditorWidget::sizeHint() const
{
    // Les QScrollArea bornent leur propre sizeHint à quelques dizaines de lignes de texte : on calcule
    // donc explicitement la taille du contenu (axe + une bande par piste, largeur [0, Tmax]).
    // L'épaisseur des barres de défilement est ajoutée dans les deux sens : si le conteneur borne la
    // taille à l'écran, l'une d'elles apparaît sans masquer de piste ni la fin de l'axe.
    const int frame = 2 * m_rightScrollArea->frameWidth();
    const int scrollBar = style()->pixelMetric(QStyle::PM_ScrollBarExtent, nullptr, m_rightScrollArea);
    const QSize content = m_multiView->sizeHint();
    const QMargins margins = layout()->contentsMargins();
    const int spacing = qMax(0, layout()->spacing());

    const int splitWidth = m_leftScrollArea->width() + content.width() + frame + scrollBar;
    const int width = qMax(m_toolbarLayout->sizeHint().width(), splitWidth) + margins.left() + margins.right();
    const int height = margins.top() + m_toolbarLayout->sizeHint().height() + spacing
        + content.height() + frame + scrollBar
        + spacing + m_bottomLayout->sizeHint().height() + margins.bottom();
    return QSize(width, height);
}

void ChronogramEditorWidget::requestClearTrack(StimulusTrack *track)
{
    if (!track || track->count() == 0)
        return; // rien à effacer

    auto *box = new QMessageBox(
        QMessageBox::Question, tr("Confirmer"),
        tr("Effacer les %1 stimulus(es) de la piste '%2' ?").arg(track->count()).arg(track->name()),
        QMessageBox::Yes | QMessageBox::No, this);
    box->setAttribute(Qt::WA_DeleteOnClose);
    box->setModal(false);

    connect(box, &QMessageBox::finished, this, [this, track](int result) {
        if (result == QMessageBox::Yes) {
            track->clear();
            m_multiView->update();
        }
    });

    box->show();
}

void ChronogramEditorWidget::onTmaxChanged(double tmax)
{
    m_multiView->setDuration(tmax);
}

void ChronogramEditorWidget::onZoomInClicked()
{
    const double factor = 1.25;
    m_multiView->setPixelsPerUnit(m_multiView->pixelsPerUnit() * factor);
}

void ChronogramEditorWidget::onZoomOutClicked()
{
    const double factor = 1.25;
    m_multiView->setPixelsPerUnit(m_multiView->pixelsPerUnit() / factor);
}

void ChronogramEditorWidget::onZoomFitClicked()
{
    // Ajuste le zoom pour que [0, Tmax] occupe exactement la largeur visible du panneau droit.
    const int viewportWidth = m_rightScrollArea->viewport()->width();
    const double tmax = m_tmaxSpin->value();
    if (tmax <= 0.0 || viewportWidth <= 0)
        return;

    // On retire une petite marge pour ne pas coller le dernier repère au bord.
    const double usableWidth = qMax(10, viewportWidth - 20);
    m_multiView->setPixelsPerUnit(usableWidth / tmax);
}

void ChronogramEditorWidget::onPrecisionChanged(double precision)
{
    m_multiView->setSnapStep(precision);
}

void ChronogramEditorWidget::onCloseClicked()
{
    emit closeRequested();
}

void ChronogramEditorWidget::onCancelClicked()
{
    emit cancelRequested();
}

void ChronogramEditorWidget::onAddStimulusRequested(StimulusTrack *track, double date)
{
    if (!track)
        return;

    // Dialogue NON modal : show() plutôt qu'exec(). La validation/construction du
    // stimulus se fait dans le slot finished(), pas après un exec() bloquant -
    // évite les soucis de réactivation de fenêtre observés avec des dialogues
    // modaux imbriqués (voir historique du projet).
    auto *dlg = new StimulusEditorDialog(this, track->name(), track->type(), date);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setModal(false);

    connect(dlg, &QDialog::finished, this, [this, track, dlg](int result) {
        if (result == QDialog::Accepted) {
            track->add(dlg->buildStimulus());
            m_multiView->update();
        }
    });

    dlg->show();
}

void ChronogramEditorWidget::onEditStimulusRequested(StimulusTrack *track, int index)
{
    if (!track)
        return;

    const TrackStimulus *existing = track->at(index);
    if (!existing)
        return;

    auto *dlg = new StimulusEditorDialog(this, existing);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setModal(false);

    // Le dialogue étant non modal, la piste peut être modifiée pendant l'édition (ajouts,
    // suppressions...) : son index n'est alors plus valable. On mémorise donc l'identifiant
    // du stimulus et on retrouve son index à la validation.
    const quint64 id = existing->id();
    connect(dlg, &QDialog::finished, this, [this, track, id, dlg](int result) {
        if (result == QDialog::Accepted) {
            const int index = track->indexOfId(id);
            if (index < 0)
                return; // Le stimulus a été supprimé entre-temps
            track->replace(index, dlg->buildStimulus());
            m_multiView->update();
        }
    });

    dlg->show();
}

void ChronogramEditorWidget::onRemoveStimulusRequested(StimulusTrack *track, int index)
{
    if (!track)
        return;

    const TrackStimulus *existing = track->at(index);
    if (!existing)
        return;
    // Voir onEditStimulusRequested : l'index peut changer avant la confirmation
    const quint64 id = existing->id();

    auto *box = new QMessageBox(QMessageBox::Question, tr("Confirmer"),
                                 tr("Supprimer ce stimulus ?"),
                                 QMessageBox::Yes | QMessageBox::No, this);
    box->setAttribute(Qt::WA_DeleteOnClose);
    box->setModal(false);

    connect(box, &QMessageBox::finished, this, [this, track, id](int result) {
        if (result == QMessageBox::Yes) {
            const int index = track->indexOfId(id);
            if (index < 0)
                return; // Le stimulus a déjà été supprimé entre-temps
            track->removeAt(index);
            m_multiView->update();
        }
    });

    box->show();
}

void ChronogramEditorWidget::onPeriodicFillRequested(StimulusTrack *track, double clickedDate)
{
    if (!track || track->type() != TrackStimulusType::Event)
        return;

    auto *dlg = new QDialog(this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setModal(false);
    dlg->setWindowTitle(tr("Remplissage périodique - %1").arg(track->name()));

    auto *form = new QFormLayout(dlg);

    auto *startSpin = new QDoubleSpinBox(dlg);
    startSpin->setRange(0.0, 1e9);
    startSpin->setDecimals(3);
    startSpin->setValue(0.0);

    auto *periodSpin = new QDoubleSpinBox(dlg);
    periodSpin->setRange(0.001, 1e9);
    periodSpin->setDecimals(3);
    periodSpin->setValue(10.0);

    auto *endSpin = new QDoubleSpinBox(dlg);
    endSpin->setRange(0.0, 1e9);
    endSpin->setDecimals(3);
    endSpin->setValue(m_tmaxSpin->value());

    auto *clearCheck = new QCheckBox(tr("Effacer les événements existants avant remplissage"), dlg);
    clearCheck->setChecked(true);

    form->addRow(tr("Date de début :"), startSpin);
    form->addRow(tr("Période :"), periodSpin);
    form->addRow(tr("Date de fin :"), endSpin);
    form->addRow(clearCheck);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, dlg);
    form->addRow(buttons);
    connect(buttons, &QDialogButtonBox::accepted, dlg, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, dlg, &QDialog::reject);

    Q_UNUSED(clickedDate); // pourrait servir à pré-remplir startSpin si souhaité

    connect(dlg, &QDialog::finished, this,
            [this, track, startSpin, periodSpin, endSpin, clearCheck](int result) {
        if (result != QDialog::Accepted)
            return;

        const double start = startSpin->value();
        const double period = periodSpin->value();
        const double end = endSpin->value();
        const bool clearFirst = clearCheck->isChecked();

        if (period <= 0.0 || end < start) {
            QMessageBox::warning(this, tr("Paramètres invalides"),
                                  tr("La période doit être positive et la date de fin postérieure "
                                     "ou égale à la date de début."));
            return;
        }

        const int count = static_cast<int>(std::floor((end - start) / period)) + 1;

        auto applyFill = [this, track, start, period, count, clearFirst]() {
            if (clearFirst)
                track->clear();
            for (int i = 0; i < count; ++i)
                track->add(std::make_unique<EventStimulus>(start + i * period, track->name()));
            m_multiView->update();
        };

        if (count > 5000) {
            auto *confirmBox = new QMessageBox(
                QMessageBox::Question, tr("Confirmer"),
                tr("Cela va générer %1 événements. Continuer ?").arg(count),
                QMessageBox::Yes | QMessageBox::No, this);
            confirmBox->setAttribute(Qt::WA_DeleteOnClose);
            confirmBox->setModal(false);
            connect(confirmBox, &QMessageBox::finished, this, [applyFill](int r) {
                if (r == QMessageBox::Yes)
                    applyFill();
            });
            confirmBox->show();
        } else {
            applyFill();
        }
    });

    dlg->show();
}
