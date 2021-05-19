# BLE class for supporting communication with the dongle
import base64
import collections
import math
import sys
import time

import common.ble_msgs
from common.cli_serial_if_threaded import CliSerialThreaded

service = collections.namedtuple('service', ['handle', 'uuid'])
characteristic = collections.namedtuple('characteristic', ['char_handle', 'val_handle', 'uuid', 'props'])
descriptor = collections.namedtuple('descriptor', ['handle', 'uuid'])

PROPS_BROADCAST = 1
PROPS_READ = 2
PROPS_WRITE_NO_RSP = 4
PROPS_WRITE = 8
PROPS_NOTIFY = 16
PROPS_INDICATE = 32
PROPS_SIGNED_WRITE = 64
PROPS_QUEUED_WRITE = 128


class BleHandler:
    def __init__(self, com_port):
        self.image_size = 0
        # self.cli = BleCli(com_port, 1000000, 1.0)
        self.cli = CliSerialThreaded(com_port, 1000000, 1.0)
        self.database = []

    @staticmethod
    def __pass_fail(ret, msg, suppress=False):
        if ret:
            if not suppress:
                print(f'PASS: {msg}')
        else:
            print(f'FAILURE: {msg}')
            sys.exit()

    def __send_rcv(self, message, ack, timeout):
        msg = common.ble_msgs.PREFIX + message + '\r'
        return self.cli.send_and_ack(msg, ack, timeout)

    def __send_rcv_block(self, message, end, timeout):
        msg = common.ble_msgs.PREFIX + message + '\r'
        return self.cli.send_and_rcv_block(msg, end, timeout)

    def __rcv(self, timeout=5.0):
        return self.cli.rcv_line(timeout)

    @staticmethod
    def __check_adv_data_match(data, a):
        if (a['type'] == data.type) and (a['length'] == data.len) \
                and set(a['data']).issubset(data.data):
            return True

        return False

    def __check_adv_against_filters(self, data_parsed, adv_filter):
        adv_match = False
        if adv_filter:
            for f in adv_filter[common.ble_msgs.ADV_FILTER_KEY]:
                for a in data_parsed:
                    adv_match = self.__check_adv_data_match(a, f)

                if not adv_match:
                    return False
        else:
            return True

        return True

    @staticmethod
    def __check_filters(filters):
        adv_filter = False
        mac_id_filter = False
        rssi_filter = False

        if common.ble_msgs.ADV_FILTER_KEY in filters:
            adv_filter = True
        elif common.ble_msgs.RSSI_FILTER_KEY in filters:
            rssi_filter = True
        elif common.ble_msgs.BD_ADDR_FILTER_KEY in filters:
            mac_id_filter = True

        return mac_id_filter, adv_filter, rssi_filter

    @staticmethod
    def __parse_adv_data(data):
        i = 0
        adv_elements = collections.namedtuple('Adv_Element', ['len', 'type', 'data'])
        adv_element_list = []

        while i < len(data):
            length = data[i]
            a = adv_elements(length, data[i + 1], data[(i + 2):(i + 2 + length)])
            adv_element_list.append(a)
            i += data[i] + 1

        return adv_element_list

    @staticmethod
    def __get_num_elements(self, data):
        i = 0
        elements = 0
        while i < len(data):
            elements += 1
            i += data[i] + 1

        return elements

    @staticmethod
    def __parse_adv_string(input_string):
        data_hex = []
        address = input_string.split(common.ble_msgs.SCAN_ADDR_SUB)[1].split(',')[0].strip()
        address_type = int(input_string.split(common.ble_msgs.SCAN_ADD_T_SUB)[1].split(',')[0].strip())

        try:
            data = input_string.split(common.ble_msgs.SCAN_DATA_SUB)[1].split(',')[0].strip()
            data_hex = [int(x, 16) for x in data.split()]
        except IndexError:
            data_hex = 0

        try:
            rssi = int(input_string.split(common.ble_msgs.SCAN_RSSI_SUB)[1].split(',')[0].strip(), 10)
        except IndexError:
            rssi = -256

        return address, data_hex, rssi, address_type

    @classmethod
    def crc16_sdk(cls, buf, crc16=0xFFFF):
        for b in bytearray(buf):
            for _ in range(8):
                need_xor = True if (((crc16 & 0x8000) >> 15) ^ ((b & 0x80) >> 7)) else False
                crc16 <<= 1

                if need_xor:
                    crc16 ^= 0x1021  # CRC16-CCITT polynomial
                b <<= 1

        return crc16 & 0xFFFF

    @staticmethod
    def __parse_gatt_db(raw_text):
        database = []
        temp_char_handle = 'invalid'
        temp_props = 0

        for item in raw_text:
            if 'serv' in item:
                handle = item.split('serv')[0].strip()
                try:
                    uuid = item.split('0x')[1].strip()
                except IndexError:
                    uuid = item.split('serv')[1].strip()
                s = service(handle, uuid)
                database.append(s)
                continue

            if 'char' in item:
                temp_char_handle = item.split('char')[0].strip()
                temp_props = int(item.split('=')[1].strip().split('(')[0].strip(), 16)
                continue

            if temp_char_handle in item:
                handle = temp_char_handle
                props = temp_props
                try:
                    uuid = item.split('----')[1].strip().split()[0].strip()
                    if '0x' in uuid:
                        uuid = item.split('0x')[1].strip()
                except IndexError:
                    print(f'Exception Capture {temp_char_handle} {item}\n')

                v_handle = hex((int(handle, 16) + 1)).split('0x')[1].strip()
                num_zeros = "0" * (4 - len(v_handle))
                v_handle = f'{num_zeros}{v_handle}'
                c = characteristic(handle, v_handle, uuid, props)
                database.append(c)
                continue

            if 'desc' in item:
                handle = item.split('desc')[0].strip()
                uuid = item.split('0x')[1].strip()

                d = descriptor(handle, uuid)
                database.append(d)
                continue

        return database

    def reset_device(self):
        attempts = 0
        print('Resetting Dongle...')
        msg = common.ble_msgs.RESET
        ret = self.__send_rcv(msg, common.ble_msgs.INIT_EVT, 2.0)
        if not ret:
            ret = self.__send_rcv(msg, common.ble_msgs.INIT_EVT, 2.0)

        self.__pass_fail(ret, msg, suppress=True)

    def verify_img(self):
        msg = common.ble_msgs.VERIFY_IMAGE
        data, to = self.__send_rcv_block(msg, common.ble_msgs.END_IMAGE, timeout=2.0)
        self.__pass_fail(not to, msg, True)

        for item in data:
            if item.find(common.ble_msgs.OK) == -1:
                print_str = item.strip('\r\n')
                print(print_str)

            if common.ble_msgs.FW_SIZE_INFO in item:
                self.image_size = int(item.split(common.ble_msgs.FW_SIZE_INFO)[1].split('bytes')[0].strip(), 10)

    def get_cli_fw_version(self):
        msg = common.ble_msgs.GET_FW_REV
        data, to = self.__send_rcv_block(msg, common.ble_msgs.GET_VERSION_EVT, timeout=1.0)
        self.__pass_fail(not to, msg, True)
        for item in data:
            print_str = item.strip('\r\n')
            if common.ble_msgs.GET_VERSION_EVT in item:
                return item.split(':')[1].strip()

    def erase_image(self):
        print("Erasing Image...")
        msg = common.ble_msgs.ERASE_IMAGE
        ret = self.__send_rcv(msg, common.ble_msgs.OK, 30.0)
        self.__pass_fail(ret, msg, suppress=True)

    def disconnect(self):
        print("Disconnecting...")
        msg = f'{common.ble_msgs.DISCONNECT} {0}'
        ret, to = self.__send_rcv_block(msg, common.ble_msgs.DISCONNECT_EVT, 3.0)
        self.__pass_fail(not to, msg, True)

    def prepare_suota(self):
        print('Preparing SUOTA...')
        msg = f'{common.ble_msgs.GATT_BROWSE} {0}'
        ret, to = self.__send_rcv_block(msg, common.ble_msgs.SUOTA_READY, 10.0)
        self.__pass_fail(not to, msg, True)

    def browse_database(self):
        print('Browsing Database...')
        msg = f'{common.ble_msgs.GATT_BROWSE} {0}'
        ret, to = self.__send_rcv_block(msg, common.ble_msgs.BROWSE_COMPLETED, 10.0)
        self.__pass_fail(not to, msg, True)

        self.database = self.__parse_gatt_db(ret)
        print(f'{self.database}')

    def connect_to_device(self, mac, add_type, con_intvl=None, slave_latency=None, timeout=None):
        print(f'Connecting to {mac}')

        if con_intvl is not None and slave_latency is not None and timeout is not None:
            msg = f'{common.ble_msgs.CONNECT} {mac} {add_type} {con_intvl} {con_intvl} {slave_latency} {timeout}'
        else:
            msg = f'{common.ble_msgs.CONNECT} {mac} {add_type}'

        print(f'{msg}')
        ret, to = self.__send_rcv_block(msg, common.ble_msgs.CONNECTED_EVT, 20.0)
        self.__pass_fail(not to, msg, True)
        print('Connected!')

    def stop_scan(self):
        msg = f'{common.ble_msgs.SCAN} {0}'
        ret, to = self.__send_rcv_block(msg, common.ble_msgs.SCAN_COMPLETED, 3.0)
        self.__pass_fail(not to, msg, True)

    def receive_scan_event(self, adv_filter=None):
        if adv_filter is not None:
            mac_filter, adv_data_filter, rssi_filter = self.__check_filters(adv_filter)

        rcv_line = self.__rcv(5.0)
        data = ''

        if common.ble_msgs.ADV_REPORT_EVT in rcv_line:
            mac, data, rssi, add_type = self.__parse_adv_string(rcv_line)
            data_parsed = self.__parse_adv_data(data)

            '''
            adv_match = False
            if adv_filter:
                for f in adv_filter[common.ble_msgs.ADV_FILTER_KEY]:
                    for a in data_parsed:
                        adv_match = self.__check_adv_data_match(a, f)

                    if not adv_match:
                        return ''
            else:
                return data

            return data
            '''
            if self.__check_adv_against_filters(data_parsed, adv_filter):
                return data
            else:
                return ''
        else:
            return data

    def start_scan(self):
        msg = f'{common.ble_msgs.SCAN} {1}'
        ret = self.__send_rcv(msg, common.ble_msgs.OK, .5)
        self.__pass_fail(ret, msg, True)

    def scan_for_devices(self, adv_filter=None, rssi=0, scan_duration=10):
        if adv_filter is None:
            print('No valid filters...exiting')
            sys.exit()

        print('Scanning For Devices...')
        mac_filter, adv_data_filter, rssi_filter = self.__check_filters(adv_filter)

        msg = f'{common.ble_msgs.SCAN} {1}'
        ret = self.__send_rcv(msg, common.ble_msgs.OK, .5)
        self.__pass_fail(ret, msg, True)

        start_time = time.time()
        i = 0
        while True:
            rcv_line = self.__rcv(5.0)
            mac, data, rssi, add_type = self.__parse_adv_string(rcv_line)
            data_parsed = self.__parse_adv_data(data)

            if mac_filter is True:
                pass
            else:
                '''
                if adv_filter:
                   
                    for a in data_parsed:
                        adv_match = self.__check_adv_data_match(a, adv_filter[common.ble_msgs.ADV_FILTER_KEY])
                    '''
                adv_match = self.__check_adv_against_filters(data_parsed, adv_filter)

                if rssi_filter:
                    if rssi > adv_filter[common.ble_msgs.RSSI_FILTER_KEY]:
                        rssi_match = True
                else:
                    rssi_match = True

            if (adv_match is True) and (rssi_match is True):
                return mac, add_type

            time_elapsed = time.time() - start_time
            if time_elapsed > scan_duration:
                return '', ''

    @staticmethod
    def print_progress_bar(iteration, total, prefix='', suffix='', decimals=1, length=100, fill='█', print_end="\r"):
        percent = ("{0:." + str(decimals) + "f}").format(100 * (iteration / float(total)))
        filled_length = int(length * iteration // total)
        bar = fill * filled_length + '-' * (length - filled_length)
        print(f'\r{prefix} |{bar}| {percent}% {suffix}', end='')
        # Print New Line on Complete
        if iteration == total:
            print()

    def __gatt_read(self, handle):
        msg = f'{common.ble_msgs.GATT_READ} {0} {handle}'
        ack_msg = f'{common.ble_msgs.GATT_READ_EVT}: conn_idx:0000 handle:{handle}'
        ret, to = self.__send_rcv_block(msg, ack_msg, 5.0)
        self.__pass_fail(not to, msg, True)
        # should be next line after EVT_RECEIVED
        value = self.__rcv(1.0)
        return value.split('Value:')[1].strip()

    def __gatt_write(self, handle, props, value):
        msg_op = ''
        if props & PROPS_WRITE:
            msg_op = common.ble_msgs.GATT_WRITE
        if props & PROPS_WRITE_NO_RSP:
            # Let it fall through if both supported since this is faster
            msg_op = common.ble_msgs.GATT_WRITE_NO_RSP
        if msg_op == '':
            print("ERROR: Char has no property write!")

        msg = f'{msg_op} {value} {handle} {0}'
        ret, to = self.__send_rcv_block(msg, 'ATT_ERROR_OK', 5.0)
        self.__pass_fail(not to, msg, True)

    def read_chars(self, param):
        for char in param:
            for elem in self.database:
                if isinstance(elem, characteristic):
                    if char == elem.uuid:
                        value = self.__gatt_read(elem.val_handle)
                        print(f'Char:{elem.uuid} Value:{value}')

    def write_chars(self, param):
        for char in param:
            for elem in self.database:
                if isinstance(elem, characteristic):
                    if char["uuid"] == elem.uuid:
                        print(f'Writing {char["value"]} to Char {char["uuid"]}...')
                        self.__gatt_write(elem.val_handle, elem.props, char["value"])

    def start_suota(self):
        force_gatt_update = 0
        msg = f'{common.ble_msgs.SUOTA_START} {force_gatt_update}'
        ret = self.__send_rcv(msg, common.ble_msgs.OK, 2.0)

        self.print_progress_bar(0, self.image_size, prefix='Progress:', suffix='Complete', length=50)

        rcv_line = ''
        while common.ble_msgs.SUOTA_COMPLETE not in rcv_line:
            rcv_line = self.__rcv(1.0)

            if common.ble_msgs.SUOTA_PROGRESS in rcv_line:
                bytes_sent = int(rcv_line.split(common.ble_msgs.SUOTA_PROGRESS)[1].split('bytes')[0].strip(), 10)
                self.print_progress_bar(bytes_sent, self.image_size, prefix='Progress:', suffix='Complete', length=50)

        self.print_progress_bar(self.image_size, self.image_size, prefix='Progress:', suffix='Complete', length=50)

    def download_image(self, image):
        print("Downloading Image...")
        with open(image, 'rb') as fo:
            bytes_read = fo.read()

        chunk_size = 1024
        sector_size = 4096
        chunk_items = 0

        blocks = sector_size / chunk_size
        file_len = len(bytes_read)
        chunks = math.ceil(file_len / chunk_size)

        self.print_progress_bar(0, chunks, prefix='Progress:', suffix='Complete', length=50)
        for i in range(chunks):
            crc = 0xFFFF
            write_to_flash = 0

            if i == (chunks - 1):
                chunk_items = file_len % chunk_size
                write_to_flash = 1
            else:
                chunk_items = chunk_size
                if (i % blocks) == blocks - 1:
                    write_to_flash = 1

            addr = i * chunk_size
            mydata = (base64.b64encode(bytes_read[addr:addr + chunk_items])).rstrip(b'\n').decode()
            crc = self.crc16_sdk(bytes_read[addr:addr + chunk_items], crc)
            command_string = f'{common.ble_msgs.BLOCK_UPDATE} {mydata} {chunk_items} {addr} {crc} {write_to_flash}'
            ret = self.__send_rcv(command_string, common.ble_msgs.OK, timeout=5.0)
            self.__pass_fail(ret, command_string, True)
            self.print_progress_bar(i, chunks, prefix='Progress:', suffix='Complete', length=50)

        self.print_progress_bar(chunks, chunks, prefix='Progress:', suffix='Complete', length=50)
