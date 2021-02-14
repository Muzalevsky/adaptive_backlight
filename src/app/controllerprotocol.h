#ifndef CONTROLLERPROTOCOL_H
#define CONTROLLERPROTOCOL_H

#include <QByteArray>
#include <QObject>
#include <QRgb>

typedef enum {
    GET_ID = 0x00,
    SET_LED_NUMBER = 0x01,
    SET_BRIGHTNESS = 0x02,
    GET_PARAMS = 0x03,
    SET_LED_COLOR = 0x04,
    SET_ID = 0x05
} Commands;

class ControllerProtocol : public QObject
{
    Q_OBJECT
public:
    ControllerProtocol(QObject *parent);

    uint8_t id;

    void getId(QByteArray ba);
    void getParams(QByteArray ba);
    void setBrightness(QByteArray ba, int br);
    void setId(QByteArray ba, int new_id);
    void setLedNumber(QByteArray ba, int nLed);
private:
    uint8_t broadcast_id = 0xFF;
    int ledNumber;
    int brightness;
public slots:
    void parseAnswer(QByteArray ba);

signals:
    void getIdFromDevice(uint8_t);
    void getBrightnessFromDevice(int);
};

#endif // CONTROLLERPROTOCOL_H
