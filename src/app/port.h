#ifndef PORT_H
#define PORT_H

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QVector>

typedef struct {
    QString name;
    qint32 baudRate;
    int dataBits;
    int parity;
    int stopBits;
    int flowControl;
    int responseTime;
    int numberOfRetries;
} PortSettings;

Q_DECLARE_METATYPE(PortSettings)

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
    void portStateChanged(bool isConnected);

public slots:
    void closePort();
    void openPort();
    void process_Port();
    void WriteToPort(QByteArray ba);
    void ReadInPort();
    void setPortSettings(PortSettings ps);

private slots:
    void handleError(QSerialPort::SerialPortError error);
};

#endif // PORT_H
