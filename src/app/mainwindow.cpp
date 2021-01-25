#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QScreen>
#include <QWindow>
#include <QtSerialPort/QSerialPort>
#include <QRgb>
#include <QThread>
#include <QTimer>
// 1920 / 396 = 4 pixels per LED
const uint16_t nLed = 396;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _debug(true)
{
    ui->setupUi(this);

    settings = new QSettings( "settings.ini", QSettings::IniFormat );


    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::makeScreenShot);
    timer->start(5000);

    led_serial    = new Port();
    m_settingsDialog = new SettingsDialog(this);
    connect( ui->serialSettingsButton, &QPushButton::clicked,
              m_settingsDialog, &QDialog::show, Qt::QueuedConnection);

    loadSettings();

    QThread *threadLed = new QThread;
    led_serial->moveToThread(threadLed);
    connect(led_serial, SIGNAL(finished_Port()), threadLed, SLOT(deleteLater()));
    connect(led_serial, SIGNAL(finished_Port()), threadLed, SLOT(quit()));
    connect(threadLed, SIGNAL(started()), led_serial, SLOT(process_Port()));//Переназначения метода run
    connect(threadLed, SIGNAL(finished()), led_serial, SLOT(deleteLater()));//Удалить к чертям поток
    threadLed->start();

    connect( this, &MainWindow::updateLeds, led_serial, &Port::WriteToPort );



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

    int px_per_led = screen->size().width() / nLed;
    qDebug() << "Using " << px_per_led  << "px_per_led";

    pixmap = screen->grabWindow(0);

    QByteArray ba;

    for (uint16_t led_idx = 0; led_idx < nLed; led_idx++) {
        QPixmap pixmap_per_led = pixmap.copy(px_per_led * led_idx, 0, px_per_led, px_per_led);
        QImage img_per_led = pixmap_per_led.toImage();

        // bilinear filtration
        QImage average_px = img_per_led.scaled(1,1,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
        QColor color = QColor::fromRgba(average_px.pixel(0,0));
//        qDebug() << "color" << color;

        ba.append(led_idx);
        ba.append(color.rgb());

        if (_debug) {
            QImage *image = new QImage(10, 10, QImage::Format_RGB32);
            image->fill(color);
            image->save(QString::number(led_idx) + ".jpg");
        }
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

    settings->setValue( "led_controller/port", m_settingsDialog->port_name );
    settings->setValue( "led_controller/baud", QString::number( m_settingsDialog->settings().baud ) );

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

    m_settingsDialog->port_name = settings->value("led_controller/port").toString();
    int led_controller_baud = settings->value("led_controller/baud").toInt();
    m_settingsDialog->setBaud(led_controller_baud);
    qDebug() << "LED controller" << m_settingsDialog->port_name << " /baud " << led_controller_baud;

    qDebug() << "Настройки считаны";
}

void MainWindow::on_serialSettingsButton_released()
{
    m_settingsDialog->show();
}

void MainWindow::openSerialPort()
{
    const SettingsDialog::Settings p = m_settingsDialog->settings();

    led_serial->setPortSettings( m_settingsDialog->port_name,
                           p.baud, p.dataBits, p.parity, p.stopBits, p.flow );

    led_serial->openPort();
}

void MainWindow::closeSerialPort()
{
    led_serial->closePort();
}
