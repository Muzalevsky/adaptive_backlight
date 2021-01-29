#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

#include <port.h>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SettingsDialog(PortSettings& settings, QWidget *parent = nullptr);
    ~SettingsDialog();

    PortSettings& m_settings;
private:
    Ui::SettingsDialog *ui;

};

#endif // SETTINGSDIALOG_H
