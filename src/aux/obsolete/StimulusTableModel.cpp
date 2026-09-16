#include "StimulusTableModel.h"

StimulusTableModel::StimulusTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int StimulusTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return static_cast<int>(m_stimuli.size());
}

int StimulusTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return ColumnCount;
}

QVariant StimulusTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= static_cast<int>(m_stimuli.size()))
        return QVariant();

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    const TrackStimulus *stim = m_stimuli[static_cast<size_t>(index.row())].get();

    switch (index.column()) {
    case ColDate:
        return stim->date();
    case ColType:
        return stim->type() == TrackStimulusType::Event ? tr("Événement") : tr("Changement de valeur");
    case ColName:
        if (stim->type() == TrackStimulusType::Event)
            return static_cast<const EventStimulus *>(stim)->eventName();
        else
            return static_cast<const ValueChangeStimulus *>(stim)->targetName();
    case ColValue:
        if (stim->type() == TrackStimulusType::Event)
            return static_cast<const EventStimulus *>(stim)->param();
        else
            return static_cast<const ValueChangeStimulus *>(stim)->value();
    default:
        return QVariant();
    }
}

QVariant StimulusTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
        return QVariant();

    switch (section) {
    case ColDate:  return tr("Date");
    case ColType:  return tr("Type");
    case ColName:  return tr("Nom");
    case ColValue: return tr("Valeur");
    default:       return QVariant();
    }
}

int StimulusTableModel::indexForDate(double date) const
{
    // Premier index dont la date est >= date (recherche linéaire ; suffisant
    // pour un scénario de quelques dizaines/centaines de stimuli édité en GUI)
    int i = 0;
    for (; i < static_cast<int>(m_stimuli.size()); ++i) {
        if (m_stimuli[static_cast<size_t>(i)]->date() >= date)
            break;
    }
    return i;
}

void StimulusTableModel::addStimulus(std::unique_ptr<TrackStimulus> stimulus)
{
    const int row = indexForDate(stimulus->date());
    beginInsertRows(QModelIndex(), row, row);
    m_stimuli.insert(m_stimuli.begin() + row, std::move(stimulus));
    endInsertRows();
}

void StimulusTableModel::replaceStimulus(int row, std::unique_ptr<TrackStimulus> stimulus)
{
    if (row < 0 || row >= static_cast<int>(m_stimuli.size()))
        return;

    // Retrait puis réinsertion au bon endroit (la date a pu changer lors de l'édition)
    beginRemoveRows(QModelIndex(), row, row);
    m_stimuli.erase(m_stimuli.begin() + row);
    endRemoveRows();

    addStimulus(std::move(stimulus));
}

void StimulusTableModel::removeStimulus(int row)
{
    if (row < 0 || row >= static_cast<int>(m_stimuli.size()))
        return;

    beginRemoveRows(QModelIndex(), row, row);
    m_stimuli.erase(m_stimuli.begin() + row);
    endRemoveRows();
}

const TrackStimulus *StimulusTableModel::stimulusAt(int row) const
{
    if (row < 0 || row >= static_cast<int>(m_stimuli.size()))
        return nullptr;
    return m_stimuli[static_cast<size_t>(row)].get();
}

void StimulusTableModel::clear()
{
    beginResetModel();
    m_stimuli.clear();
    endResetModel();
}

std::vector<std::unique_ptr<TrackStimulus>> StimulusTableModel::takeAll()
{
    beginResetModel();
    std::vector<std::unique_ptr<TrackStimulus>> result = std::move(m_stimuli);
    m_stimuli.clear();
    endResetModel();
    return result;
}
