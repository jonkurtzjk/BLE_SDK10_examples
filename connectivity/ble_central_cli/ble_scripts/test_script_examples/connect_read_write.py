import argparse
import datetime
import json
import os
import sys
import time

import common
from common.ble_manager import BleHandler


def connect_config(com_port, filters):
    ble = BleHandler(com_port)

    # Always reset the device to get the dongle to a known starting point
    ble.reset_device()
    mac = ble.scan_for_devices(filters, scan_duration=10)
    if mac == '':
        print('Device Not Found, Exiting...')
        sys.exit()

    ble.stop_scan()
    ble.connect_to_device(mac)
    # always browse database to find services and characteristics
    ble.browse_database()
    
    if common.ble_msgs.READ_CHARS_KEY in filters:
        ble.read_chars(filters[common.ble_msgs.READ_CHARS_KEY])

    if common.ble_msgs.WRITE_CHARS_KEY in filters:
        ble.write_chars(filters[common.ble_msgs.WRITE_CHARS_KEY])

    # Sleep for 5 seconds for proximity reporter, so the LED stays lit
    time.sleep(5.0)
        
    ble.disconnect()


if __name__ == '__main__':
    begin_time = datetime.datetime.now()
    parser = argparse.ArgumentParser(description="Download SUOTA Image")
    parser.add_argument("-com_port", metavar="C", type=str, help="Input File to download locally to BLE dongle")
    args = parser.parse_args()

    scan_filters = os.getcwd() + "/connect_config.json"
    f = open(scan_filters)
    data = json.load(f)

    connect_config(args.com_port, data)

    print(f'Script Completed In {datetime.datetime.now() - begin_time} seconds')
