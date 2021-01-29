#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(PortSettings& settings, QWidget *parent) :
    QDialog(parent),
    m_settings(settings),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    ui->portNameCombo->clear();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
        ui->portNameCombo->addItem(info.portName());

    ui->parityCombo->setCurrentIndex(0);
    ui->baudCombo->setCurrentText(QString::number(m_settings.baudRate));
    ui->dataBitsCombo->setCurrentText(QString::number(m_settings.dataBits));
    ui->stopBitsCombo->setCurrentText(QString::number(m_settings.stopBits));
    ui->FlowCombo->setCurrentText(QString::number(m_settings.flowControl));
    ui->timeoutSpinner->setValue(m_settings.responseTime);
    ui->retriesSpinner->setValue(m_settings.numberOfRetries);

    connect(ui->applyButton, &QPushButton::clicked, [this]() {
        m_settings.parity = ui->parityCombo->currentIndex();
        if (m_settings.parity > 0)
            m_settings.parity++;
        m_settings.baudRate = ui->baudCombo->currentText().toInt();
        m_settings.dataBits = ui->dataBitsCombo->currentText().toInt();
        m_settings.stopBits = ui->stopBitsCombo->currentText().toInt();
        m_settings.responseTime = ui->timeoutSpinner->value();
        m_settings.numberOfRetries = ui->retriesSpinner->value();
        m_settings.flowControl = ui->FlowCombo->currentIndex();
        m_settings.name = ui->portNameCombo->currentText();
        hide();
    });
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}
