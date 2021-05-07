# This is cli interface class for interface with ble cli serial
import time

import serial
import sys


class BleCli:
    def __init__(self, com_port, baud_rate=1000000, timeout=1.0):
        try:
            self.port = serial.Serial(com_port, baudrate=baud_rate, timeout=timeout)
        except (OSError, serial.SerialException):
            print("Invalid COM Port")
            sys.exit()

    def send_and_rcv_block(self, send_string, eod, timeout=5.0):
        data = []
        did_timeout = False
        self.port.timeout = timeout
        self.port.flushInput()
        self.port.write(send_string.encode())

        while True:
            rcv = self.port.readline().decode('utf-8')

            if rcv.find(eod) == -1 and len(rcv) != 0:
                data.append(rcv)
            elif len(rcv) == 0:
                did_timeout = True
                return data, did_timeout
            else:
                data.append(rcv)
                return data, did_timeout

    def send_and_ack(self, send_string, ack_string, timeout=.5):
        self.port.timeout = timeout
        self.port.write(send_string.encode())
        self.port.flushInput()
        # rcv = (self.port.read_until('\r\n')).decode('utf-8')
        rcv = self.port.readline()
        rcv = rcv.decode('utf-8')
        if rcv.find(ack_string) != -1:
            return True
        else:
            return False

    def rcv_line(self, timeout=5.0):
        self.port.timeout = timeout
        return self.port.readline().decode('utf-8')
