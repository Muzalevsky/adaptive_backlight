#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QScreen>
#include <QWindow>
//#include <QtSerialPort/QSerialPort>
//#include <QRgb>
#include <QThread>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    settings = new QSettings( "settings.ini", QSettings::IniFormat );

    /*
     * Custom class for serial port. Let's try to place and handle in another thread
     */
    led_serial = new Port();
    QThread *threadLed = new QThread;
    led_serial->moveToThread(threadLed);
    connect(led_serial, SIGNAL(finished_Port()), threadLed, SLOT(deleteLater()));
    connect(led_serial, SIGNAL(finished_Port()), threadLed, SLOT(quit()));
    connect(threadLed, SIGNAL(started()), led_serial, SLOT(process_Port()));//Переназначения метода run
    connect(threadLed, SIGNAL(finished()), led_serial, SLOT(deleteLater()));//Удалить к чертям поток
    threadLed->start();

    connect( this, &MainWindow::updateLeds, led_serial, &Port::WriteToPort );
    connect(ui->openPortButton, &QPushButton::clicked, this, &MainWindow::connectSerialPortClicked);

    loadSettings();

    /*
     * Timer for screen capturing and picture refreshing
     */
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::makeScreenShot);
    timer->start(timer_delay_ms);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::makeScreenShot()
{
    QScreen *screen = QGuiApplication::primaryScreen();
    if (const QWindow *window = windowHandle())
        screen = window->screen();

    if (!screen)
        return;

    if (!nLed)
        return;

    int px_per_led = screen->size().width() / nLed;
    qDebug() << "Using " << px_per_led  << "px_per_led";

    /*
     * Start analyzing screenshot
     */
    pixmap = screen->grabWindow(0);

    /*
     * Here we create message for the LED controller in different cycles
     * for different sides of the screen
     */
    QByteArray ba;

    /* Top side */
    for (uint16_t led_idx = 0; led_idx < width_nLed; led_idx++) {
        QPixmap pixmap_per_led = pixmap.copy(px_per_led * led_idx, 0, px_per_led, px_per_led);
        QImage img_per_led = pixmap_per_led.toImage();

        // bilinear filtration
        QImage average_px = img_per_led.scaled(1,1,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
        QColor color = QColor::fromRgba(average_px.pixel(0,0));
//        qDebug() << "color" << color;

        ba.append(led_idx);
        ba.append(color.rgb());
    }

    /* Right side */
    for (uint16_t led_idx = width_nLed; led_idx < width_nLed + height_nLed; led_idx++) {
        QPixmap pixmap_per_led = pixmap.copy( pixmap.width() - px_per_led, px_per_led * led_idx, px_per_led, px_per_led);
        QImage img_per_led = pixmap_per_led.toImage();

        // bilinear filtration
        QImage average_px = img_per_led.scaled(1,1,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
        QColor color = QColor::fromRgba(average_px.pixel(0,0));
//        qDebug() << "color" << color;

        ba.append(led_idx);
        ba.append(color.rgb());
    }

    /* Bottom side */
    for (uint16_t led_idx = width_nLed + height_nLed; led_idx < (2 * width_nLed + height_nLed); led_idx++) {
        QPixmap pixmap_per_led = pixmap.copy( pixmap.width() - px_per_led * led_idx, pixmap.height() - px_per_led, px_per_led, px_per_led);
        QImage img_per_led = pixmap_per_led.toImage();

        // bilinear filtration
        QImage average_px = img_per_led.scaled(1,1,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
        QColor color = QColor::fromRgba(average_px.pixel(0,0));
//        qDebug() << "color" << color;

        ba.append(led_idx);
        ba.append(color.rgb());
    }

    /* Left side */
    for (uint16_t led_idx = (2 * width_nLed + height_nLed); led_idx < (2 * (width_nLed + height_nLed)); led_idx++) {
        QPixmap pixmap_per_led = pixmap.copy( 0, pixmap.height() - px_per_led * led_idx, px_per_led, px_per_led);
        QImage img_per_led = pixmap_per_led.toImage();

        // bilinear filtration
        QImage average_px = img_per_led.scaled(1,1,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
        QColor color = QColor::fromRgba(average_px.pixel(0,0));
//        qDebug() << "color" << color;

        ba.append(led_idx);
        ba.append(color.rgb());
    }

    emit updateLeds(ba);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    event->accept();
}

void MainWindow::writeSettings()
{
    settings->setValue("app/pos", pos());
    settings->setValue("app/size", size());
    settings->setValue("app/timer_step", timer_delay_ms);

    settings->setValue("led_params/numberOfDiods", nLed);
    settings->setValue("led_params/widthInLed", width_nLed);
    settings->setValue("led_params/heightInLed", height_nLed);

    settings->setValue("led_controller/port", led_serial->portSettings.name);
    settings->setValue("led_controller/baud", QString::number(led_serial->portSettings.baudRate));
    settings->setValue("led_controller/dataBits", QString::number(led_serial->portSettings.dataBits));
    settings->setValue("led_controller/flow", QString::number(led_serial->portSettings.flowControl));
    settings->setValue("led_controller/parity", QString::number(led_serial->portSettings.parity));
    settings->setValue("led_controller/stopBits", QString::number(led_serial->portSettings.stopBits));
    settings->setValue("led_controller/device_id", dev_id);

    settings->sync();
    qDebug() << "Настройки сохранены";

}

void MainWindow::loadSettings()
{
    //Application
    QPoint pos = settings->value("app/pos", QPoint(100, 100)).toPoint();
    QSize size = settings->value("app/size", QSize(1000, 400)).toSize();
    resize(size);
    move(pos);

    timer_delay_ms = settings->value("app/timer_step", 1000).toInt();

    nLed = settings->value("led_params/numberOfDiods", 100).toInt();
    width_nLed = settings->value("led_params/widthInLed", 35).toInt();
    height_nLed = settings->value("led_params/heightInLed", 15).toInt();

    led_serial->portSettings.name = (settings->value("led_controller/port").toString());
    int led_controller_baud = settings->value("led_controller/baud").toInt();
    led_serial->portSettings.baudRate = led_controller_baud;

    led_serial->portSettings.dataBits = settings->value("led_controller/dataBits").toInt();
    led_serial->portSettings.flowControl = settings->value("led_controller/flow").toInt();
    led_serial->portSettings.parity = settings->value("led_controller/parity").toInt();
    led_serial->portSettings.stopBits = settings->value("led_controller/stopBits").toInt();
    dev_id = settings->value("led_controller/device_id").toInt();

    qDebug() << "LED controller" << led_serial->portSettings.name << " /baud " << led_controller_baud;

    qDebug() << "Настройки считаны";
}

void MainWindow::connectSerialPortClicked()
{
    if (!led_serial->isOpened()) {
        led_serial->openPort();
        if (led_serial->isOpened())
            ui->openPortButton->setText("Отключиться");
    } else {
        led_serial->closePort();
        if (!led_serial->isOpened())
            ui->openPortButton->setText("Подключиться");
    }
}

void MainWindow::closeSerialPort()
{
    led_serial->closePort();
}

void MainWindow::on_serialSettingsButton_clicked()
{
    /*
     * When user push the button, we create and show GUI for serial port settings
     */
    SettingsDialog  *m_settingsDialog = new SettingsDialog(led_serial->portSettings, this);
    m_settingsDialog->show();
}
