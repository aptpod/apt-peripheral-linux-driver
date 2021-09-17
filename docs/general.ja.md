# EDGEPLANT USB Peripherals General Information

## About

EDGEPLANT USB Peripherals で共通している機能などについて記載します。

## How to sync timestamp

同期ケーブルで接続されているデバイス間でタイムスタンプの同期を行なう場合は、[apt_usbtrx_timesync_all.sh](../tools/apt_usbtrx_timesync/apt_usbtrx_timesync_all.sh) を実行してください。

実行例:

```sh
$ sudo ./apt_usbtrx_timesync_all.sh
enable timestamp reset for /dev/aptUSB0 (source)
enable timestamp reset for /dev/aptUSB1 (source)
enable timestamp reset for /dev/aptUSB2 (external)
enable timestamp reset for /dev/aptUSB3 (external)
reset timestamp of /dev/aptUSB0
basetime: 2133.184170688
```

## Firmware upgrade

### Prerequisites

ファームウェアアップデート用のツールを利用するためには、以下のパッケージが必要となります。

- Python3 (>= 3.6)

### How to upgrade

デバイスのファームウェアアップデートを行う場合は、[apt_usbtrx_fwupdate.py](../tools/apt_usbtrx_fwupdate.py) を利用してください。

```sh
$ sudo ./apt_usbtrx_fwupdate.py --firmware <FIRMWARE FILE> /dev/aptUSB0
Current firmware version: 2.3.0
Write firmware... 393216 bytes
Completed.
Current firmware version: 2.3.1
```

アップデート前に現在のファームウェアのバージョンを取得したい場合は、`version` オプションを利用してください。

```sh
$ sudo ./apt_usbtrx_fwupdate.py --version /dev/aptUSB0
Current firmware version: 2.3.1
```

アップデート用のファームウェアは[製品のホームページ](https://www.aptpod.co.jp/products/edgeplant/edgeplant-peripherals)にて公開されますので、そちらを参照してください。

## ioctl

ioctl に利用可能な値は、[apt_usbtrx_ioctl.h](../module/apt_usbtrx_ioctl.h) に定義されています。EDGEPLANT USB Peripherals で共通の定義は以下の通りです。

| request number                           | description                  |
| ---------------------------------------- | ---------------------------- |
| APT_USBTRX_IOCTL_GET_DEVICE_ID           | ユニークデバイス ID 取得     |
| APT_USBTRX_IOCTL_GET_FW_VERSION          | DEPRECATED, DO NOT USE       |
| APT_USBTRX_IOCTL_GET_FW_VERSION_REVISION | ファームウェアバージョン取得 |
| APT_USBTRX_IOCTL_ENABLE_RESET_TS         | タイムスタンプリセット有効化 |
| APT_USBTRX_IOCTL_RESET_TS                | タイムスタンプリセット       |
| APT_USBTRX_IOCTL_GET_SERIAL_NO           | シリアルナンバー取得         |
| APT_USBTRX_IOCTL_RESET_DEVICE            | デバイス再起動               |
| APT_USBTRX_IOCTL_SET_TIMESTAMP_MODE      | タイムスタンプモード設定     |
| APT_USBTRX_IOCTL_GET_TIMESTAMP_MODE      | タイムスタンプモード取得     |
| APT_USBTRX_IOCTL_SET_BASETIME            | OBSOLETE, DO NOT USE         |
| APT_USBTRX_IOCTL_GET_BASETIME            | 基準時刻取得                 |

### General return values

- 操作に成功した場合、ioctl の呼び出しに対して `0` を返します。
- 異常が発生した場合、ioctl の呼び出しに対して `-1` を返し、errno に発生したエラーに対応した値を設定します。

| errno     | description                              |
| --------- | ---------------------------------------- |
| ENODEV    | デバイスが存在しない                     |
| ESHUTDOWN | デバイスとの通信が切断されている         |
| EIO       | デバイスとの通信でエラーが発生した       |
| EFAULT    | ユーザ空間とのデータのやりとりに失敗した |

### APT_USBTRX_IOCTL_GET_DEVICE_ID

対象のデバイスのユニークデバイス ID を取得します。

#### Usage

```c
apt_usbtrx_ioctl_get_device_id_t device_id;
ioctl(fd, APT_USBTRX_IOCTL_GET_DEVICE_ID, &device_id);
```

#### Inputs

none

#### Outputs

`apt_usbtrx_ioctl_get_device_id_t` 型で返します。

### APT_USBTRX_IOCTL_GET_FW_VERSION

_DEPRECATED, DO NOT USE_

マイナーバージョンまでしか取得できないため、`APT_USBTRX_IOCTL_GET_FW_VERSION_REVISION` を利用してください。

### APT_USBTRX_IOCTL_GET_FW_VERSION_REVISION

対象のデバイスにインストールされているファームウェアのバージョンを取得します。

#### Usage

```c
apt_usbtrx_ioctl_get_fw_version_revision_t version;
ioctl(fd, APT_USBTRX_IOCTL_GET_FW_VERSION_REVISION, &version);
```

#### Inputs

none

#### Outputs

`apt_usbtrx_ioctl_get_fw_version_revision_t` 型で返します。

### APT_USBTRX_IOCTL_ENABLE_RESET_TS

デバイスが保持しているタイムスタンプをリセットする機能を有効にします。

#### Usage

```c
ioctl(fd, APT_USBTRX_IOCTL_ENABLE_RESET_TS);
```

#### Inputs

none

#### Outputs

none

#### Errors

- EIO タイムスタンプリセット機能を有効にできなかった

#### Notes

APT_USBTRX_IOCTL_RESET_TS でタイムスタンプをリセットする前に実行します。

### APT_USBTRX_IOCTL_RESET_TS

デバイスが保持しているタイムスタンプをリセットします。

#### Usage

```c
apt_usbtrx_ioctl_reset_ts_t ts;
ioctl(fd, APT_USBTRX_IOCTL_RESET_TS, &ts);
```

#### Inputs

none

#### Outputs

`apt_usbtrx_ioctl_reset_ts_t` 型で、タイムスタンプをリセットした時刻を返します。

#### Errors

- EIO タイムスタンプのリセットを行えなかった

#### Notes

タイムスタンプのリセットを行う場合は、事前に APT_USBTRX_IOCTL_ENABLE_RESET_TS によってリセット機能の有効化を行っている必要があります。また、同期ケーブルによって複数台のデバイスが接続されている場合は、クロックソースとなっているデバイスにリセットを行うことで、デバイス間でタイムスタンプの同期が行なわれます。具体的な手順は以下の通りです。

1. デバイス同士を同期ケーブルで接続します。このとき、接続の先頭にあたるデバイス（SYNC INが接続されておらず、SYNC OUTが他のデバイスに接続されているデバイス）がクロックソースとなります。
2. 全デバイスを同時、もしくは接続の先頭から順番に USB 接続します。
3. 全デバイスでタイムスタンプリセット機能を有効化します。
4. クロックソースのデバイスでタイムスタンプのリセットを行います。

上記の 3.以降の手順は [apt_usbtrx_timesync_all.sh](../tools/apt_usbtrx_timesync/apt_usbtrx_timesync_all.sh) で実装されているので、こちらも参照ください。

### APT_USBTRX_IOCTL_GET_SERIAL_NO

対象のデバイスに付与されたシリアルナンバーを取得します。また、このリクエストでは同時にデバイスの同期の状態も取得します。

#### Usage

```c
apt_usbtrx_ioctl_get_serial_no_t serial;
ioctl(fd, APT_USBTRX_IOCTL_GET_SERIAL_NO, &serial);
```

#### Inputs

none

#### Outputs

`apt_usbtrx_ioctl_get_serial_no_t` 型で返します。

#### Notes

デバイスの同期状態は、同期ケーブルの接続方法によって決まります。

- 同期ケーブルが非接続 : APT_USBTRX_SYNC_PULSE_SOURCE
- 同期ケーブルが接続
  - SYNC IN に同期ケーブルが非接続(同期の先頭) : APT_USBTRX_SYNC_PULSE_SOURCE
  - SYNC IN に同期ケーブルが接続 : APT_USBTRX_SYNC_PULSE_EXTERNAL

### APT_USBTRX_IOCTL_RESET_DEVICE

対象のデバイスを再起動します。

#### Usage

```c
ioctl(fd, APT_USBTRX_IOCTL_RESET_DEVICE);
```

#### Inputs

none

#### Outputs

none

#### Notes

デバイスを再起動した場合、デバイスの各種設定値は初期化されます。

### APT_USBTRX_IOCTL_SET_TIMESTAMP_MODE

受信したデータに対して、タイムスタンプをどのように付与するかを設定します。

#### Usage

```c
apt_usbtrx_ioctl_set_timestamp_mode_t mode;
ioctl(fd, APT_USBTRX_IOCTL_SET_TIMESTAMP_MODE, &mode);
```

#### Inputs

`apt_usbtrx_ioctl_set_timestamp_mode_t` 型で入力します。

| value                            | description                                  |
| -------------------------------- | -------------------------------------------- |
| APT_USBTRX_TIMESTAMP_MODE_DEVICE | デバイスの機能を利用してタイムスタンプを付与 |
| APT_USBTRX_TIMESTAMP_MODE_HOST   | ホスト側の時刻を利用してタイムスタンプを付与 |

#### Outputs

none

#### Errors

- EINVAL 範囲外の設定値が指定された
- EBUSY デバイスがデータ入力状態になっている

#### Notes

APT_USBTRX_TIMESTAMP_MODE_DEVICE を利用する場合、同期ケーブルを利用することでデバイス間でのタイムスタンプを同期することができます。ただしこの場合、クロック差などの要因でデバイスとホスト間で少しずつ時間のずれが発生していきます。ホストの時間との同期が必要となる場合は、APT_USBTRX_TIMESTAMP_HOST を利用してください。

### APT_USBTRX_IOCTL_GET_TIMESTAMP_MODE

受信したデータへのタイムスタンプの付与方法を取得します。

#### Usage

```c
apt_usbtrx_ioctl_get_timestamp_mode_t mode;
ioctl(fd, APT_USBTRX_IOCTL_GET_TIMESTAMP_MODE, &mode);
```

#### Inputs

none

#### Outputs

`apt_usbtrx_ioctl_set_timestamp_mode_t` 型で返します。

### APT_USBTRX_IOCTL_SET_BASETIME

_OBSOLETE, DO NOT USE_

基準時刻は APT_USBTRX_IOCTL_RESET_TS でタイムスタンプのリセットを行った際に設定されるため、こちらを利用しないでください。

### APT_USBTRX_IOCTL_GET_BASETIME

基準時刻を取得します。

#### Usage

```c
apt_usbtrx_ioctl_get_basetime_t basetime;
ioctl(fd, APT_USBTRX_IOCTL_GET_BASETIME, &basetime);
```

#### Inputs

none

#### Outputs

`apt_usbtrx_ioctl_get_basetime_t` 型で返します。

#### Notes

データに付与されているタイムスタンプは、基準時刻からの相対時間です。 データを受信した実際の時刻は、この基準時刻に対してデータのタイムスタンプを加算することで求めることができます。
