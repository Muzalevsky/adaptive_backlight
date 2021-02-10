#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QByteArray>
#include <QCloseEvent>
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
    QPixmap         pixmap;
    QSettings       *settings;

    Port            *led_serial;
    QTimer          *screenshot_timer;
    uint16_t        nLed;
    uint16_t        dev_id;
    uint16_t        width_nLed;
    uint16_t        height_nLed;
    int             timer_delay_ms;

//    PortSettings    portSettings;

public slots:
    void makeScreenShot();
    void parseAnswer(QByteArray ba);
signals:
    void updateLeds(QByteArray);
    void openPort();
    void closePort();
//    void setPortSettings(PortSettings&);

private slots:
    void startScreenshotTimer();
    void stopScreenshotTimer();

    void getId();
    void getParams();
    void loadSettings();
    void writeSettings();
    void connectSerialPortClicked();

    void on_serialSettingsButton_clicked();
};

#endif // MAINWINDOW_H
