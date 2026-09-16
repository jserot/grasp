#include "StimulusEditorWidget.h"
#include "StimulusTableModel.h"
#include "StimulusEditorDialog.h"

#include <QTableView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QItemSelectionModel>

StimulusEditorWidget::StimulusEditorWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
}

void StimulusEditorWidget::setupUi()
{
    m_model = new StimulusTableModel(this);

    m_tableView = new QTableView(this);
    m_tableView->setModel(m_model);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers); // édition via le dialogue
    m_tableView->horizontalHeader()->setStretchLastSection(true);
    m_tableView->verticalHeader()->setVisible(false);

    m_addButton = new QPushButton(tr("Ajouter..."), this);
    m_editButton = new QPushButton(tr("Modifier..."), this);
    m_removeButton = new QPushButton(tr("Supprimer"), this);
    m_editButton->setEnabled(false);
    m_removeButton->setEnabled(false);

    auto *buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(m_addButton);
    buttonsLayout->addWidget(m_editButton);
    buttonsLayout->addWidget(m_removeButton);
    buttonsLayout->addStretch();

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_tableView);
    mainLayout->addLayout(buttonsLayout);

    connect(m_addButton, &QPushButton::clicked, this, &StimulusEditorWidget::onAddClicked);
    connect(m_editButton, &QPushButton::clicked, this, &StimulusEditorWidget::onEditClicked);
    connect(m_removeButton, &QPushButton::clicked, this, &StimulusEditorWidget::onRemoveClicked);
    connect(m_tableView, &QTableView::doubleClicked, this, &StimulusEditorWidget::onRowDoubleClicked);
    connect(m_tableView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &StimulusEditorWidget::onSelectionChanged);
}

void StimulusEditorWidget::onAddClicked()
{
    StimulusEditorDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted)
        m_model->addStimulus(dlg.buildStimulus());
}

void StimulusEditorWidget::onEditClicked()
{
    const int row = m_tableView->currentIndex().row();
    const TrackStimulus *existing = m_model->stimulusAt(row);
    if (!existing)
        return;

    StimulusEditorDialog dlg(this, existing);
    if (dlg.exec() == QDialog::Accepted)
        m_model->replaceStimulus(row, dlg.buildStimulus());
}

void StimulusEditorWidget::onRemoveClicked()
{
    const int row = m_tableView->currentIndex().row();
    if (row < 0)
        return;

    if (QMessageBox::question(this, tr("Confirmer"), tr("Supprimer ce stimulus ?"))
        == QMessageBox::Yes) {
        m_model->removeStimulus(row);
    }
}

void StimulusEditorWidget::onSelectionChanged()
{
    const bool hasSelection = m_tableView->currentIndex().isValid();
    m_editButton->setEnabled(hasSelection);
    m_removeButton->setEnabled(hasSelection);
}

void StimulusEditorWidget::onRowDoubleClicked(const QModelIndex &index)
{
    if (index.isValid())
        onEditClicked();
}
