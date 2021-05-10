import argparse
import datetime
import json
import os
import sys

from common.ble_manager import BleHandler


def suota_update(com_port, filters):
    ble = BleHandler(com_port)

    # Always reset device to get to known state
    ble.reset_device()

    # Dongle won't verify a valid image before SUOTA, always call before hand
    ble.verify_img()
    mac = ble.scan_for_devices(filters, scan_duration=10)
    if mac == '':
        print('Device Not Found, Exiting...')
        sys.exit()

    ble.stop_scan()
    ble.connect_to_device(mac)
    # This is simply a GATT_BROWSE, but the dongle will locally handle SUOTA characteristics
    ble.prepare_suota()
    # SUOTA will reset device after so no need for disconnect after
    ble.start_suota()


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
