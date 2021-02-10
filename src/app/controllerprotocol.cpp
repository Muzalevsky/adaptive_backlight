//#include "controllerprotocol.h"
//#include <QDataStream>
//#include <QDebug>

//#include <crc_calc.h>

//ControllerProtocol::ControllerProtocol()
//{

//}

//void ControllerProtocol::addCrcToArray(QByteArray& ba)
//{
//    uint16_t crc = crc16(static_cast<uint8_t*>(ba.data()), ba.length());
//    uint8_t crc_h = (crc >> 8) & 0xFF;
//    uint8_t crc_l = crc & 0xFF;
//    ba << crc_l << crc_h;
//}

//void ControllerProtocol::getId()
//{
////    Структура фрейма запроса(к устройству): [0xFF][0x00][CRC16_L][CRC16_H]
//    QByteArray ba;
//    QDataStream stream(&ba, QIODevice::WriteOnly);
//    stream.setVersion(QDataStream::Qt_5_10);
//    stream.setByteOrder( QDataStream::LittleEndian );

//    stream << broadcast_id << Commands::GET_ID;
//    addCrcToArray(ba);

//    send(ba);
//}

////Структура фрейма ответа(от устройства): [ID][0x00][CRC16_L][CRC16_H]
////[ID] - ID устройства;
////[0x00] - команда;


//void ControllerProtocol::setLedNumber(int nLed)
//{
////    Структура фрейма запроса(к устройству): [ID][0x01][LED1][LED2][CRC16_L][CRC16_H]
//    QByteArray ba;
//    QDataStream stream(&ba, QIODevice::WriteOnly);
//    stream.setVersion(QDataStream::Qt_5_10);
//    stream.setByteOrder( QDataStream::LittleEndian );

//    stream << id << Commands::SET_LED_NUMBER << nLed / 10 << nLed % 10;

//    addCrcToArray(ba);

//    send(ba);
//}

////Структура фрейма ответа(от устройства): [ID][0x01][OK][CRC16_L][CRC16_H]
////[ID] - ID устройства;
////[0x01] - команда;
////[OK] - статус(0 - не выполнено; 1 - выполнено);

//void ControllerProtocol::setBrightness(int br)
//{
////    Структура фрейма запроса(к устройству): [ID][0x02][BR1][BR2][CRC16_L][CRC16_H]
//    QByteArray ba;
//    QDataStream stream(&ba, QIODevice::WriteOnly);
//    stream.setVersion(QDataStream::Qt_5_10);
//    stream.setByteOrder( QDataStream::LittleEndian );

//    stream << id << Commands::SET_BRIGHTNESS << br / 10 << br % 10;
//    addCrcToArray(ba);

//    send(ba);
//}

////Структура фрейма ответа(от устройства): [ID][0x02][OK][CRC16_L][CRC16_H]
////[ID] - ID устройства;
////[0x02] - команда;
////[OK] - статус(0 - не выполнено; 1 - выполнено);

//void ControllerProtocol::getParams()
//{
////    Структура фрейма запроса(к устройству): [ID][0x03][CRC16_L][CRC16_H]
//    QByteArray ba;
//    QDataStream stream(&ba, QIODevice::WriteOnly);
//    stream.setVersion(QDataStream::Qt_5_10);
//    stream.setByteOrder( QDataStream::LittleEndian );

//    stream << id << Commands::GET_PARAMS;
//    addCrcToArray(ba);

//    send(ba);
//}

////Структура фрейма ответа(от устройства): [ID][0x03][LED1][LED2][BR1][BR2][CRC16_L][CRC16_H]
////[ID] - ID устройства;
////[0x03] - команда;
////[LED1] - число диодов с мультипликатором х10;
////[LED2] - число диодов без мультипликатора;
////Установленное число диодов будет равно LED1x10 + LED2.
////[BR1] - яркость с мультипликатором х10;
////[BR2] - яркость без мультипликатора;
////Установленная яркость будет равна BR1x10 + BR2.

//void ControllerProtocol::setLedColor( QVector<QRgb> colors)
//{
////    Структура фрейма запроса(к устройству): [ID][0x04][R1][G1][B1]...[Rn][Gn][Bn][CRC16_L][CRC16_H]
//    QByteArray ba;
//    QDataStream stream(&ba, QIODevice::WriteOnly);
//    stream.setVersion(QDataStream::Qt_5_10);
//    stream.setByteOrder( QDataStream::LittleEndian );

//    stream << id << Commands::SET_LED_COLOR << colors.data();
//    addCrcToArray(ba);

//    send(ba);
//}
////Ответ не отправляется.

//void ControllerProtocol::setId(int new_id)
//{
////    Структура фрейма запроса(к устройству): [ID][0x05][ID_new][CRC16_L][CRC16_H]
//    QByteArray ba;
//    QDataStream stream(&ba, QIODevice::WriteOnly);
//    stream.setVersion(QDataStream::Qt_5_10);
//    stream.setByteOrder( QDataStream::LittleEndian );

//    stream << id << Commands::SET_ID << new_id;
//    addCrcToArray(ba);

//    send(ba);
//}
////Структура фрейма ответа(от устройства): [ID_new][0x05][OK][CRC16_L][CRC16_H]
////[ID_new] - новый ID устройства;
////[0x05] - команда;
////[OK] - статус(0 - не выполнено; 1 - выполнено);

//void ControllerProtocol::parseAnswer(QByteArray ba)
//{
//    int reply_id = ba[0];
//    Commands cmd = static_cast<Commands>(ba[1]);
//    switch (cmd)
//    {
//        case Commands::SET_ID:
//            qDebug() << "Новый ID установлен";
//            break;

//        default:
//            qDebug() << "Response error ";
//            break;
//    }
//}
