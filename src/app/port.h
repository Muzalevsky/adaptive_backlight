#ifndef PORT_H
#define PORT_H

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QVector>

struct PortSettings {
    QString name;
    qint32 baudRate;
    int dataBits;
    int parity;
    int stopBits;
    int flowControl;
    int responseTime;
    int numberOfRetries;
};

class Port : public QObject
{
    Q_OBJECT
public:
    explicit Port(QObject *parent = 0);
    ~Port();

    bool isOpened();

    PortSettings portSettings;
private:
    QSerialPort thisPort;

signals:
    void finished_Port();
    void outPortByteArray(QByteArray data);
    void connectionStateChanged(bool isConnected);

public slots:
    void closePort();
    void openPort();
    void process_Port();
    void WriteToPort(QByteArray ba);
    void ReadInPort();
//    void setPortSettings(PortSettings& ps);

private slots:
    void handleError(QSerialPort::SerialPortError error);
};

#endif // PORT_H
