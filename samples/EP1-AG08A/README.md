# Code Samples for EDGEPLANT ANALOG-USB Interface

## Prerequisites

- _Install device driver_. Please read [this](../README.md#installation).

## Build samples

```sh
make all
```

## Read analog data packets

This sample reads analog data packets from the device `/dev/aptUSB0`.

### Source code

- [read_analog.c](./read_analog.c)

### Example

The analog data packets from the ANALOG-USB Interface are output as shown below.

```sh
$ ./read_analog
[4176.319527709] ch num: 3, data:  50, -13, -7,
```
