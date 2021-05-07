import argparse
import os
import subprocess
import sys
import time
import platform

import ezFlashCLI


def flash_ez_flash():
    # ez_flash = f'ezFlashCLI probe'
    ez_flash = ezFlashCLI()
    ret = subprocess.call(ez_flash, shell=True)


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
    parser.add_argument("-com_port", metavar="C", type=str, help="Input File to download locally to BLE dongle")
    args = parser.parse_args()

    flash_usb_dongle(args.com_port)
