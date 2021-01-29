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
    ControllerProtocol();
    uint8_t broadcast_id = 0xFF;
    uint8_t id;
    void addCrcToArray(QByteArray& ba);
    void getId();
    void setLedNumber(int nLed);
    void setBrightness(int br);
    void getParams();
    void setLedColor(QVector<QRgb> colors);
    void setId(int new_id);
    void parseAnswer(QByteArray ba);

signals:
    void send(QByteArray arr);
};

#endif // CONTROLLERPROTOCOL_H
