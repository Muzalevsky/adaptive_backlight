from CRC16 import *


def cmd_0x00_send():
    out_frame = [0xff, 0x00]
    crc = crc16(out_frame)
    out_frame.append(crc[0])
    out_frame.append(crc[1])

    return [out_frame, 4]


def cmd_0x01_send(ID, led_num):
    LED1 = int(led_num / 10.0)
    LED2 = led_num - LED1 * 10

    out_frame = [ID, 0x01, LED1, LED2]
    crc = crc16(out_frame)
    out_frame.append(crc[0])
    out_frame.append(crc[1])

    return [out_frame, 5]


def cmd_0x02_send(ID, bright):
    BR1 = int(bright / 10.0)
    BR2 = bright - BR1 * 10

    out_frame = [ID, 0x02, BR1, BR2]
    crc = crc16(out_frame)
    out_frame.append(crc[0])
    out_frame.append(crc[1])

    return [out_frame, 5]


def cmd_0x03_send(ID):
    out_frame = [ID, 0x03]

    crc = crc16(out_frame)
    out_frame.append(crc[0])
    out_frame.append(crc[1])

    return [out_frame, 8]


def cmd_0x04_send(ID, leds_colors):
    out_frame = [ID, 0x04]
    out_frame.extend(leds_colors)

    crc = crc16(out_frame)
    out_frame.append(crc[0])
    out_frame.append(crc[1])

    return out_frame


def cmd_0x05_send(ID, ID_new):
    out_frame = [ID, 0x05, ID_new]

    crc = crc16(out_frame)
    out_frame.append(crc[0])
    out_frame.append(crc[1])

    return [out_frame, 5]
