#!/usr/bin/env python3
#
# Copyright (c) 2021, aptpod,Inc.
#

import argparse
import fcntl
import glob
import ctypes
import os
import re
import subprocess
import sys
import time


class AptTrxFwSize(ctypes.Structure):
    _fields_ = [("size", ctypes.c_int32)]


class AptTrxFwVersion(ctypes.Structure):
    _fields_ = [
        ("major", ctypes.c_int32),
        ("minor", ctypes.c_int32),
        ("patch", ctypes.c_int32),
    ]


class AptTrxConnector:
    def __init__(self, path):
        self.__path = path

    def __enter__(self):
        self.open()
        return self

    def __exit__(self, exception_type, exception_value, traceback):
        self.close()

    def open(self):
        self.__fd = os.open(self.__path, os.O_RDWR)
        if self.__fd == -1:
            print("Can not open {}.".format(self.__path))

    def close(self):
        os.close(self.__fd)
        self.__fd = None

    def write(self, data):
        os.write(self.__fd, data)

    def ioctl(self, req, arg=0):
        fcntl.ioctl(self.__fd, req, arg)


def make_io_req(nr):
    return ord("P") << 8 | nr


def make_ior_req(nr, size):
    return 2 << 30 | ord("P") << 8 | nr | size << 16


def show_version(dev_path):
    version = AptTrxFwVersion()
    with AptTrxConnector(dev_path) as conn:
        conn.ioctl(make_ior_req(0x28, ctypes.sizeof(version)), version)
    print(
        "Current firmware version: {}.{}.{}".format(
            version.major, version.minor, version.patch
        )
    )


def get_firmware_size(dev_path):
    fw_size = AptTrxFwSize()
    with AptTrxConnector(dev_path) as conn:
        conn.ioctl(make_ior_req(0x27, ctypes.sizeof(fw_size)), fw_size)
    return fw_size.size


def is_valid_firmware(dev_path, fw_file):
    if not os.path.exists(fw_file):
        print("Invalid firmware: file is not found.")
        return False

    if os.path.getsize(fw_file) != get_firmware_size(dev_path):
        print("Invalid firmware: size error.")
        return False
    return True


def get_udevinfo(path, key):
    info = subprocess.run(
        "udevadm info -a -n {}".format(path).split(),
        encoding="utf-8",
        stdout=subprocess.PIPE,
    )
    try:
        return re.search(r"(?<={}).*".format(key), info.stdout).group(0)
    except Exception as e:
        print("Can not get {}: {}".format(key, path))
        return ""


def get_serial_id(path):
    return "{}_{}".format(
        get_udevinfo(path, "ATTRS{product}=="), get_udevinfo(path, "ATTRS{serial}==")
    )


def wait_device_wakeup(serial, mode):
    GLOB_PATH = "/dev/apt{}*".format(mode)
    for _ in range(3):
        time.sleep(3)
        d = next(
            (f for f in glob.glob(GLOB_PATH) if get_serial_id(f) == serial),
            None,
        )
        if d is not None:
            return d
    print("Can not find {} device.".format(mode))
    return None


def wait_dfu_device_wakeup(serial):
    return wait_device_wakeup(serial, "DFU")


def wait_normal_device_wakeup(serial):
    return wait_device_wakeup(serial, "USB")


def put_into_dfu(dev_path, serial):
    with AptTrxConnector(dev_path) as conn:
        conn.ioctl(make_io_req(0x1F))
    return wait_dfu_device_wakeup(serial)


def read_each_block(fw_file):
    BLOCK_SIZE = 32
    with open(fw_file, "rb") as f:
        while True:
            d = f.read(BLOCK_SIZE)
            if len(d) == 0:
                break
            yield (d)


def write_firmware(dev_conn, fw_file):
    total = 0
    for d in read_each_block(fw_file):
        dev_conn.write(d)
        if total == 0:
            time.sleep(10)  # wait flash erase
        else:
            time.sleep(2 / 1000)  # write interval 2ms
        total += len(d)
        print("\rWrite firmware... {} bytes".format(total), end="")
    print("\nCompleted.")


def update_firmware(dev_path, serial, fw_file):
    dfu_path = put_into_dfu(dev_path, serial)
    if dfu_path == None:
        return False

    with AptTrxConnector(dfu_path) as conn:
        write_firmware(conn, fw_file)
    return True


def main():
    parser = argparse.ArgumentParser(description="Firmware updater.")
    parser.add_argument(
        "--version", help="show current firmware version.", action="store_true"
    )
    parser.add_argument(
        "--firmware", type=str, help="set firmware file path.", default=""
    )
    parser.add_argument("device", type=str, help="Target device path.")
    args = parser.parse_args()

    if args.version:
        show_version(args.device)
        return

    if not is_valid_firmware(args.device, args.firmware):
        sys.exit(1)

    show_version(args.device)

    serial = get_serial_id(args.device)
    if not update_firmware(args.device, serial, args.firmware):
        sys.exit(1)

    new_devpath = wait_normal_device_wakeup(serial)
    show_version(new_devpath)


if __name__ == "__main__":
    main()
