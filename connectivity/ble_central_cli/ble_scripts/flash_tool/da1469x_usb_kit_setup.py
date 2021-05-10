import argparse
import json
import logging
import os
import subprocess
import sys
import platform

import ezFlashCLI.cli
from ezFlashCLI.ezFlash.smartbond.smartbondDevices import da1469x


def flash_ez_flash(file):
    with open(os.path.join(os.path.dirname(os.path.abspath(ezFlashCLI.__file__)), 'flash_database.json')) as json_file:
        flash_db = json.load(json_file)

        json_file.close()

    try:
        fp = open(file, 'rb')
    except IOError:
        logging.error("Failed to open {}".format(file))
        sys.exit(1)

    file_data = fp.read()
    fp.close()

    da = da1469x()
    da.connect(None)
    dev = da.flash_probe()
    print('Erasing flash...')
    da.flash_erase()
    dev = da.flash_probe()
    flash = da.get_flash(dev, flash_db)
    print('Programming Binary...')
    da.flash_program_image(file_data, flash)
    print('Finished.')
    da.link.resetNoHalt()
    print("CLI Central Started!")


def flash_usb_dongle(com_port):
    op_sys = platform.system()
    bin_dir = os.getcwd() + '/binaries'
    bin_file = bin_dir + '/ble_central_cli_img.bin'
    cli_programmer = ''

    if op_sys == 'Linux':
        cli_programmer = os.path.join(bin_dir, 'cli_programmer')
    if op_sys == 'Darwin':
        cli_programmer = os.path.join(bin_dir, 'cli_programmer_mac')
    if op_sys == 'Windows':
        cli_programmer = os.path.join(bin_dir, 'cli_programmer.exe')

    if op_sys == '':
        print('Unsupported OS')
        sys.exit()

    cli_programmer_sub = f'{cli_programmer} {com_port} write_qspi 0x0 {bin_file}'

    ret = subprocess.call(cli_programmer_sub, shell=True)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Download CLI binary to Kit")
    parser.add_argument("-com_port", metavar="C", type=str, help="Com PORT if using serial loading, use only "
                                                                 "if the full image file was compiled properly")
    parser.add_argument("-file", metavar="C", type=str, help="Input File to download locally to BLE dongle",
                        required=True)
    args = parser.parse_args()

    flash_ez_flash(args.file)
