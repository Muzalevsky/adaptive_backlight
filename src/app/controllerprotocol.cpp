#include <QDataStream>
#include <QDebug>

#include "controllerprotocol.h"
#include <crc_calc.h>


ControllerProtocol::ControllerProtocol(QObject *parent) :
    QObject(parent),
    id(0),
    ledNumber(0),
    brightness(0)
{

}

void ControllerProtocol::parseAnswer(QByteArray ba)
{
    uint8_t _cmd = ba[1];
    Commands cmd = static_cast<Commands>(_cmd);
    switch (cmd)
    {
        case Commands::GET_ID:
        {
            id = ba[0];
            qDebug() << "<< GET_ID:" << id;
            emit getIdFromDevice(id);
            break;
        }
        case Commands::SET_LED_NUMBER:
        {
            bool ok = ba[2];
            qDebug() << "<< SET_LED_NUMBER:" << ledNumber << (ok ? "ok" : "failed");
            break;
        }
        case Commands::SET_BRIGHTNESS:
        {
            bool ok = ba[2];
            qDebug() << "<< SET_BRIGHTNESS:" << brightness << (ok ? "ok" : "failed");
            break;
        }
        case Commands::GET_PARAMS:
        {
            id = ba[0];
            qDebug() << "<< GET_PARAMS:" << id;

            ledNumber = ba[2] * 10 + ba[3];
            brightness = ba[4] * 10 + ba[5];
            emit getBrightnessFromDevice(brightness);
            break;
        }
        case Commands::SET_ID:
        {
            id = ba[0];
            qDebug() << "<< SET_ID:" << id;
            break;
        }
        case Commands::SET_LED_COLOR:
            break;
        default:
            qDebug() << "Response error ";
            break;
    }
}

//    Структура фрейма запроса(к устройству): [0xFF][0x00][CRC16_L][CRC16_H]
void ControllerProtocol::getId(QByteArray ba)
{
    QDataStream stream(&ba, QIODevice::WriteOnly);
    stream.setByteOrder( QDataStream::LittleEndian );

    stream << static_cast<quint8>(broadcast_id) << static_cast<quint8>(Commands::GET_ID);
    quint16 crc = crc16(reinterpret_cast<quint8*>(ba.data()), ba.length());
    quint8 crc_h = (crc >> 8) & 0xFF;
    quint8 crc_l = crc & 0xFF;
    stream << static_cast<quint8>(crc_l) << static_cast<quint8>(crc_h);
}
////Структура фрейма ответа(от устройства): [ID][0x00][CRC16_L][CRC16_H]
////[ID] - ID устройства;
////[0x00] - команда;


//    Структура фрейма запроса(к устройству): [ID][0x01][LED1][LED2][CRC16_L][CRC16_H]
void ControllerProtocol::setLedNumber(QByteArray ba, int nLed)
{
    ledNumber = nLed;
    QDataStream stream(&ba, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_10);
    stream.setByteOrder( QDataStream::LittleEndian );

    stream << static_cast<quint8>(id) << static_cast<quint8>(Commands::SET_LED_NUMBER);
    stream << static_cast<quint8>(ledNumber / 10) << static_cast<quint8>(ledNumber % 10);
    quint16 crc = crc16(reinterpret_cast<quint8*>(ba.data()), ba.length());
    quint8 crc_h = (crc >> 8) & 0xFF;
    quint8 crc_l = crc & 0xFF;
    stream << static_cast<quint8>(crc_l) << static_cast<quint8>(crc_h);
}
////Структура фрейма ответа(от устройства): [ID][0x01][OK][CRC16_L][CRC16_H]
////[ID] - ID устройства;
////[0x01] - команда;
////[OK] - статус(0 - не выполнено; 1 - выполнено);

//    Структура фрейма запроса(к устройству): [ID][0x02][BR1][BR2][CRC16_L][CRC16_H]
void ControllerProtocol::setBrightness(QByteArray ba, int br)
{
    brightness = br;

    QDataStream stream(&ba, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_10);
    stream.setByteOrder( QDataStream::LittleEndian );

    stream << static_cast<quint8>(id) << static_cast<quint8>(Commands::SET_BRIGHTNESS);
    stream << static_cast<quint8>(brightness / 10) << static_cast<quint8>(brightness % 10);
    quint16 crc = crc16(reinterpret_cast<quint8*>(ba.data()), ba.length());
    quint8 crc_h = (crc >> 8) & 0xFF;
    quint8 crc_l = crc & 0xFF;
    stream << static_cast<quint8>(crc_l) << static_cast<quint8>(crc_h);
}
////Структура фрейма ответа(от устройства): [ID][0x02][OK][CRC16_L][CRC16_H]
////[ID] - ID устройства;
////[0x02] - команда;
////[OK] - статус(0 - не выполнено; 1 - выполнено);

//    Структура фрейма запроса(к устройству): [ID][0x03][CRC16_L][CRC16_H]
void ControllerProtocol::getParams(QByteArray ba)
{
    QDataStream stream(&ba, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_10);
    stream.setByteOrder( QDataStream::LittleEndian );

    stream << static_cast<quint8>(id) << static_cast<quint8>(Commands::GET_PARAMS);
    quint16 crc = crc16(reinterpret_cast<quint8*>(ba.data()), ba.length());
    quint8 crc_h = (crc >> 8) & 0xFF;
    quint8 crc_l = crc & 0xFF;
    stream << static_cast<quint8>(crc_l) << static_cast<quint8>(crc_h);
}
////Структура фрейма ответа(от устройства): [ID][0x03][LED1][LED2][BR1][BR2][CRC16_L][CRC16_H]
////[ID] - ID устройства;
////[0x03] - команда;
////[LED1] - число диодов с мультипликатором х10;
////[LED2] - число диодов без мультипликатора;
////Установленное число диодов будет равно LED1x10 + LED2.
////[BR1] - яркость с мультипликатором х10;
////[BR2] - яркость без мультипликатора;
////Установленная яркость будет равна BR1x10 + BR2.


//    Структура фрейма запроса(к устройству): [ID][0x05][ID_new][CRC16_L][CRC16_H]
void ControllerProtocol::setId(QByteArray ba, int new_id)
{
    QDataStream stream(&ba, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_10);
    stream.setByteOrder( QDataStream::LittleEndian );

    stream << static_cast<quint8>(id) << static_cast<quint8>(Commands::SET_ID);
    stream << static_cast<quint8>(new_id);
    quint16 crc = crc16(reinterpret_cast<quint8*>(ba.data()), ba.length());
    quint8 crc_h = (crc >> 8) & 0xFF;
    quint8 crc_l = crc & 0xFF;
    stream << static_cast<quint8>(crc_l) << static_cast<quint8>(crc_h);
}
////Структура фрейма ответа(от устройства): [ID_new][0x05][OK][CRC16_L][CRC16_H]
////[ID_new] - новый ID устройства;
////[0x05] - команда;
////[OK] - статус(0 - не выполнено; 1 - выполнено);

