# This is cli interface class for interface with ble cli serial
import queue
import time

import serial
import sys
import threading

import common.ble_msgs


class CliSerialThreaded:

    def __read_thread(self):
        while True:
            rcv = self.port.readline().decode('utf-8', 'ignore')
            # rcv is empty string if readline() times out
            if rcv != '':
                self.read_queue.put(rcv)

    def write_line(self, write_val):
        self.port.write(write_val.encode())

    def rcv_line(self, timeout):
        try:
            return self.read_queue.get(block=True, timeout=timeout)
        except queue.Empty:
            return ''

    def send_and_ack(self, send_string, ack_string, timeout=5.0):
        self.write_line(send_string)

        while True:
            try:
                data = self.read_queue.get(block=True, timeout=timeout)
                if ack_string in data:
                    return True
            except queue.Empty:
                return False

    def send_and_rcv_block(self, send_string, ack_string, timeout=5.0):
        self.write_line(send_string)
        data = []
        to = False
        while True:
            try:
                d = self.read_queue.get(block=True, timeout=timeout)
                data.append(d)
                if ack_string in d:
                    return data, to
            except queue.Empty:
                to = True
                return data, to

    def __init__(self, com_port, baud_rate=1000000, timeout=1.0):
        self.read_queue = queue.Queue(1024)  # Queue for storing data from dongle, good for thread synchronization
        try:
            self.port = serial.Serial(com_port, baudrate=baud_rate, timeout=timeout)
        except (OSError, serial.SerialException):
            print("Invalid COM Port")
            sys.exit()

        # Once main exits Daemon threads will have .join() called and terminated so no need to do this
        x = threading.Thread(target=self.__read_thread, daemon=True)
        x.start()
