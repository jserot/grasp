#ifndef STIMULUSEDITORDIALOG_H
#define STIMULUSEDITORDIALOG_H

#include <QDialog>
#include <memory>
#include "TrackStimulus.h"

class QDoubleSpinBox;
class QSpinBox;
class QDialogButtonBox;

/**
 * @brief Boîte de dialogue d'ajout ou d'édition d'un stimulus sur une piste
 *        dont le nom et le type sont déjà déterminés (par la piste cliquée
 *        dans le chronogramme). Ne construit que les champs réellement
 *        nécessaires selon le type :
 *          - piste Événement  : uniquement la date
 *          - piste ValueChange : la date et la valeur (entière)
 *
 * Utilisation :
 * @code
 *   // Ajout
 *   StimulusEditorDialog dlg(parent, trackName, trackType, initialDate);
 *   // Édition d'un stimulus existant (nom/type repris de "existing")
 *   StimulusEditorDialog dlg(parent, existing);
 *
 *   connect(&dlg, &QDialog::finished, ..., [&dlg](int result) {
 *       if (result == QDialog::Accepted)
 *           auto stim = dlg.buildStimulus();
 *   });
 * @endcode
 */
class StimulusEditorDialog : public QDialog
{
    Q_OBJECT
public:
    /// Ajout d'un nouveau stimulus sur la piste "trackName" (type déjà connu)
    StimulusEditorDialog(QWidget *parent, const QString &trackName, TrackStimulusType type, double initialDate);

    /// Édition d'un stimulus existant (nom et type repris de "existing", non modifiables)
    StimulusEditorDialog(QWidget *parent, const TrackStimulus *existing);

    /// Construit le stimulus correspondant aux champs actuels (après exec()/finished() == Accepted)
    std::unique_ptr<TrackStimulus> buildStimulus() const;

private:
    void setupUi();

    QString m_trackName;
    TrackStimulusType m_type = TrackStimulusType::Event;

    QDoubleSpinBox *m_dateSpin = nullptr;
    QSpinBox *m_valueSpin = nullptr; // uniquement créé pour une piste ValueChange
    QDialogButtonBox *m_buttons = nullptr;
};

#endif // STIMULUSEDITORDIALOG_H
