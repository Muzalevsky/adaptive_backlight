#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QScreen>
#include <QWindow>
#include <QThread>
#include <QTimer>

#include <controllerprotocol.h>
#include <crc_calc.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    isTimerEnabled(false)
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

    connect(this, SIGNAL(openPort()), led_serial, SLOT(openPort()), Qt::QueuedConnection);
    connect(this, SIGNAL(closePort()), led_serial, SLOT(closePort()), Qt::QueuedConnection);

    connect(led_serial, &Port::portStateChanged, this, &MainWindow::serialPortConnected, Qt::QueuedConnection);

    if (QMetaType::type("PortSettings") == QMetaType::UnknownType) {
       qRegisterMetaType<PortSettings>("PortSettings");
    }

    connect(this,&MainWindow::setPortSettings, led_serial, &Port::setPortSettings, Qt::QueuedConnection);

    connect(threadLed, SIGNAL(started()), led_serial, SLOT(process_Port()));
    connect(threadLed, SIGNAL(finished()), led_serial, SLOT(deleteLater()));
    threadLed->start();

    connect( this, &MainWindow::updateLeds, led_serial, &Port::WriteToPort, Qt::QueuedConnection );
    connect(ui->openPortButton, &QPushButton::clicked, this, &MainWindow::connectSerialPortClicked);

    connect(led_serial, &Port::outPortByteArray, this, &MainWindow::parseAnswer, Qt::QueuedConnection);

    loadSettings();

    /*
     * Timer for screen capturing and picture refreshing
     */
    screenshot_timer = new QTimer(this);
    connect(screenshot_timer, &QTimer::timeout, this, &MainWindow::makeScreenShot);
    connect(ui->timerButton, &QPushButton::clicked, this, &MainWindow::timerButtonClicked);

    /*
     * Testing buttons
     */
    connect(ui->getIdButton, &QPushButton::clicked, this, &MainWindow::getId);
    connect(ui->getParamsButton, &QPushButton::clicked, this, &MainWindow::getParams);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::getId()
{
    QByteArray ba;
    QDataStream stream(&ba, QIODevice::WriteOnly);
    stream.setByteOrder( QDataStream::LittleEndian );

    stream << static_cast<quint8>(0xFF) << static_cast<quint8>(Commands::GET_ID);
    quint16 crc = crc16(reinterpret_cast<quint8*>(ba.data()), ba.length());
    quint8 crc_h = (crc >> 8) & 0xFF;
    quint8 crc_l = crc & 0xFF;
    stream << static_cast<quint8>(crc_l) << static_cast<quint8>(crc_h);
    qDebug() << ba;
    emit updateLeds(ba);
}

void MainWindow::getParams()
{
    QByteArray ba;
    QDataStream stream(&ba, QIODevice::WriteOnly);
    stream.setByteOrder( QDataStream::LittleEndian );

    stream << static_cast<quint8>(13) << static_cast<quint8>(Commands::GET_PARAMS);
    quint16 crc = crc16(reinterpret_cast<quint8*>(ba.data()), ba.length());
    quint8 crc_h = (crc >> 8) & 0xFF;
    quint8 crc_l = crc & 0xFF;
    stream << static_cast<quint8>(crc_l) << static_cast<quint8>(crc_h);
    qDebug() << ba;
    emit updateLeds(ba);
}

void MainWindow::timerButtonClicked()
{
    if (!isTimerEnabled) {
        isTimerEnabled = true;
        screenshot_timer->start(timer_delay_ms);
        ui->timerButton->setText(tr("Stop"));
    } else {
        isTimerEnabled = false;
        screenshot_timer->stop();
        ui->timerButton->setText(tr("Start"));
    }
}

void MainWindow::parseAnswer(QByteArray ba)
{
    qDebug() << ba;
}

void MainWindow::makeScreenShot()
{
    QScreen *screen = QGuiApplication::primaryScreen();
    if (const QWindow *window = windowHandle())
        screen = window->screen();

    if (!screen)
        return;

    if (width_nLed == 0 || height_nLed == 0)
        return;

    /*
     * Get screenshot as image
     */
    QImage image = screen->grabWindow(0).toImage();
    QSize image_size = image.size();
    int px_per_led_w = image_size.width() / width_nLed;
    int px_per_led_h = image_size.height() / height_nLed;

    /*
     * Start creating message for the LED controller
     * Each side is in its cycle
     */
    QByteArray ba;
    QDataStream stream(&ba, QIODevice::WriteOnly);
    stream.setByteOrder( QDataStream::LittleEndian );

    /*
     * First two bytes. Check protocol
     */
    stream << static_cast<quint8>(dev_id) << static_cast<quint8>(Commands::SET_LED_COLOR);

    /* Top side */
    for (quint16 led_idx = 0; led_idx < width_nLed; led_idx++) {
        QRect r(px_per_led_w * led_idx, 0, px_per_led_w, px_per_led_h);
        QImage img_per_led = image.copy(r);
        /*
         * Trying to apply bilinear filter to get average image color
         */
        QImage average_px = img_per_led.scaled(1,1,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
        QColor color = QColor::fromRgba(average_px.pixel(0,0));

        stream << static_cast<quint8>(color.red())
               << static_cast<quint8>(color.green())
               << static_cast<quint8>(color.blue());
    }

    /* Right side */
    for (quint16 led_idx = 0; led_idx < height_nLed; led_idx++) {
        QRect r(image_size.width() - px_per_led_w, px_per_led_h * led_idx, px_per_led_w, px_per_led_h);
        QImage img_per_led = image.copy(r);

        QImage average_px = img_per_led.scaled(1,1,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
        QColor color = QColor::fromRgba(average_px.pixel(0,0));

        stream << static_cast<quint8>(color.red())
               << static_cast<quint8>(color.green())
               << static_cast<quint8>(color.blue());
    }

    /* Bottom side */
    for (quint16 led_idx = 0; led_idx < width_nLed; led_idx++) {
        QRect r(image_size.width() - px_per_led_w * (led_idx + 1), image_size.height() - px_per_led_h, px_per_led_w, px_per_led_h);
        QImage img_per_led = image.copy(r);

        QImage average_px = img_per_led.scaled(1,1,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
        QColor color = QColor::fromRgba(average_px.pixel(0,0));

        stream << static_cast<quint8>(color.red())
               << static_cast<quint8>(color.green())
               << static_cast<quint8>(color.blue());
    }

    /* Left side */
    for (quint16 led_idx = 0; led_idx < height_nLed; led_idx++) {
        QRect r(0, image_size.height() - px_per_led_h * (led_idx + 1), px_per_led_w, px_per_led_h);
        QImage img_per_led = image.copy(r);

        QImage average_px = img_per_led.scaled(1,1,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
        QColor color = QColor::fromRgba(average_px.pixel(0,0));

        stream << static_cast<quint8>(color.red())
               << static_cast<quint8>(color.green())
               << static_cast<quint8>(color.blue());
    }

    quint16 crc = crc16(reinterpret_cast<quint8*>(ba.data()), ba.length());
    quint8 crc_h = (crc >> 8) & 0xFF;
    quint8 crc_l = crc & 0xFF;
    stream << static_cast<quint8>(crc_l) << static_cast<quint8>(crc_h);

//    qDebug() << ba;

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

    width_nLed = settings->value("led_params/widthInLed", 35).toInt();
    height_nLed = settings->value("led_params/heightInLed", 15).toInt();

    PortSettings portSettings;

    portSettings.name = (settings->value("led_controller/port").toString());
    int led_controller_baud = settings->value("led_controller/baud").toInt();
    portSettings.baudRate = led_controller_baud;

    portSettings.dataBits = settings->value("led_controller/dataBits").toInt();
    portSettings.flowControl = settings->value("led_controller/flow").toInt();
    portSettings.parity = settings->value("led_controller/parity").toInt();
    portSettings.stopBits = settings->value("led_controller/stopBits").toInt();
    dev_id = settings->value("led_controller/device_id").toInt();

    qDebug() << "LED controller" << portSettings.name << " /baud " << led_controller_baud;

    emit setPortSettings(portSettings);

    qDebug() << "Настройки считаны";
}

void MainWindow::connectSerialPortClicked()
{
    qDebug() << "Clicked";
    qDebug() << "ui->openPortButton->isChecked()" << ui->openPortButton->isChecked();

    if (ui->openPortButton->isChecked()) {
        emit openPort();
    } else {
        emit closePort();
    }
}

void MainWindow::serialPortConnected(bool isConnected)
{
    if (isConnected) {
        ui->openPortButton->setText(tr("Disconnect"));
    } else {
        ui->openPortButton->setText(tr("Connect"));
        if (ui->openPortButton->isChecked())
            ui->openPortButton->setChecked(false);
    }
}

void MainWindow::on_serialSettingsButton_clicked()
{
    /*
     * When user push the button, we create and show GUI for serial port settings
     */
    SettingsDialog  *m_settingsDialog = new SettingsDialog(led_serial->portSettings, this);
    m_settingsDialog->show();
}
