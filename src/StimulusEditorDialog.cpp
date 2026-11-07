#include "StimulusEditorDialog.h"

#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QVBoxLayout>

StimulusEditorDialog::StimulusEditorDialog(QWidget *parent, const QString &trackName,
                                            TrackStimulusType type, double initialDate)
    : QDialog(parent)
    , m_trackName(trackName)
    , m_type(type)
{
    setupUi();
    setWindowTitle(tr("Nouveau stimulus - %1").arg(trackName));
    m_dateSpin->setValue(initialDate);
}

StimulusEditorDialog::StimulusEditorDialog(QWidget *parent, const TrackStimulus *existing)
    : QDialog(parent)
{
    Q_ASSERT(existing);
    m_type = existing->type();
    m_trackName = (m_type == TrackStimulusType::Event)
        ? static_cast<const EventStimulus *>(existing)->eventName()
        : static_cast<const ValueChangeStimulus *>(existing)->targetName();

    setupUi();
    setWindowTitle(tr("Modifier le stimulus - %1").arg(m_trackName));
    m_dateSpin->setValue(existing->date());

    if (m_type == TrackStimulusType::ValueChange) {
        const auto *vc = static_cast<const ValueChangeStimulus *>(existing);
        m_valueSpin->setValue(vc->value().toInt());
    }
}

void StimulusEditorDialog::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);
    auto *form = new QFormLayout();

    m_dateSpin = new QDoubleSpinBox(this);
    m_dateSpin->setRange(0.0, 1e9);
    m_dateSpin->setDecimals(3);
    form->addRow(tr("Date :"), m_dateSpin);

    if (m_type == TrackStimulusType::ValueChange) {
        m_valueSpin = new QSpinBox(this);
        m_valueSpin->setRange(-1000000000, 1000000000);
        form->addRow(tr("Valeur :"), m_valueSpin);
    }

    mainLayout->addLayout(form);

    m_buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    mainLayout->addWidget(m_buttons);

    connect(m_buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(m_buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    setMinimumWidth(240);
    adjustSize();
}

std::unique_ptr<TrackStimulus> StimulusEditorDialog::buildStimulus() const
{
    const double date = m_dateSpin->value();

    if (m_type == TrackStimulusType::Event)
        return std::make_unique<EventStimulus>(date, m_trackName);

    return std::make_unique<ValueChangeStimulus>(date, m_trackName, QVariant(m_valueSpin->value()));
}
