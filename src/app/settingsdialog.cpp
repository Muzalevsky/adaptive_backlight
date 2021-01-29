#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QDebug>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    ui->parityCombo->setCurrentIndex(0);
    ui->baudCombo->setCurrentText(QString::number(m_settings.baudRate));
    ui->dataBitsCombo->setCurrentText(QString::number(m_settings.dataBits));
    ui->stopBitsCombo->setCurrentText(QString::number(m_settings.stopBits));
    ui->FlowCombo->setCurrentText(QString::number(m_settings.flowControl));
    ui->timeoutSpinner->setValue(m_settings.responseTime);
    ui->retriesSpinner->setValue(m_settings.numberOfRetries);

    ui->portNameCombo->clear();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
        ui->portNameCombo->addItem(info.portName());

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

PortSettings SettingsDialog::settings() const
{
    return m_settings;
}

void SettingsDialog::setBaud( qint32 baud )
{
    int index = ui->baudCombo->findText( QString::number(baud) );
    qDebug() << "index of current baud" << index << "baud:" << baud;
    ui->baudCombo->setCurrentIndex( index );
    m_settings.baudRate = baud;
}

void SettingsDialog::setName(QString name)
{
    int index = ui->portNameCombo->findText(name);
    qDebug() << "index of current port" << index << "name:" << name;
    ui->portNameCombo->setCurrentIndex(index);
    m_settings.name = name;
}
