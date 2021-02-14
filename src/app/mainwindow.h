#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QByteArray>
#include <QCloseEvent>
#include <QMainWindow>
#include <QSettings>
#include <QTimer>

#include <controllerprotocol.h>
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
    void                closeEvent(QCloseEvent *event);

    Ui::MainWindow      *ui;
    ControllerProtocol  *controllerProto;
    QSettings           *settings;
    Port                *led_serial;
    QTimer              *screenshot_timer;

    bool                isTimerEnabled;
    uint16_t            dev_id;
    uint16_t            width_nLed;
    uint16_t            height_nLed;
    int                 timer_delay_ms;


public slots:
    void serialPortConnected(bool isConnected);

signals:
    void closePort();
    void openPort();
    void setPortSettings(PortSettings);
    void writeToPort(QByteArray);

private slots:
    void assignNewBrightness(int br);
    void assignNewId(int id);
    void assignNewLedNumber(int led_per_side);

    void connectSerialPortClicked();
    void requestDeviceId();
    void requestDeviceParams();
    void loadSettings();
    void makeScreenShot();
    void on_serialSettingsButton_clicked();
    void timerButtonClicked();
    void writeSettings();
};

#endif // MAINWINDOW_H
