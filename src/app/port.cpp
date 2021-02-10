#include "port.h"
#include <QDebug>
#include <QMessageBox>

#include <QThread>

Port::Port(QObject *parent) :
    QObject(parent)
{
    /*
     * see https://doc-snapshots.qt.io/qt5-5.10/qserialport.html
     *
     * This option is useful if the data is only read at certain points in time
     *  (for instance in a real-time streaming application) or if the serial
     * port should be protected against receiving too much data, which
     * may eventually cause the application to run out of memory.
     */
//    thisPort.setReadBufferSize(1000);
}

Port::~Port()
{
    emit finished_Port();
}

//void Port::setPortSettings(PortSettings& ps)
//{
////    portSettings = ps;

//    portSettings.baudRate = ps.baudRate;
//    portSettings.dataBits = ps.dataBits;
//    portSettings.flowControl = ps.flowControl;
//    portSettings.name = ps.name;
//    portSettings.numberOfRetries = ps.numberOfRetries;
//    portSettings.parity = ps.parity;
//    portSettings.responseTime = ps.responseTime;
//    portSettings.stopBits = ps.stopBits;


//}

void Port::process_Port()
{
    qDebug() << "Serial port" << this
             << "in thread" << this->thread()
             << "parent" << parent();

    qRegisterMetaType<QSerialPort::SerialPortError>();
    connect(&thisPort, &QSerialPort::errorOccurred, this, &Port::handleError);
    connect(&thisPort, SIGNAL(readyRead()),this,SLOT(ReadInPort()));
}

void Port::openPort()
{
    thisPort.setPortName(portSettings.name);
    qDebug() << "Opening " << thisPort.portName();

    qDebug() << "baudRate" << portSettings.baudRate;
    qDebug() << "data bits" << portSettings.dataBits;
    qDebug() << "parity" << portSettings.parity;
    qDebug() << "stop bits" << portSettings.stopBits;
    qDebug() << "FlowControl" << portSettings.flowControl;

    if (thisPort.open(QIODevice::ReadWrite))
    {
        if ( thisPort.setBaudRate(portSettings.baudRate) &&
            thisPort.setDataBits(static_cast<QSerialPort::DataBits>(portSettings.dataBits)) &&
            thisPort.setParity(static_cast<QSerialPort::Parity>(portSettings.parity)) &&
            thisPort.setStopBits(static_cast<QSerialPort::StopBits>(portSettings.stopBits)) &&
            thisPort.setFlowControl(static_cast<QSerialPort::FlowControl>(portSettings.flowControl)) )
        {
            if ( thisPort.isOpen() )
            {
                qDebug() << portSettings.name + " >> Open!";
                thisPort.clear();
            }
        }
        else
        {
            thisPort.close();
            qDebug() << thisPort.errorString();
        }
    }
    else
    {
        qDebug() << thisPort.errorString();
    }
}

// TODO Check how it is working during deleting now
void Port::handleError(QSerialPort::SerialPortError error)
{
    if ( error != QSerialPort::NoError )
        qDebug() << thisPort.portName() << "Error:" <<  thisPort.errorString();

    switch ( error )
    {
        /*
         * An error occurred while attempting to open an non-existing device.
         */
        case QSerialPort::DeviceNotFoundError:
            thisPort.close();
        break;

        /*
         * An I/O error occurred when a resource becomes unavailable, e.g.
         * when the device is unexpectedly removed from the system.
         */
        case QSerialPort::ResourceError:
            thisPort.close();
        break;

        case QSerialPort::TimeoutError:
        break;

        default:
        break;
    }
}

void Port::closePort()
{
    if ( thisPort.isOpen() ) {
        thisPort.clear( QSerialPort::AllDirections );
        thisPort.close();
        qDebug() << portSettings.name << " >> Close!";
    }
}

void Port::WriteToPort(QByteArray ba)
{
    if ( thisPort.isOpen() ) {
        thisPort.write(ba);
//        qDebug() << ">>>" << ba;
    }
}

void Port::ReadInPort()
{
    if ( thisPort.isOpen() )
    {
        QByteArray data;
        data.append(thisPort.readAll());
//        qDebug() << "<<<" << data;
        emit outPortByteArray(data);
    }
}

bool Port::isOpened()
{
    return thisPort.isOpen();
}
