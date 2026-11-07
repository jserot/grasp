#ifndef STIMULUSSCENARIO_H
#define STIMULUSSCENARIO_H

#include <QVector>
#include <QString>
#include <memory>
#include <vector>
#include "StimulusTrack.h"
#include "iov.h"

/**
 * @brief Ensemble de pistes (signaux) formant un scénario de stimuli complet.
 *
 * Note : le stockage interne utilise std::vector (et non QVector/QList) car
 * ces derniers exigent un type copiable pour leurs opérations internes de
 * réallocation, ce que std::unique_ptr<StimulusTrack> n'est pas.
 */
class StimulusScenario
{
public:
    StimulusTrack *addTrack(const QString &name, TrackStimulusType type); // Add an empty track
    void addTrack(std::unique_ptr<StimulusTrack> track);

    void removeTrack(const QString &name);

    StimulusTrack *track(const QString &name);
    const StimulusTrack *track(const QString &name) const;

    /// true si 'name' est déjà utilisé par une piste autre que 'excluding'
    bool isNameUsed(const QString &name, const StimulusTrack *excluding = nullptr) const;

    QVector<StimulusTrack *> tracks();
    QVector<const StimulusTrack *> tracks() const;

    /// Date maximale sur l'ensemble des pistes (utile pour dimensionner le chronogramme)
    double maxDate() const;

private:
    std::vector<std::unique_ptr<StimulusTrack>> m_tracks;
};

#endif // STIMULUSSCENARIO_H
