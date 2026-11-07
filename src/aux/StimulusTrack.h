#ifndef STIMULUSTRACK_H
#define STIMULUSTRACK_H

#include <QString>
#include <QVariant>
#include <memory>
#include <vector>
#include "TrackStimulus.h"
#include "iov.h"

/**
 * @brief Une piste ("signal") regroupant tous les stimuli portant le même nom
 *        (nom d'événement ou nom de cible) et le même type.
 */
class StimulusTrack
{
public:
    StimulusTrack(const QString &name, TrackStimulusType type)
      : m_name(name), m_type(type), m_io(nullptr) {}
    StimulusTrack(Input* io);

    QString name() const { return m_name; }
    void setName(const QString &name) { m_name = name; }

    TrackStimulusType type() const { return m_type; }
    void setType(TrackStimulusType type) { m_type = type; }

    Input* io() const { return m_io; }

    /// Supprime tous les stimuli de la piste (utile avant un changement de type)
    void clear() { m_stimuli.clear(); }

    int count() const { return static_cast<int>(m_stimuli.size()); }
    const TrackStimulus *at(int index) const;

    /// Ajoute un stimulus à la piste, garde la liste triée par date
    void add(std::unique_ptr<TrackStimulus> stimulus);

    /// Remplace le stimulus à l'index donné (édition), garde le tri par date
    void replace(int index, std::unique_ptr<TrackStimulus> stimulus);

    void removeAt(int index);

    /// Index du stimulus le plus proche de 'date' à 'tolerance' près (-1 si aucun)
    int indexNear(double date, double tolerance) const;

    /// Bornes des valeurs numériques de la piste (pistes ValueChange). false si non numérique.
    bool numericRange(double &minValue, double &maxValue) const;

    double lastDate() const;

private:
    QString m_name;
    TrackStimulusType m_type;
    Input *m_io;
    std::vector<std::unique_ptr<TrackStimulus>> m_stimuli; // triés par date
};

#endif // STIMULUSTRACK_H
