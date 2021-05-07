# BLE Test Scripts (PythonCode)

This project consists of multiple ble test scripts that interact with the ble_central_cli.bin
running on the DA1469x development Kit.  

The **PythonCode** illustrate mechanisms for interacting with the BLE dongle for test type 
applications.

The following test applications can be used to get started:
- connect_read_write.py
- update_local_img.py
- suota_update_remote.py


## Quick start

We recommend Python 3, and Python 3.8 has been tested
Install [Python 3.8.x](https://www.python.org/) on your system and then verify
your python version at the terminal or command line:

    > python -VV
    Python 3.8.0 (tags/v3.8.0:fa919fd, Oct 14 2019, 19:37:50) [MSC v.1916 64 bit (AMD64)]

Ensure that you have Python 3.8.x  and 64-bit.

If the ble_central_cli.bin has not been downloaded to a Dialog Dev kit.  Please use one of 
the following tools to download the binary.

- [EZFlashCLI](https://pypi.org/project/ezFlashCLI/#files)
- [EZFlash GUI](https://github.com/ezflash/ezFlashApp)


### Install Requirements

Extract the python code and install necessary requirements with:

    pip3 install -U -r requirements.txt

#### To Run:

First, run the da1469x_usb_kit_setup.py.  Specify the COM port as a parameter and the binary folder
('./binaries/ble_central_cli_img.bin').  Once loaded, press the reset button on the development kit
to start the downloaded code. 

There are multiple entry points for each script and be run with the following, e.g.:

    python connect_read_write.py    (Windows)
    python3 connect_read_write.py    (MAC / Linux)

The programs will require arguments as followings

    python3 connect_read_write.py -com_port COM_PORT
    python3 update_local_image.py -com_port COM_PORT -img_file UPDATE_IMAGE_FILE
    python3 suota_update_remote.py -com_port COM_PORT

### Script Descriptions

#### da1469x_usb_kit_setup.py
This file will load the USB dongle code into the DA1469x basic kit.

#### connect_read_write.py

This file will read a json file titled connect_config.json.  The JSON file will have advertising
filters, that the script uses to select a certain advertiser and connect to the device.
Once connected, it will do a service discovery, then do a GATT_READ on the characteristics
specified in the JSON file.  Once the characteristics are read, the script will do a 
GATT_WRITE on characteristics that are specified to be written in the JSON file.  

If run with the pxp_reporter_test.img, it will read the battery voltage and the manufacturer
name from the Device Information Service.  Following, it will write the Immediate Alert and 
the Link Loss Services, which will cause an LED to blink on the Dialog Dev Kits.  

#### update_local_img.py

This script is intended to be run prior to any batch SUOTA updates.  It will take an image file
as an argument and then download it locally BLE Central CLI client.  Once complete, this 
image will be used from NVM to update any clients it connects to in the future. 

For this example, pxp_reporter.img can be downloaded for the next script.  

#### suota_update_remote.py 

This script will take in the JSON file scan_filters.json.  From here, it will filter based 
on rssi and advertising data and connecto to this device.  It will do a service discovery, 
on connection followed by the SUOTA process to update the remote.   


