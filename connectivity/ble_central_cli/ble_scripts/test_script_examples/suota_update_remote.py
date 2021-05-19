import argparse
import datetime
import json
import os
import sys
import time

from common.ble_manager import BleHandler


def suota_update(com_port, filters):
    ble = BleHandler(com_port)

    # Always reset device to get to known state
    ble.reset_device()
    rev = ble.get_cli_fw_version()
    print(f'CLI APP Version: {rev}')

    # Dongle won't verify a valid image before SUOTA, always call before hand
    ble.verify_img()
    mac, add_type = ble.scan_for_devices(filters, scan_duration=10)
    if mac == '':
        print('Device Not Found, Exiting...')
        sys.exit()

    ble.stop_scan()
    ble.connect_to_device(mac, add_type, 10, 0, 1000)
    # ble.connect_to_device(mac, add_type)
    # This is simply a GATT_BROWSE, but the dongle will locally handle SUOTA characteristics
    time.sleep(.2)
    ble.prepare_suota()
    begin_time = datetime.datetime.now()
    # SUOTA will reset device after so no need for disconnect after
    ble.start_suota()

    print(f'SUOTA Completed In {datetime.datetime.now() - begin_time} seconds')


if __name__ == '__main__':
    begin_time = datetime.datetime.now()
    parser = argparse.ArgumentParser(description="Download SUOTA Image")
    parser.add_argument("-com_port", metavar="C", type=str, help="Input File to download locally to BLE dongle")
    args = parser.parse_args()

    scan_filters = os.getcwd() + "/scan_filters.json"
    f = open(scan_filters)
    data = json.load(f)

    suota_update(args.com_port, data)

    print(f'Script Completed In {datetime.datetime.now() - begin_time} seconds')
