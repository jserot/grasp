#ifndef TRACK_STIMULUS_H
#define TRACK_STIMULUS_H

#include <QString>
#include <QVariant>

/**
 * @brief Type de stimulus : un événement ponctuel ou un changement de valeur.
 */
enum class TrackStimulusType {
    Event,
    ValueChange
};

/**
 * @brief Classe abstraite représentant un stimulus déclenché à une date donnée.
 *
 * Un stimulus est une action programmée dans le temps de simulation :
 * soit un événement nommé (ex: "clic"), soit un changement de valeur
 * d'une variable du système simulé (ex: x = 10 à t = 100).
 *
 * Cette classe ne fait qu'encapsuler des données ; le déclenchement effectif
 * (émission de signaux, etc.) est géré par StimulusManager.
 */
class TrackStimulus
{
public:
    explicit TrackStimulus(double date) : m_date(date), m_id(nextId()) {}
    virtual ~TrackStimulus() = default;

    double date() const { return m_date; }

    /// Identifiant unique du stimulus, stable tant qu'il existe (contrairement à son
    /// index dans la piste, qui change quand on ajoute ou supprime d'autres stimuli).
    /// Sert aux dialogues non modaux pour retrouver le stimulus à la validation.
    quint64 id() const { return m_id; }
    virtual TrackStimulusType type() const = 0;

    /// Description lisible, utile pour le debug ou l'affichage dans l'UI
    virtual QString description() const = 0;

    bool operator<(const TrackStimulus &other) const { return m_date < other.m_date; }

protected:
    double m_date;

private:
    quint64 m_id;
    static quint64 nextId() { static quint64 n = 0; return ++n; }
};

/**
 * @brief TrackStimulus représentant un événement ponctuel (ex: clic, appui touche...)
 */
class EventStimulus : public TrackStimulus
{
public:
    EventStimulus(double date, const QString &eventName, const QVariant &param = QVariant())
        : TrackStimulus(date), m_eventName(eventName), m_param(param) {}

    TrackStimulusType type() const override { return TrackStimulusType::Event; }
    QString eventName() const { return m_eventName; }
    QVariant param() const { return m_param; }

    QString description() const override
    {
        return QString("[t=%1] Événement '%2'%3")
            .arg(m_date)
            .arg(m_eventName)
            .arg(m_param.isValid() ? QString(" (param=%1)").arg(m_param.toString()) : QString());
    }

private:
    QString m_eventName;
    QVariant m_param; // paramètre optionnel associé à l'événement
};

/**
 * @brief TrackStimulus représentant le changement de valeur d'une variable du système simulé.
 */
class ValueChangeStimulus : public TrackStimulus
{
public:
    ValueChangeStimulus(double date, const QString &targetName, const QVariant &value)
        : TrackStimulus(date), m_targetName(targetName), m_value(value) {}

    TrackStimulusType type() const override { return TrackStimulusType::ValueChange; }
    QString targetName() const { return m_targetName; }
    QVariant value() const { return m_value; }

    QString description() const override
    {
        return QString("[t=%1] %2 <- %3").arg(m_date).arg(m_targetName).arg(m_value.toString());
    }

private:
    QString m_targetName; // nom de la variable/entité ciblée
    QVariant m_value;     // nouvelle valeur
};

#endif // TRACK_STIMULUS_H
