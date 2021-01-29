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

    Port            *led_serial;

    uint16_t        nLed;
    uint16_t        dev_id;
    uint16_t        width_nLed;
    uint16_t        height_nLed;
    int             timer_delay_ms;

public slots:
    void makeScreenShot();
signals:
    void updateLeds(QByteArray);
private slots:
    void loadSettings();
    void writeSettings();
    void connectSerialPortClicked();
    void closeSerialPort();

    void on_serialSettingsButton_clicked();
};

#endif // MAINWINDOW_H
