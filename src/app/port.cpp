#include "port.h"
#include <QDebug>
#include <QMessageBox>

#include <QThread>

Port::Port(QObject *parent) :
    QObject(parent),
    thisPort(new QSerialPort(this))
{
    /*
     * see https://doc-snapshots.qt.io/qt5-5.10/qserialport.html
     */
    qRegisterMetaType<QSerialPort::SerialPortError>();
    connect(thisPort, &QSerialPort::errorOccurred, this, &Port::handleError);
    connect(thisPort, &QSerialPort::readyRead,this, &Port::ReadInPort);
}

Port::~Port()
{
    emit finished_Port();
}

void Port::setPortSettings(PortSettings ps)
{
    portSettings = ps;
}

void Port::process_Port()
{
    qDebug() << "Serial port" << this
             << "in thread" << this->thread()
             << "parent" << parent();
}

void Port::openPort()
{
    thisPort->setPortName(portSettings.name);
    qDebug() << "Opening " << thisPort->portName();

    qDebug() << "baudRate" << portSettings.baudRate;
    qDebug() << "data bits" << portSettings.dataBits;
    qDebug() << "parity" << portSettings.parity;
    qDebug() << "stop bits" << portSettings.stopBits;
    qDebug() << "FlowControl" << portSettings.flowControl;

    if (thisPort->open(QIODevice::ReadWrite))
    {
        if ( thisPort->setBaudRate(portSettings.baudRate) &&
            thisPort->setDataBits(static_cast<QSerialPort::DataBits>(portSettings.dataBits)) &&
            thisPort->setParity(static_cast<QSerialPort::Parity>(portSettings.parity)) &&
            thisPort->setStopBits(static_cast<QSerialPort::StopBits>(portSettings.stopBits)) &&
            thisPort->setFlowControl(static_cast<QSerialPort::FlowControl>(portSettings.flowControl)) )
        {
            if ( thisPort->isOpen() )
            {
                qDebug() << portSettings.name + " >> Open!";
                emit portStateChanged(true);
                thisPort->clear();
                thisPort->clear( QSerialPort::AllDirections );
            }
        }
        else
        {
            thisPort->close();
            emit portStateChanged(false);
            qDebug() << thisPort->errorString();
        }
    }
    else
    {
        qDebug() << thisPort->errorString();
    }
}

// TODO Check how it is working during deleting now
void Port::handleError(QSerialPort::SerialPortError error)
{
    if ( error != QSerialPort::NoError )
        qDebug() << thisPort->portName() << "Error:" <<  thisPort->errorString();

    switch ( error )
    {
        /*
         * An error occurred while attempting to open an non-existing device.
         */
        case QSerialPort::DeviceNotFoundError:
            thisPort->close();
            emit portStateChanged(false);
        break;

        /*
         * An I/O error occurred when a resource becomes unavailable, e.g.
         * when the device is unexpectedly removed from the system.
         */
        case QSerialPort::ResourceError:
            thisPort->close();
            emit portStateChanged(false);
        break;

        case QSerialPort::TimeoutError:
        break;

        default:
        break;
    }
}

void Port::closePort()
{
    if ( thisPort->isOpen() ) {
        thisPort->clear( QSerialPort::AllDirections );
        thisPort->close();
        qDebug() << portSettings.name << " >> Close!";
        emit portStateChanged(false);
    }
}

void Port::WriteToPort(QByteArray ba)
{
    if ( thisPort->isOpen() ) {
        thisPort->write(ba);
//        qDebug() << ">>>" << ba.toHex();
    }
}

void Port::ReadInPort()
{
    if ( thisPort->isOpen() )
    {
        QByteArray data;
        data.append(thisPort->readAll());
//        qDebug() << "<<<" << data;
        emit outPortByteArray(data);
    }
}
