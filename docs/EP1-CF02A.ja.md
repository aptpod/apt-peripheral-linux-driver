# Device Driver Documentation for EDGEPLANT CAN FD USB Interface

## About

EDGEPLANT CAN FD USB Interface は CAN、CAN FDフレームを送受信するためのインターフェイスデバイスです。
製品の詳細については[こちら](https://www.aptpod.co.jp/products/edgeplant/edgeplant-peripherals/#edgeplant-can-fd-interface)を参照してください。

EDGEPLANT USB Peripherals で共通している機能などについては、[こちら](./generic.ja.md)を参照してください。

## Tools

EDGEPLANT CAN FD USB Interface 向けに以下のツールが使用できます。
ツールのソースコードについては、[こちら](../../tools/ep1_cf02a)を参照してください。

| ツール名 | 説明 |
| --- | --- |
| ep1_cf02a_set_host_resetts | 最後に行われたタイムスタンプリセット時のホストのMONOTONIC時刻をデバイスに設定します。 [apt_usbtrx_timesync_all.sh](../../tools/apt_usbtrx_timesync/apt_usbtrx_timesync_all.sh) 内部で使用します。 |
| ep1_cf02a_get_rtc_time | RTC時刻を取得します。 |
| ep1_cf02a_set_rtc_time | RTC時刻にホストの現在時刻を設定します。 |
| ep1_cf02a_get_store_data_list | 蓄積データのリストを表示します。 |
| ep1_cf02a_delete_store_data | 蓄積データを削除します。 |
| ep1_cf02a_init_store_data_media | 蓄積データメディアを初期化します。 |

## How to use with SocketCAN

Socket CAN を利用して CAN データの送受信を行う方法について記載します。Socket CAN の利用方法については、以下などを参考にしてください。

- https://www.kernel.org/doc/html/latest/networking/can.html

### Configuration

Socket CAN interface の設定を行います。以下は `can0` のアービトレーションフェーズのビットレートを `500kbps` に、データフェーズのビットレートを `2Mbps` 設定する例です。
サンプルポイントの設定 `sample-point` および `dsample-point` は省略可能です。省略した場合はCiA推奨値が使用されます。

```sh
sudo ip link set can0 down
sudo ip link set can0 type can fd on bitrate 500000 sample-point 0.875 dbitrate 2000000 dsample-point 0.750
sudo ip link set can0 up
```

ビットレートではなくビットタイミングによって動作設定することも可能です。

```sh
sudo ip link set can0 down
sudo ip link set can0 type can fd on \
  tq 50 prop-seg 14 phase-seg1 15 phase-seg2 10 sjw 1 \
  dtq 12 dprop-seg 7 dphase-seg1 8 dphase-seg2 4 dsjw 1
sudo ip link set can0 up
```

CAN FD USB Interface を受信のみで動作させたい場合は、`listen-only` を設定します。`listen-only` が `on` になっていると、ACK を含めた一切の CAN フレーム送信を行わないようになります。

```sh
sudo ip link set can0 down
sudo ip link set can0 type can listen-only on
sudo ip link set can0 up
```

ISO非標準のモードに切り替えたい場合は、 `fd-non-iso` を `on` に設定します。

```sh
sudo ip link set can0 down
sudo ip link set can0 type can fd-non-iso on
sudo ip link set can0 up
```

### Receive can frame

`candump` を利用することで、CAN フレームを受信することができます。

```sh
candump -ta -H -L can0
```

### Send can frame

`cansend` を利用することで、CAN、CAN FDフレームを送信することができます。

CAN FDフレームを送信する場合は、 `<can_id>##<flags>{data}` の形式で送信します。

`flags` は16進数1文字で指定します。
flagsのbit 0はBit Rate Switch(BRS)に該当し、高速ビットレートを使用する場合は1を、使用しない場合は0を指定します。

```sh
# ID=0x001 16バイトのCAN FDフレームを、高速ビットレートを使用して送信
cansend can0 001##100112233445566778899AABBCCDDEEFF

# ID=0x001 16バイトのCAN FDフレームを、高速ビットレートを使用せずに送信
cansend can0 001##000112233445566778899AABBCCDDEEFF
```

CANフレームを送信する場合は、 `<can_id>#{data}` の形式で送信します。

```sh
# ID=0x001 8バイトのCANフレームを送信
cansend can0 001#1122334455667788
```

## How to use system calls

システムコールを利用してデバイスを操作する方法について記載します。

### Samples

サンプルコードは[こちら](../samples/EP1-CF02A)に用意しています。

FIXME: サンプルコードの追加

### CAN Frame

システムコールで CAN フレームを送受信する際のデータの形式は以下の通りです。

```
 0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                          CAN ID                         |R|C|C|
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|  DLC  |Reserv.|     Flags     |      CAN Data
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+//
```

各データの詳細は以下の通りです。

| name           | length   | order | sign | description                            |
| :------------- | -------: | :---- | :--- | :------------------------------------- |
| CAN ID         |    29bit | LE    | U    | CAN ID                                 |
| Reserved       |     1bit | -     | U    |                                        |
| CAN Frame Type |     1bit | -     | U    | CAN フレーム種別 (0:データフレーム/1:リモートフレーム)<br>※CAN FD(FDF=1)の場合は常に0 |
| CAN ID Type    |     1bit | -     | U    | CAN ID Type (0:標準 ID/1:拡張 ID)       |
| CAN DLC        |     4bit | -     | U    | CAN DLC                                |
| Reserved       |     4bit | -     | U    |                                        |
| Flags          |     8bit | -     | U    | bit0: Bit Rate Switch(BRS) (0:無効/1:有効)  <br>bit1: Error State Indicator(ESI) (0:Error Active/1:Error Passive)<br>bit2: FD Format Indicator(FDF) (0:CAN/1:CAN FD) |
| CAN Data       |   512bit | -     | U    | CAN データ                              |

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
| Time Sec  |  4byte | LE    | U    | タイムスタンプ（秒）     |
| Time USec |  4byte | LE    | U    | タイムスタンプ（マイクロ秒） |
| CAN Frame |      - | -     | -    | [CAN フレーム](#can-frame)       |

EP1-CF02Aのタイムスタンプはタイムスタンプリセット時にタイムスタンプがゼロクリアされません。
以下の計算で、データ受信時刻を求めることができます。

データ受信時時刻 = [基準時刻](./general.ja.md#apt_usbtrx_ioctl_get_basetime) + (CANデータのタイムスタンプ - デバイスのタイムスタンプリセット時刻)

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

ioctl に利用可能な値は、[apt_usbtrx_ioctl.h](../module/apt_usbtrx_ioctl.h) に定義されています。CAN FD USB Interface で使用可能なものは以下になります。

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

CAN FD USB Interface 固有の値は以下の通りです。

リアルタイム用途

| request number                      | description                      |
| ----------------------------------- | -------------------------------- |
| EP1_CF02A_IOCTL_GET_SILENT_MODE     | サイレントモード取得 |
| EP1_CF02A_IOCTL_SET_SILENT_MODE     | サイレントモード設定 |
| EP1_CF02A_IOCTL_GET_FD_MODE         | FDモード取得 |
| EP1_CF02A_IOCTL_SET_FD_MODE         | FDモード設定 |
| EP1_CF02A_IOCTL_GET_ISO_MODE        | ISOモード取得 |
| EP1_CF02A_IOCTL_SET_ISO_MODE        | ISOモード設定 |
| EP1_CF02A_IOCTL_GET_BITRATE         | ビットレート取得 |
| EP1_CF02A_IOCTL_SET_BITRATE         | ビットレート設定 |
| EP1_CF02A_IOCTL_GET_DATA_BITRATE    | データフェーズビットレート取得 |
| EP1_CF02A_IOCTL_SET_DATA_BITRATE    | データフェーズビットレート設定 |
| EP1_CF02A_IOCTL_GET_BIT_TIMING      | ビットタイミング取得 |
| EP1_CF02A_IOCTL_SET_BIT_TIMING      | ビットタイミング設定 |
| EP1_CF02A_IOCTL_GET_DATA_BIT_TIMING | データフェーズビットタイミング取得 |
| EP1_CF02A_IOCTL_SET_DATA_BIT_TIMING | データフェーズビットタイミング設定 |
| EP1_CF02A_IOCTL_GET_TX_RX_CONTROL   | CAN送受信状態の取得 |
| EP1_CF02A_IOCTL_SET_TX_RX_CONTROL   | CAN送受信制御設定 |
| EP1_CF02A_IOCTL_RESET_CAN_SUMMARY   | CAN受信データサマリーのリセット |
| EP1_CF02A_IOCTL_GET_DEVICE_TIMESTAMP_RESET_TIME | デバイスタイムスタンプリセット時刻取得 |

蓄積データ取得用途

| request number                      | description                      |
| ----------------------------------- | -------------------------------- |
| EP1_CF02A_IOCTL_SET_HOST_TIMESTAMP_RESET_TIME | ホストタイムスタンプリセット時刻設定 |
| EP1_CF02A_IOCTL_GET_RTC_TIME | RTC時刻取得 |
| EP1_CF02A_IOCTL_SET_RTC_TIME | RTC時刻設定 |
| EP1_CF02A_IOCTL_GET_CURRENT_STORE_DATA_STATE | 蓄積データ現在状態取得 |
| EP1_CF02A_IOCTL_GET_STORE_DATA_ID_LIST_COUNT | 蓄積データIDリスト要素数取得 |
| EP1_CF02A_IOCTL_GET_STORE_DATA_ID_LIST | 蓄積データIDリスト取得 |
| EP1_CF02A_IOCTL_GET_STORE_DATA_META | 蓄積データメタ情報取得 |
| EP1_CF02A_IOCTL_GET_STORE_DATA_RX_CONTROL | 蓄積データCANフレーム受信状態の取得 |
| EP1_CF02A_IOCTL_SET_STORE_DATA_RX_CONTROL | 蓄積データCANフレーム受信制御設定 |
| EP1_CF02A_IOCTL_READ_STORE_DATA | 蓄積データ受信 |
| EP1_CF02A_IOCTL_DELETE_STORE_DATA | 蓄積データ削除 |
| EP1_CF02A_IOCTL_INIT_STORE_DATA_MEDIA | 蓄積データメディア初期化 |

### General return values

- 操作に成功した場合、ioctl の呼び出しに対して `0` を返します。
- 異常が発生した場合、ioctl の呼び出しに対して `-1` を返し、errno に発生したエラーに対応した値を設定します。

| errno     | description                              |
| --------- | ---------------------------------------- |
| ENODEV    | デバイスが存在しない                     |
| ESHUTDOWN | デバイスとの通信が切断されている         |
| EIO       | デバイスとの通信でエラーが発生した       |
| EFAULT    | ユーザ空間とのデータのやりとりに失敗した |

### EP1_CF02A_IOCTL_GET_SILENT_MODE

サイレントモードを取得します。

#### Usage

```c
ep1_cf02a_ioctl_get_silent_mode_t mode;
ioctl(fd, EP1_CF02A_IOCTL_GET_SILENT_MODE, &mode);
```

#### Inputs

none

#### Outputs

`ep1_cf02a_ioctl_get_silent_mode_t` 型で返します。

### EP1_CF02A_IOCTL_SET_SILENT_MODE

サイレントモードを設定します。

#### Usage

```c
ep1_cf02a_ioctl_set_silent_mode_t mode;
ioctl(fd, EP1_CF02A_IOCTL_SET_SILENT_MODE, &mode);
```

#### Inputs

`ep1_cf02a_ioctl_set_silent_mode_t` 型で入力します。

#### Outputs

none

#### Notes

設定しない場合の初期値は、以下の通りになります。

| member   | value |
| -------- | ----- |
| silent   | false |

### EP1_CF02A_IOCTL_GET_FD_MODE

FDモードを取得します。

#### Usage

```c
ep1_cf02a_ioctl_get_fd_mode_t mode;
ioctl(fd, EP1_CF02A_IOCTL_GET_FD_MODE, &mode);
```

#### Inputs

none

#### Outputs

`ep1_cf02a_ioctl_get_fd_mode_t` 型で返します。

### EP1_CF02A_IOCTL_SET_FD_MODE

FDモードを設定します。

#### Usage

```c
ep1_cf02a_ioctl_set_fd_mode_t mode;
ioctl(fd, EP1_CF02A_IOCTL_SET_FD_MODE, &mode);
```

#### Inputs

`ep1_cf02a_ioctl_set_fd_mode_t` 型で入力します。

#### Outputs

none

#### Notes

設定しない場合の初期値は、以下の通りになります。

| member   | value |
| -------- | ----- |
| fd | true |

### EP1_CF02A_IOCTL_GET_ISO_MODE

ISOモードを取得します。

#### Usage

```c
ep1_cf02a_ioctl_get_iso_mode_t mode;
ioctl(fd, EP1_CF02A_IOCTL_GET_ISO_MODE, &mode);
```

#### Inputs

none

#### Outputs

`ep1_cf02a_ioctl_get_iso_mode_t` 型で返します。

### EP1_CF02A_IOCTL_SET_ISO_MODE

ISOモードを設定します。

#### Usage

```c
ep1_cf02a_ioctl_set_iso_mode_t mode;
ioctl(fd, EP1_CF02A_IOCTL_SET_ISO_MODE, &mode);
```

#### Inputs

`ep1_cf02a_ioctl_set_iso_mode_t` 型で入力します。

#### Outputs

none

#### Notes

設定しない場合の初期値は、以下の通りになります。

| member   | value |
| -------- | ----- |
| non_iso_mode | false |

### EP1_CF02A_IOCTL_GET_BITRATE

アービトレーションフェーズのビットレートを取得します。

#### Usage

```c
ep1_cf02a_ioctl_get_bitrate_t bitrate;
ioctl(fd, EP1_CF02A_IOCTL_GET_BITRATE, &bitrate);
```

#### Inputs

none

#### Outputs

`ep1_cf02a_ioctl_get_bitrate_t` 型で返します。

### EP1_CF02A_IOCTL_SET_BITRATE

アービトレーションフェーズのビットレートを設定します。

#### Usage

```c
ep1_cf02a_ioctl_set_bitrate_t bitrate;
ioctl(fd, EP1_CF02A_IOCTL_SET_BITRATE, &bitrate);
```

#### Inputs

`ep1_cf02a_ioctl_set_bitrate_t` 型で入力します。

#### Outputs

none

#### Notes

設定しない場合の初期値は、以下の通りになります。

| member       | value |
| ------------ | :---: |
| bitrate      |  FIXME: TBD  |
| sample_point |   FIXME: TBD   |

FIXME: アービトレーションンフェーズのビットタイミング設定から計算する、サンプルポイント自動設定の場合は0とする

### EP1_CF02A_IOCTL_GET_DATA_BITRATE

データフェーズのビットレートを取得します。

#### Usage

```c
ep1_cf02a_ioctl_get_data_bitrate_t bitrate;
ioctl(fd, EP1_CF02A_IOCTL_GET_DATA_BITRATE, &bitrate);
```

#### Inputs

none

#### Outputs

`ep1_cf02a_ioctl_get_data_bitrate_t` 型で返します。

### EP1_CF02A_IOCTL_SET_DATA_BITRATE

データフェーズのビットレートを設定します。

#### Usage

```c
ep1_cf02a_ioctl_set_data_bitrate_t bitrate;
ioctl(fd, EP1_CF02A_IOCTL_SET_DATA_BITRATE, &bitrate);
```

#### Inputs

`ep1_cf02a_ioctl_set_data_bitrate_t` 型で入力します。

#### Outputs

none

#### Notes

設定しない場合の初期値は、以下の通りになります。

| member       | value |
| ------------ | :---: |
| bitrate      |   FIXME: TBD   |
| sample_point |   FIXME: TBD   |

FIXME: アービトレーションンフェーズのビットタイミング設定から計算する、サンプルポイント自動設定の場合は0とする

### EP1_CF02A_IOCTL_GET_BIT_TIMING

アービトレーションフェーズのビットタイミングを取得します。

#### Usage

```c
ep1_cf02a_ioctl_get_bit_timing_t timing;
ioctl(fd, EP1_CH0A_IOCTL_GET_BIT_TIMING, &timing);
```

#### Inputs

none

#### Outputs

`ep1_cf02a_ioctl_get_bit_timing_t` 型で返します。

### EP1_CF02A_IOCTL_SET_BIT_TIMING

アービトレーションフェーズのビットタイミングを設定します。

#### Usage

```c
ep1_cf02a_ioctl_set_bit_timing_t timing;
ioctl(fd, EP1_CH0A_IOCTL_SET_BIT_TIMING, &timing);
```

#### Inputs

`ep1_cf02a_ioctl_set_bit_timing_t` 型で入力します。

#### Outputs

none

#### Notes

設定しない場合の初期値は、以下の通りになります。

| member     | value |
| ---------- | :---: |
| prop_seg   |   8   |
| phase_seg1 |   3   |
| phase_seg2 |   3   |
| sjw        |   3   |
| brp        |   4   |

### EP1_CF02A_IOCTL_GET_DATA_BIT_TIMING

データフェーズのビットタイミングを取得します。

#### Usage

```c
ep1_cf02a_ioctl_get_data_bit_timing_t timing;
ioctl(fd, EP1_CF02A_IOCTL_GET_DATA_BIT_TIMING, &timing);
```

#### Inputs

none

#### Outputs

`ep1_cf02a_ioctl_get_data_bit_timing_t` 型で返します。

### EP1_CF02A_IOCTL_SET_DATA_BIT_TIMING

データフェーズのビットタイミングを設定します。

#### Usage

```c
ep1_cf02a_ioctl_set_data_bit_timing_t timing;
ioctl(fd, EP1_CF02A_IOCTL_SET_DATA_BIT_TIMING, &timing);
```

#### Inputs

`ep1_cf02a_ioctl_set_data_bit_timing_t` 型で入力します。

#### Outputs

none

#### Notes

設定しない場合の初期値は、以下の通りになります。

| member     | value |
| ---------- | :---: |
| prop_seg   |   FIXME: TBD   |
| phase_seg1 |   FIXME: TBD   |
| phase_seg2 |   FIXME: TBD   |
| sjw        |   FIXME: TBD   |
| brp        |   FIXME: TBD   |

### EP1_CF02A_IOCTL_GET_TX_RX_CONTROL

CAN送受信状態を取得します。

#### Usage

```c
ep1_cf02a_ioctl_get_tx_rx_control_t ;
ioctl(fd, EP1_CF02A_IOCTL_GET_TX_RX_CONTROL, &control);
```

#### Inputs

none

#### Outputs

`ep1_cf02a_ioctl_get_tx_rx_control_t` 型で返します。

### EP1_CF02A_IOCTL_SET_TX_RX_CONTROL

CAN送受信制御を設定します。

#### Usage

```c
ep1_cf02a_ioctl_set_tx_rx_control_t control;
ioctl(fd, EP1_CF02A_IOCTL_SET_TX_RX_CONTROL, &control);
```

#### Inputs

`ep1_cf02a_ioctl_set_tx_rx_control_t` 型で入力します。

#### Outputs

none

#### Notes

設定しない場合の初期値は、以下の通りになります。

| member   | value |
| -------- | ----- |
| start    | false |

### EP1_CF02A_IOCTL_RESET_CAN_SUMMARY

デバイスが保持している CAN 受信に関する集計情報をクリアします。

#### Usage

```c
ioctl(fd, EP1_CF02A_IOCTL_RESET_CAN_SUMMARY);
```

#### Inputs

none

#### Outputs

none

### EP1_CF02A_IOCTL_GET_DEVICE_TIMESTAMP_RESET_TIME
最後に行われたタイムスタンプリセット時のデバイスのタイムスタンプ値を取得します。  
EP1-CF02Aはタイムスタンプリセット時にタイムスタンプが0にリセットされないため、経過時間の計算にこの値を利用する必要があります。
#### Usage
```c
ep1_cf02a_ioctl_get_device_timestamp_reset_time_t reset_time;
ioctl(fd, EP1_CF02A_IOCTL_GET_DEVICE_TIMESTAMP_RESET_TIME, &reset_time);
```
#### Inputs
none
#### Outputs
`ep1_cf02a_ioctl_get_device_timestamp_reset_time_t` 型で返します。

### EP1_CF02A_IOCTL_SET_HOST_TIMESTAMP_RESET_TIME
最後に行われたタイムスタンプリセット時のホストのMONOTONIC時刻をEP1-CF02Aに設定します。  
設定した時刻は蓄積データに書き込まれ、既存計測がある場合の経過時間の計算に使用されます。  
蓄積データの取得が開始されていない場合は、エラーします。

#### Usage
```c
ioctl(fd, EP1_CF02A_IOCTL_SET_HOST_TIMESTAMP_RESET_TIME);
```
#### Inputs
none
#### Outputs
none

### EP1_CF02A_IOCTL_GET_RTC_TIME
RTC時刻を取得します。
#### Usage
```c
ep1_cf02a_ioctl_get_rtc_time_t rtc_time;
ioctl(fd, EP1_CF02A_IOCTL_GET_RTC_TIME, &rtc_time);
```
#### Inputs
none
#### Outputs
`ep1_cf02a_ioctl_get_rtc_time_t` 型で返します。

### EP1_CF02A_IOCTL_SET_RTC_TIME
RTC時刻にホストの現在時刻を設定します。
#### Usage
```c
ioctl(fd, EP1_CF02A_IOCTL_SET_RTC_TIME);
```
#### Inputs
none
#### Outputs
none

### EP1_CF02A_IOCTL_GET_CURRENT_STORE_DATA_STATE
現在の蓄積データの取得状態や、現在の蓄積データIDを取得します。
#### Usage
```c
ep1_cf02a_ioctl_get_current_store_data_state_t current_store_data_state;
ioctl(fd, EP1_CF02A_IOCTL_GET_CURRENT_STORE_DATA_STATE, &current_store_data_state);
```
#### Inputs
none
#### Outputs
`ep1_cf02a_ioctl_get_current_store_data_state_t` 型で返します。

### EP1_CF02A_IOCTL_GET_STORE_DATA_ID_LIST_COUNT
蓄積データIDリストの要素数を取得します。
#### Usage
```c
ep1_cf02a_ioctl_get_store_data_id_list_count_t store_data_id_list_count;
ioctl(fd, EP1_CF02A_IOCTL_GET_STORE_DATA_ID_LIST_COUNT, &store_data_id_list_count);
```
#### Inputs
none
#### Outputs
`ep1_cf02a_ioctl_get_store_data_id_list_count_t` 型で返します。

### EP1_CF02A_IOCTL_GET_STORE_DATA_ID_LIST
蓄積データIDリストを取得します。  
**事前に`id_list`のメモリを確保する必要があります**。
#### Usage
```c
ep1_cf02a_ioctl_get_store_data_id_list_t store_data_id_list;
ioctl(fd, EP1_CF02A_IOCTL_GET_STORE_DATA_ID_LIST, &store_data_id_list);
```
#### Inputs
`ep1_cf02a_ioctl_get_store_data_id_list_t` 型で入力します。  
`count`に要素数を入力します。  
`id_list`に、`count * EP1_CF02A_STORE_DATA_ID_MAX_LENGTH`バイトのメモリを確保します。
#### Outputs
`ep1_cf02a_ioctl_get_store_data_id_list_t` 型で返します。  
`id_list`に蓄積データIDのリストが格納されます。

### EP1_CF02A_IOCTL_GET_STORE_DATA_META
指定した蓄積データIDの蓄積データメタ情報を取得します。
#### Usage
```c
ep1_cf02a_ioctl_get_store_data_meta_t store_data_meta;

memcpy(store_data_meta.id, store_data_id_list.id_list[i], EP1_CF02A_STORE_DATA_ID_MAX_LENGTH);

ioctl(fd, EP1_CF02A_IOCTL_GET_STORE_DATA_META, &store_data_meta);
```
#### Inputs
`ep1_cf02a_ioctl_get_store_data_meta_t` 型で入力します。  
`id`に蓄積データIDを入力します。
#### Outputs
`ep1_cf02a_ioctl_get_store_data_meta_t` 型で返します。

### EP1_CF02A_IOCTL_GET_STORE_DATA_RX_CONTROL
指定した蓄積データIDのCAN受信状態を取得します。
#### Usage
```c
ep1_cf02a_ioctl_get_store_data_rx_control_t control;

memcpy(control.id, store_data_id_list.id_list[i], EP1_CF02A_STORE_DATA_ID_MAX_LENGTH);

ioctl(fd, EP1_CF02A_IOCTL_GET_STORE_DATA_RX_CONTROL, &control);
```
#### Inputs
`ep1_cf02a_ioctl_get_store_data_rx_control_t` 型で入力します。  
`id`に蓄積データIDを入力します。
#### Outputs
`ep1_cf02a_ioctl_get_store_data_rx_control_t` 型で返します。

### EP1_CF02A_IOCTL_SET_STORE_DATA_RX_CONTROL
指定した蓄積データIDのCAN受信開始/停止や、受信間隔を設定します。
#### Usage
```c
ep1_cf02a_ioctl_set_store_data_rx_control_t control;
ioctl(fd, EP1_CF02A_IOCTL_SET_STORE_DATA_RX_CONTROL, &control);
```
#### Inputs
`ep1_cf02a_ioctl_get_store_data_rx_control_t` 型で入力します。
#### Outputs
none

### EP1_CF02A_IOCTL_READ_STORE_DATA
蓄積データに保存されたCANフレームを取得します。  
**事前に`buffer`のメモリを確保する必要があります**。
#### Usage
```c
ep1_cf02a_ioctl_read_store_data_t store_data;
ioctl(fd, EP1_CF02A_IOCTL_READ_STORE_DATA, &store_data);
```
#### Inputs
`ep1_cf02a_ioctl_read_store_data_t` 型で入力します。  
`count`に`buffer`のバイト数を入力します。  
`buffer`に`count`バイトのメモリを確保します。
#### Outputs
`ep1_cf02a_ioctl_read_store_data_t` 型で返します。

### EP1_CF02A_IOCTL_DELETE_STORE_DATA
指定した蓄積データIDの蓄積データを削除します。
#### Usage
```c
ep1_cf02a_ioctl_delete_store_data_t delete_store_data;
ioctl(fd, EP1_CF02A_IOCTL_DELETE_STORE_DATA, &delete_store_data);
```
#### Inputs
`ep1_cf02a_ioctl_delete_store_data_t` 型で入力します。
#### Outputs
none

### 蓄積データメディア初期化
蓄積データを保存するメディアを初期化します。
#### Usage
```c
ioctl(fd, EP1_CF02A_IOCTL_INIT_STORE_DATA_MEDIA);
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
