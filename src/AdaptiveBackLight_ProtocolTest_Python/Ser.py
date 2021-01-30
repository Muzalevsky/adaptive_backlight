import serial


class SerialPort:
    def __init__(self, device, baud):
        self.__dev = device
        self.__baud = baud
        self.Serial = serial.Serial()
        self.Serial.port = self.__dev
        self.Serial.baudrate = self.__baud

    def response_to_list(self, resp):
        length = len(resp)
        out_list = []
        for i in range(length):
            out_list.append(resp[i])
        return out_list
