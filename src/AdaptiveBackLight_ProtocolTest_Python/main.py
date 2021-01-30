import time
from CRC16 import *
from ProtocolFuns import *
from Ser import *
from tkinter import colorchooser

class AdaptiveBackLight:
    def __init__(self, COM, baud):
        self.ser = SerialPort(COM, baud)

    def get_device_id(self):
        buf, rx_bytes = cmd_0x00_send()
        print("Request  cmd 0x00: " + str(buf))
        self.ser.Serial.write(buf)
        time.sleep(1.25)
        response = self.ser.Serial.read(rx_bytes)
        response = self.ser.response_to_list(response)
        print("Response cmd 0x00: " + str(response))
        dev_id = response[0]
        print("Device ID = " + str(dev_id))

    def set_device_id(self, cur_ID, new_ID):
        buf, rx_bytes = cmd_0x05_send(cur_ID, new_ID)
        print("Request  cmd 0x05: " + str(buf))
        print("Change device with ID %d to %d" % (cur_ID, new_ID))
        self.ser.Serial.write(buf)
        time.sleep(1.25)
        response = self.ser.Serial.read(rx_bytes)
        response = self.ser.response_to_list(response)
        print("Response cmd 0x05: " + str(response))
        if response[2]:
            print("Change ID DONE!")
        else:
            print("Change failed!")

    def get_info_about_device(self, ID):
        buf, rx_bytes = cmd_0x03_send(ID)
        print("Request  cmd 0x03: " + str(buf))
        self.ser.Serial.write(buf)
        time.sleep(1.25)
        response = self.ser.Serial.read(rx_bytes)
        response = self.ser.response_to_list(response)
        print("Response cmd 0x03: " + str(response))
        Leds_number = response[2]*10 + response[3]
        Bright = response[4]*10 + response[5]
        print("<---- Info about device ---->")
        print("Leds number = " + str(Leds_number))
        print("Bright = " + str(Bright))
        return Leds_number, Bright

    def set_leds_number(self, ID, leds_num):
        buf, rx_bytes = cmd_0x01_send(ID, leds_num)
        print("Request  cmd 0x01: " + str(buf))
        self.ser.Serial.write(buf)
        time.sleep(1.25)
        response = self.ser.Serial.read(rx_bytes)
        response = self.ser.response_to_list(response)
        if response[2]:
            print("Leds number set DONE!")
        else:
            print("Leds number set FAILED")

    def set_bright(self, ID, bright):
        buf, rx_bytes = cmd_0x02_send(ID, bright)
        print("Request  cmd 0x02: " + str(buf))
        self.ser.Serial.write(buf)
        time.sleep(1.25)
        response = self.ser.Serial.read(rx_bytes)
        response = self.ser.response_to_list(response)
        if response[2]:
            print("Bright set DONE!")
        else:
            print("Bright set FAILED")

    def set_leds(self, ID, leds_colors):
        buf = cmd_0x04_send(ID, leds_colors)
        print("Request  cmd 0x04: " + str(buf))
        self.ser.Serial.write(buf)
        print("Leds colors set DONE!")


DEV_ID = 13

if __name__ == '__main__':
    ABL = AdaptiveBackLight('COM5', 256000)
    ABL.ser.Serial.open()
    time.sleep(6)

    # ABL.set_leds_number(DEV_ID, 190)
    # ABL.set_bright(DEV_ID, 255)
    Leds_Num, Bright = ABL.get_info_about_device(DEV_ID)
    for i in range(25):
        color = colorchooser.askcolor()
        red = int(color[0][0])
        green = int(color[0][1])
        blue = int(color[0][2])

        Leds_arr = []
        for i in range(Leds_Num):
            Leds_arr.append(red)
            Leds_arr.append(green)
            Leds_arr.append(blue)
        ABL.set_leds(DEV_ID, Leds_arr)

    ABL.ser.Serial.close()
