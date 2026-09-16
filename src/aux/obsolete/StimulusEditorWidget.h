#ifndef STIMULUSEDITORWIDGET_H
#define STIMULUSEDITORWIDGET_H

#include <QWidget>
#include <QModelIndex>

class QTableView;
class QPushButton;
class StimulusTableModel;

/**
 * @brief Widget complet de gestion des stimuli : tableau + boutons Ajouter/Modifier/Supprimer.
 *
 * À insérer dans une fenêtre ou un onglet de l'application. Récupérez la liste
 * finale des stimuli via model()->takeAll() (ou stimulusAt()/rowCount() pour
 * un accès en lecture, par ex. pour peupler votre StimulusManager runtime).
 */
class StimulusEditorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit StimulusEditorWidget(QWidget *parent = nullptr);

    /// Accès au modèle sous-jacent (pour pré-remplir ou exporter les stimuli)
    StimulusTableModel *model() const { return m_model; }

private slots:
    void onAddClicked();
    void onEditClicked();
    void onRemoveClicked();
    void onSelectionChanged();
    void onRowDoubleClicked(const QModelIndex &index);

private:
    void setupUi();

    StimulusTableModel *m_model = nullptr;
    QTableView *m_tableView = nullptr;
    QPushButton *m_addButton = nullptr;
    QPushButton *m_editButton = nullptr;
    QPushButton *m_removeButton = nullptr;
};

#endif // STIMULUSEDITORWIDGET_H
