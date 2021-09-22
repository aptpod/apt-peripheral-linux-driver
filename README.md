# Device driver for EDGEPLANT USB Peripherals

## About

EDGEPLANT USB peripheral device driver under Linux allows you to receive and send data with the following products.

- EDGEPLANT CAN-USB Interface
- EDGEPLANT ANALOG-USB Interface

For more information about EDGEPLANT Peripherals, please visit https://www.aptpod.co.jp/products/edgeplant/edgeplant-peripherals .

## Getting started

### Prerequisites

To build this device driver, the following packages are required.

- GNU C
- GNU make
- Linux headers

### Installation

1. Clone this repository.

   ```sh
   git clone https://github.com/aptpod/apt-peripheral-linux-driver
   ```

1. Build the device driver and tools.

   ```sh
   make all
   ```

1. Install the device driver and tools.

   ```sh
   sudo make install
   sudo modprobe apt_usbtrx
   ```

   By default, the tools will be installed in `/usr/local/bin`. You can set the installation directory by using the `DESTDIR` variable.

   ```sh
   mkdir -p /path/to/dir/bin
   make DESTDIR=/path/to/dir install
   ```

### Running the unit tests

This project uses [KUnit](https://www.kernel.org/doc/html/latest/dev-tools/kunit/index.html) for unit testing. If you want to run the unit tests, execute the following script.

```sh
cd kunit && ./run_test.sh
```

## Usage

For information on how to use the peripheral devices, see [docs](./docs/README.md).

## License

Distributed under the GPL v2.0 License. See LICENSE for more information.

## Contact

https://www.aptpod.co.jp/contact/
