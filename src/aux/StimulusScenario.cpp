#include "StimulusScenario.h"
#include <algorithm>
#include "../iov.h"

StimulusTrack *StimulusScenario::addTrack(const QString &name, TrackStimulusType type)
{
    if (track(name) != nullptr)
        return nullptr; // nom déjà utilisé
    m_tracks.push_back(std::make_unique<StimulusTrack>(name, type));
    return m_tracks.back().get();
}

void StimulusScenario::addTrack(std::unique_ptr<StimulusTrack> track)
{
    m_tracks.push_back(std::move(track));
}

void StimulusScenario::removeTrack(const QString &name)
{
    auto it = std::find_if(m_tracks.begin(), m_tracks.end(),
                            [&name](const std::unique_ptr<StimulusTrack> &t) {
                                return t->name() == name;
                            });
    if (it != m_tracks.end())
        m_tracks.erase(it);
}

StimulusTrack *StimulusScenario::track(const QString &name)
{
    for (auto &t : m_tracks) {
        if (t->name() == name)
            return t.get();
    }
    return nullptr;
}

const StimulusTrack *StimulusScenario::track(const QString &name) const
{
    for (const auto &t : m_tracks) {
        if (t->name() == name)
            return t.get();
    }
    return nullptr;
}

bool StimulusScenario::isNameUsed(const QString &name, const StimulusTrack *excluding) const
{
    for (const auto &t : m_tracks) {
        if (t.get() != excluding && t->name() == name)
            return true;
    }
    return false;
}

QVector<StimulusTrack *> StimulusScenario::tracks()
{
    QVector<StimulusTrack *> result;
    result.reserve(static_cast<int>(m_tracks.size()));
    for (auto &t : m_tracks)
        result.push_back(t.get());
    return result;
}

QVector<const StimulusTrack *> StimulusScenario::tracks() const
{
    QVector<const StimulusTrack *> result;
    result.reserve(static_cast<int>(m_tracks.size()));
    for (const auto &t : m_tracks)
        result.push_back(t.get());
    return result;
}

double StimulusScenario::maxDate() const
{
    double m = 0.0;
    for (const auto &t : m_tracks)
        m = std::max(m, t->lastDate());
    return m;
}
