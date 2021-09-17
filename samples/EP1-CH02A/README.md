# Code Samples for EDGEPLANT CAN-USB Interface

## Prerequisites

- _Install the device driver_. Please read [this](../README.md#installation).

## Build samples

```sh
make all
```

## Read CAN packets

This sample reads CAN packets from the device `/dev/aptUSB0`.

### Source code

- [read_can.c](./read_can.c)

### Example

The data read from the CAN bus is output as shown below.

```sh
$ ./read_can
[70388.301658099] id: 2, dlc: 8, data: 00 00 00 00 00 00 00 01
[70388.301910099] id: 3, dlc: 8, data: 00 00 00 00 00 00 00 01
[70388.302404099] id: 1, dlc: 8, data: 00 00 00 00 00 00 00 02
[70388.302657099] id: 2, dlc: 8, data: 00 00 00 00 00 00 00 02
[70388.302911099] id: 3, dlc: 8, data: 00 00 00 00 00 00 00 02
```

## Write CAN packets

This sample wirtes CAN packets to the device `/dev/aptUSB1`.

### Source code

- [write_can.c](./write_can.c)

### Example

By executing the samples as shown below, `write_can` writes CAN data to `/dev/aptUSB1` and `read_can` reads that data from `/dev/aptUSB0`.

```sh
$ ./read_can &
$ ./write_can
[70507.870931099] id: 1, dlc: 8, data: 02 03 04 05 06 07 08 09
```
