#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QByteArray>
#include <QCloseEvent>
#include <QMainWindow>
#include <QSettings>
#include <QTimer>

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
    QSettings       *settings;

    Port            *led_serial;
    QTimer          *screenshot_timer;
    bool            isTimerEnabled;
    uint16_t        dev_id;
    uint16_t        width_nLed;
    uint16_t        height_nLed;
    int             timer_delay_ms;

//    PortSettings    portSettings;

public slots:
    void parseAnswer(QByteArray ba);
    void serialPortConnected(bool isConnected);

signals:
    void updateLeds(QByteArray);
    void openPort();
    void closePort();
    void setPortSettings(PortSettings);

private slots:
    void connectSerialPortClicked();
    void getId();
    void getParams();
    void loadSettings();
    void makeScreenShot();
    void on_serialSettingsButton_clicked();
    void timerButtonClicked();
    void writeSettings();
};

#endif // MAINWINDOW_H
