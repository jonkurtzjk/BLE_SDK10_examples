# This is a sample Python script.

# Press Shift+F10 to execute it or replace it with your code.
# Press Double Shift to search everywhere for classes, files, tool windows, actions, and settings.
import argparse
import pathlib

from common.ble_manager import BleHandler


def download_image(image, com_port):
    # Pass into argparse
    ble = BleHandler(com_port)
    ble.reset_device()
    ble.erase_image()
    ble.download_image(image)
    ble.verify_img()


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Download SUOTA Image")
    parser.add_argument("-img_file", metavar="I", type=str, help="Input File to download locally to BLE dongle")
    parser.add_argument("-com_port", metavar="C", type=str, help="Input File to download locally to BLE dongle")
    args = parser.parse_args()

    download_image(args.img_file, args.com_port)
