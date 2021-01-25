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
    SettingsDialog  *m_settingsDialog;


private:
    Ui::MainWindow  *ui;
    QPixmap         pixmap;
    Port            *led_serial;
    bool            _debug;
    QSettings           *settings;

    void closeEvent(QCloseEvent *event);

public slots:
    void makeScreenShot();
signals:
    void updateLeds(QByteArray);
private slots:
    void loadSettings();
    void writeSettings();
    void openSerialPort();
    void closeSerialPort();


    void on_serialSettingsButton_released();
};

#endif // MAINWINDOW_H
