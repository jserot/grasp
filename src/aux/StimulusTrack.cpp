#include "StimulusTrack.h"
#include <algorithm>
#include <limits>
#include <cmath>

StimulusTrack::StimulusTrack(Input* io)
{
  m_io = io;
  Stimulus st = io->stim;
  qDebug() << "StimulusTrack: adding" << st.toString() << "to track" << io->name;
  switch ( st.kind ) {
  case Stimulus::None: return;
  case Stimulus::Periodic:
    m_type = TrackStimulusType::Event;
    for ( int t=st.desc.periodic.start_time; t<=st.desc.periodic.end_time; t += st.desc.periodic.period ) 
      add(std::make_unique<EventStimulus>(t, io->name));
    break;
  case Stimulus::Sporadic:
    m_type = TrackStimulusType::Event;
    for ( int t: st.desc.sporadic.dates )
      add(std::make_unique<EventStimulus>(t, io->name));
    break;
  case Stimulus::ValueChanges: 
    m_type = TrackStimulusType::ValueChange;
    for ( const QPair<int,int> &vc: st.desc.valueChanges.vcs )
       add(std::make_unique<ValueChangeStimulus>(vc.first, io->name, vc.second));
    break;
  }
}

const TrackStimulus *StimulusTrack::at(int index) const
{
    if (index < 0 || index >= static_cast<int>(m_stimuli.size()))
        return nullptr;
    return m_stimuli[static_cast<size_t>(index)].get();
}

void StimulusTrack::add(std::unique_ptr<TrackStimulus> stimulus)
{
    const double date = stimulus->date();
    auto it = std::upper_bound(m_stimuli.begin(), m_stimuli.end(), date,
                                [](double d, const std::unique_ptr<TrackStimulus> &s) {
                                    return d < s->date();
                                });
    m_stimuli.insert(it, std::move(stimulus));
}

void StimulusTrack::replace(int index, std::unique_ptr<TrackStimulus> stimulus)
{
    if (index < 0 || index >= static_cast<int>(m_stimuli.size()))
        return;
    m_stimuli.erase(m_stimuli.begin() + index);
    add(std::move(stimulus));
}

void StimulusTrack::removeAt(int index)
{
    if (index < 0 || index >= static_cast<int>(m_stimuli.size()))
        return;
    m_stimuli.erase(m_stimuli.begin() + index);
}

int StimulusTrack::indexNear(double date, double tolerance) const
{
    int best = -1;
    double bestDist = std::numeric_limits<double>::max();
    for (int i = 0; i < static_cast<int>(m_stimuli.size()); ++i) {
        const double dist = std::fabs(m_stimuli[static_cast<size_t>(i)]->date() - date);
        if (dist <= tolerance && dist < bestDist) {
            bestDist = dist;
            best = i;
        }
    }
    return best;
}

bool StimulusTrack::numericRange(double &minValue, double &maxValue) const
{
    bool found = false;
    minValue = std::numeric_limits<double>::max();
    maxValue = std::numeric_limits<double>::lowest();

    for (const auto &s : m_stimuli) {
        if (s->type() != TrackStimulusType::ValueChange)
            continue;
        const auto *vc = static_cast<const ValueChangeStimulus *>(s.get());
        bool ok = false;
        const double v = vc->value().toDouble(&ok);
        if (!ok)
            continue;
        found = true;
        minValue = std::min(minValue, v);
        maxValue = std::max(maxValue, v);
    }
    return found;
}

double StimulusTrack::lastDate() const
{
    return m_stimuli.empty() ? 0.0 : m_stimuli.back()->date();
}
