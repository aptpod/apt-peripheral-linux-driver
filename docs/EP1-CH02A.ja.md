# Device Driver Documentation for EDGEPLANT CAN-USB Interface

## About

EDGEPLANT CAN-USB Interface は CAN、J1939 などの制御信号を取得するためのインターフェイスデバイスです。
製品の詳細については[こちら](https://www.aptpod.co.jp/products/edgeplant/edgeplant-peripherals/#edgeplant-can-interface)を参照してください。

EDGEPLANT USB Peripherals で共通している機能などについては、[こちら](./generic.ja.md)を参照してください。

## How to use with SocketCAN

Socket CAN を利用して CAN データの送受信を行う方法について記載します。Socket CAN の利用方法については、以下などを参考にしてください。

- https://www.kernel.org/doc/html/latest/networking/can.html

### Configuration

Socket CAN interface の設定を行います。以下は `can0` のビットレートを `500kbps` に設定する例です。

```sh
sudo ip link set can0 type can bitrate 500000
sudo ifconfig can0 up
```

ビットレートではなくビットタイミングによって動作設定することも可能です。

```sh
sudo ip link set can0 type can tq 133 prop-seg 6 phase-seg1 6 phase-seg2 2 sjw 1
```

CAN-USB Interface を受信のみで動作させたい場合は、`listen-only` を設定します。`listen-only` が `on` になっていると、ACK を含めた一切の CAN フレーム送信を行わないようになります。

```sh
sudo ifconfig can0 down
sudo ip link set can0 type can listen-only on
sudo ifconfig can0 up
```

### Receive can frame

`candump` を利用することで、CAN フレームを受信することができます。

```sh
candump -ta -H can0
```

### Send can frame

`cansend` を利用することで、CAN フレームを送信することができます。

```sh
cansend can0 001#1122334455667788
```

## How to use system calls

システムコールを利用してデバイスを操作する方法について記載します。

### Samples

サンプルコードは[こちら](../samples/EP1-CH02A)に用意しています。

### CAN Frame

システムコールで CAN フレームを送受信する際のデータの形式は以下の通りです。

```
 0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                          CAN ID                         |R|Typ|
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|  DLC  |Reserv.|                                               |
+-+-+-+-+-+-+-+-+                                               +
|                            CAN Data                           |
+               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|               |
+-+-+-+-+-+-+-+-+
```

CAN Type 部分は以下の形式になります。

```
0             29           30           31
+---//---+----------+------------+------------+
| CAN ID | Reserved | Frame Type |  ID Type   |
+---//---+----------+------------+------------+
```

各データの詳細は以下の通りです。

| name           | length | order | sign | description                            |
| :------------- | -----: | :---- | :--- | :------------------------------------- |
| CAN ID         |  29bit | LE    | U    | CAN ID                                 |
| CAN Frame Type |   1bit | -     | U    | CAN フレーム種別 (0:データ/1:フレーム) |
| CAN ID Type    |   1bit | -     | U    | CAN ID Type (0:標準 ID/1:拡張 ID)      |
| CAN DLC        |   4bit | -     | U    | CAN DLC                                |
| CAN Data       |  8byte | -     | U    | CAN データ                             |

### Read CAN data

ファイルディスクリプタ経由で read することで、CAN のデータを取得することができます。

```c
int fd;
unsigned char buffer[128];

fd = open(devpath, O_RDONLY);
ssize_t rsize = read(fd, buffer, sizeof(buffer));
```

デバイスから受信する CAN のパケットは以下の形式になります。

```
 0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                            Time Sec                           |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                           Time USec                           |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                          CAN Frame
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+//
```

各データの詳細は以下の通りです。

| name      | length | order | sign | description                      |
| :-------- | -----: | :---- | :--- | :------------------------------- |
| Time Sec  |  4byte | LE    | U    | 基準時刻からの経過時間（秒）     |
| Time USec |  4byte | LE    | U    | 基準時刻からの経過時間（ミリ秒） |
| CAN Frame |      - | -     | -    | [CAN フレーム](#can-frame)       |

### Write CAN data

ファイルディスクリプタ経由で write することで、CAN のデータを送信することができます。

```c
int fd;
unsigned char buffer[] = { /* CAN Frame */ };

fd = open(devpath, O_WRONLY);
ssize_t rsize = write(fd, buffer, sizeof(buffer));
```

デバイスへ送信する CAN のパケットは以下の形式になります。

```
 0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                          CAN Frame
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+//
```

各データの詳細は以下の通りです。

| name      | length | order | sign | description                |
| :-------- | -----: | :---- | :--- | :------------------------- |
| CAN Frame |      - | -     | -    | [CAN フレーム](#can-frame) |

### ioctl

ioctl に利用可能な値は、[apt_usbtrx_ioctl.h](../module/apt_usbtrx_ioctl.h) に定義されています。CAN-USB Interface で使用可能なものは以下になります。

EDGEPLANT USB Peripherals で共通している値は以下の通りです。詳細については、[こちら](./general.ja.md#ioctl)を参照してください。

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

CAN-USB Interface 固有の値は以下の通りです。

| request number                    | description                      |
| --------------------------------- | -------------------------------- |
| EP1_CH02A_IOCTL_GET_STATUS        | デバイスステータス取得           |
| EP1_CH02A_IOCTL_SET_MODE          | CAN モード設定                   |
| EP1_CH02A_IOCTL_SET_BIT_TIMING    | CAN ビットタイミング設定         |
| EP1_CH02A_IOCTL_GET_BIT_TIMING    | CAN ビットタイミング取得         |
| EP1_CH02A_IOCTL_RESET_CAN_SUMMARY | CAN 受信データサマリーのリセット |
| EP1_CH02A_IOCTL_START_CAN         | CAN 送受信の開始                 |
| EP1_CH02A_IOCTL_STOP_CAN          | CAN 送受信の停止                 |

### General return values

- 操作に成功した場合、ioctl の呼び出しに対して `0` を返します。
- 異常が発生した場合、ioctl の呼び出しに対して `-1` を返し、errno に発生したエラーに対応した値を設定します。

| errno     | description                              |
| --------- | ---------------------------------------- |
| ENODEV    | デバイスが存在しない                     |
| ESHUTDOWN | デバイスとの通信が切断されている         |
| EIO       | デバイスとの通信でエラーが発生した       |
| EFAULT    | ユーザ空間とのデータのやりとりに失敗した |

### EP1_CH02A_IOCTL_GET_STATUS

デバイスの状態を取得します。

#### Usage

```c
ep1_ch02a_ioctl_get_status_t status;
ioctl(fd, EP1_CH02A_IOCTL_GET_STATUS, &status);
```

#### Inputs

none

#### Outputs

`ep1_ch02a_ioctl_get_status_t` 型で返します。

### EP1_CH02A_IOCTL_SET_MODE

CAN 通信の設定を行います。

#### Usage

```c
ep1_ch02a_ioctl_set_mode_t mode;
ioctl(fd, EP1_CH0A_IOCTL_SET_MODE, &mode);
```

#### Inputs

`ep1_ch02a_ioctl_set_mode_t` 型で入力します。設定可能なビットレートは、33kbps、50kbps、83kbps、100kbps 、125kbps、250kbps、500kbps、1000kbps です。

#### Outputs

none

#### Notes

設定を行わない場合の初期値は、以下の通りになります。

| member   | value |
| -------- | ----- |
| silent   | false |
| baudrate | 500   |

### EP1_CH02A_IOCTL_SET_BIT_TIMING

CAN のビットタイミングの設定を行います。

#### Usage

```c
ep1_ch02a_ioctl_set_bit_timing_t timing;
ioctl(fd, EP1_CH0A_IOCTL_SET_BIT_TIMING, &timing);
```

#### Inputs

`ep1_ch02a_ioctl_set_bit_timing_t` 型で入力します。

#### Outputs

none

#### Notes

設定を行わない場合の初期値は、以下の通りになります。

| member     | value |
| ---------- | :---: |
| prop_seg   |   8   |
| phase_seg1 |   3   |
| phase_seg2 |   3   |
| sjw        |   3   |
| brp        |   4   |

### EP1_CH02A_IOCTL_GET_BIT_TIMING

CAN のビットタイミングの設定を取得します。

#### Usage

```c
ep1_ch02a_ioctl_get_bit_timing_t timing;
ioctl(fd, EP1_CH0A_IOCTL_GET_BIT_TIMING, &timing);
```

#### Inputs

none

#### Outputs

`ep1_ch02a_ioctl_get_bit_timing_t` 型で返します。

### EP1_CH02A_IOCTL_RESET_CAN_SUMMARY

デバイスが保持している CAN 受信に関する集計情報をクリアします。

#### Usage

```c
ioctl(fd, EP1_CH02A_IOCTL_RESET_CAN_SUMMARY);
```

#### Inputs

none

#### Outputs

none

### EP1_CH02A_IOCTL_START_CAN

CAN の通信を開始します。

#### Usage

```c
ioctl(fd, EP1_CH02A_IOCTL_START_CAN);
```

#### Inputs

none

#### Outputs

none

### EP1_CH02A_IOCTL_STOP_CAN

CAN の通信を停止します。

#### Usage

```c
ioctl(fd, EP1_CH02A_IOCTL_STOP_CAN);
```

#### Inputs

none

#### Outputs

none

## sysfs

デバイスが保持している CAN 受信に関する集計情報を一定周期でデバイスから受信します。受信したデータは sysfs 上に書き込まれます。書き込まれるデータは以下の通りです。

| file name     | mode | description                           |
| ------------- | ---- | ------------------------------------- |
| datcnt        | R    | 標準 ID の CAN データフレーム受信数   |
| ext_datcnt    | R    | 拡張 ID の CAN データフレーム受信数   |
| rtrcnt        | R    | 標準 ID の CAN リモートフレーム受信数 |
| ext_rtrcnt    | R    | 拡張 ID の CAN リモートフレーム受信数 |
| errcnt        | R    | エラーフレーム受信数                  |
| cnt_timestamp | R    | 最終更新日時                          |

sysfs のデバイスパスは以下のコマンドで表示できます。

```sh
$ udevadm info --query=path --name=/dev/aptUSB0
/devices/3530000.xhci/usb1/1-2/1-2.3/1-2.3.1/1-2.3.1:1.0/usbmisc/aptUSB0
```

各情報は `/sys/<DEVICE PATH>/device` にあります。例えば `datcnt` の場合は以下のコマンドで表示できます。

```sh
$ cat /sys/devices/3530000.xhci/usb1/1-2/1-2.3/1-2.3.1/1-2.3.1:1.0/usbmisc/aptUSB0/device/datcnt
43729
```
