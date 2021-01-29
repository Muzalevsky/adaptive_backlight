#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QByteArray>
#include <QCloseEvent>
#include <QSettings>

#include <port.h>
#include <settingsdialog.h>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void            closeEvent(QCloseEvent *event);

    Ui::MainWindow  *ui;
    QPixmap         pixmap;
    QSettings       *settings;

    SettingsDialog  *m_settingsDialog;
    Port            *led_serial;
    bool            _debug;

public slots:
    void makeScreenShot();
signals:
    void updateLeds(QByteArray);
private slots:
    void loadSettings();
    void writeSettings();
    void connectSerialPortClicked();
    void closeSerialPort();

    void on_serialSettingsButton_released() { m_settingsDialog->show(); }
};

#endif // MAINWINDOW_H
