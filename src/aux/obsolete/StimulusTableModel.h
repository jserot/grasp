#ifndef STIMULUSTABLEMODEL_H
#define STIMULUSTABLEMODEL_H

#include <QAbstractTableModel>
#include <memory>
#include <vector>
#include "TrackStimulus.h"

/**
 * @brief Modèle Qt exposant une liste de stimuli à un QTableView.
 *
 * Le modèle est propriétaire des stimuli qu'il contient (std::unique_ptr).
 * Colonnes : Date | Type | Cible / Événement | Valeur / Paramètre
 */
class StimulusTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Column {
        ColDate = 0,
        ColType,
        ColName,   // nom de l'événement ou de la cible
        ColValue,  // valeur ou paramètre
        ColumnCount
    };

    explicit StimulusTableModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    /// Ajoute un stimulus (le modèle en prend la propriété), la liste reste triée par date
    void addStimulus(std::unique_ptr<TrackStimulus> stimulus);

    /// Remplace le stimulus à la ligne donnée par un nouveau (édition)
    void replaceStimulus(int row, std::unique_ptr<TrackStimulus> stimulus);

    /// Supprime le stimulus à la ligne donnée
    void removeStimulus(int row);

    /// Accès en lecture au stimulus d'une ligne (nullptr si index invalide)
    const TrackStimulus *stimulusAt(int row) const;

    /// Vide entièrement le modèle
    void clear();

    /// Export : transfère la propriété de tous les stimuli (vide le modèle)
    std::vector<std::unique_ptr<TrackStimulus>> takeAll();

private:
    int indexForDate(double date) const;

    std::vector<std::unique_ptr<TrackStimulus>> m_stimuli; // toujours triés par date
};

#endif // STIMULUSTABLEMODEL_H
