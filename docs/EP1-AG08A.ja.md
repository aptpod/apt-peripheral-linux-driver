# Device Driver Documentation for EDGEPLANT ANALOG-USB Interface

## About

EDGEPLANT ANALOG-USB Interface はアナログセンサーに接続し、制御信号を取得するためのインターフェイスデバイスです。
製品の詳細については[こちら](https://www.aptpod.co.jp/products/edgeplant/edgeplant-peripherals/#edgeplant-analog-interface)を参照してください。

EDGEPLANT USB Peripherals で共通している機能などについては、[こちら](./general.ja.md)を参照してください。

## How to use system calls

システムコールを利用してデバイスを操作する方法について記載します。

### Samples

サンプルコードは[こちら](../samples/EP1-AG08A)に用意しています。

### Read analog data

ファイルディスクリプタ経由で read することで、アナログデータを取得することができます。

```c
int fd;
unsigned char buffer[128];

fd = open(devpath, O_RDONLY);
ssize_t rsize = read(fd, buffer, sizeof(buffer));
```

デバイスから受信するアナログデータは以下の形式になります。

```
 0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                            Time Sec                           |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                           Time USec                           |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
| Channel       |                                               |
+-+-+-+-+-+-+-+-+                                               |
|                          Analog Data                          |
~                                                               ~
```

各データの詳細は以下の通りです。

| name        | length | order | sign | description                                        |
| :---------- | -----: | :---- | :--- | :------------------------------------------------- |
| Time Sec    |  4byte | LE    | U    | 基準時刻からの経過時間（秒）                       |
| Time USec   |  4byte | LE    | U    | 基準時刻からの経過時間（ミリ秒）                   |
| Channel     |  1byte | -     | -    | チャンネルごとの起動状態 (0: Started/ 1: Stopped)  |
| Analog Data |      - | LE    | -    | アナログデータ。2byte x 起動状態にあるチャンネル数 |

Channel の詳細は以下の通りです。各ビットに、チャンネルごとの起動状態が入っています。

```
   0     1     2          7
+-----+-----+-----+-//-+-----+
| CH0 | CH1 | CH2 |    | CH7 |
+-----+-----+-----+-//-+-----+
```

Analog Data には、起動しているチャンネル分のデータがチャンネル番号順に入っています。例えば、CH0、3、7 のみが起動している場合、データは以下のようになります。

```
+--------------------------+
| CH0 Analog Data (2byte)  |
+--------------------------+
| CH3 Analog Data (2byte)  |
+--------------------------+
| CH7 Analog Data (2byte)  |
+--------------------------+
```

#### Converting numerical values to voltage values

入力電圧設定ごとのアナログデータと電圧値の対応は以下の通りです。

| range | min           | max         |
| ----- | ------------- | ----------- |
| +-10V | -10V = -32767 | 10V = 32767 |
| +-5V  | -5V = -32767  | 5V = 32767  |
| +5V   | 0V = 0        | 5V = 65535  |

### ioctl

ioctl に利用可能な値は、[apt_usbtrx_ioctl.h](../module/apt_usbtrx_ioctl.h) に定義されています。ANALOG-USB Interface で使用可能なものは以下になります。

EDGEPLANT USB Peripherals で共通している値は以下の通りです。詳細については、[こちら](./generic.ja.md#ioctl)を参照してください。

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

ANALOG-USB Interface 固有の値は以下の通りです。

| request number                        | description             |
| ------------------------------------- | ----------------------- |
| EP1_AG08A_IOCTL_GET_STATUS            | デバイスステータス取得  |
| EP1_AG08A_IOCTL_SET_ANALOG_INPUT      | アナログ入力設定        |
| EP1_AG08A_IOCTL_CONTROL_ANALOG_INPUT  | アナログ入力の開始/停止 |
| EP1_AG08A_IOCTL_SET_ANALOG_OUTPUT     | アナログ出力設定        |
| EP1_AG08A_IOCTL_CONTROL_ANALOG_OUTPUT | アナログ出力の開始/停止 |

### General return values

- 操作に成功した場合、ioctl の呼び出しに対して `0` を返します。
- 異常が発生した場合、ioctl の呼び出しに対して `-1` を返し、errno に発生したエラーに対応した値を設定します。

| errno     | description                              |
| --------- | ---------------------------------------- |
| ENODEV    | デバイスが存在しない                     |
| ESHUTDOWN | デバイスとの通信が切断されている         |
| EIO       | デバイスとの通信でエラーが発生した       |
| EFAULT    | ユーザ空間とのデータのやりとりに失敗した |

### EP1_AG08A_IOCTL_GET_STATUS

デバイスの状態を取得します。

#### Usage

```c
ep1_ag08a_ioctl_get_status_t status;
ioctl(fd, EP1_AG08A_IOCTL_GET_STATUS, &status);
```

#### Inputs

none

#### Outputs

`ep1_ag08a_ioctl_get_status_t` 型で返します。

### EP1_AG08A_IOCTL_SET_ANALOG_INPUT

アナログ入力の設定を行います。全チャンネル共通のサンプリング周波数と、チャンネルごとに設定可能な入力電圧範囲を設定します。

#### Usage

```c
ep1_ag08a_ioctl_set_analog_input_t settings;
ioctl(fd, EP1_AG08A_IOCTL_SET_ANALOG_INPUT, &settings);
```

#### Inputs

`ep1_ag08a_ioctl_set_analog_input_t` 型で入力します。

#### Outputs

none

#### Notes

入力電圧の範囲はチャンネルごとに最大および最小で設定します。以下の3つのmax-minの組み合わせのいずれかを設定してください。

| range | max | min  |
| ----- | --- | ---- |
| +-10V | 10V | -10V |
| +-5V  | 5V  | -5V  |
| +5V   | 5V  | 0V   |

#### Errors

- EBUSY デバイスがアナログ入力状態になっている

### EP1_AG08A_IOCTL_CONTROL_ANALOG_INPUT

各チャンネルに対してアナログ入力の開始もしくは停止を指示します。

#### Usage

```c
ep1_ag08a_ioctl_control_analog_input_t ctrl;
ioctl(fd, EP1_AG08A_IOCTL_CONTROL_ANALOG_INPUT, &ctrl);
```

#### Inputs

`ep1_ag08a_ioctl_control_analog_input_t` 型で入力します。

#### Outputs

none

### EP1_AG08A_IOCTL_SET_ANALOG_OUTPUT

アナログ出力の設定を行います。

#### Usage

```c
ep1_ag08a_ioctl_set_analog_output_t settings;
ioctl(fd, EP1_AG08A_IOCTL_SET_ANALOG_OUTPUT, &settings);
```

#### Inputs

`ep1_ag08a_ioctl_set_analog_output_t` 型で入力します。

#### Outputs

none

#### Errors

- EBUSY デバイスがアナログ出力状態になっている

### EP1_AG08A_IOCTL_CONTROL_ANALOG_OUTPUT

アナログ出力の開始もしくは停止を指示します。

#### Usage

```c
ep1_ag08a_ioctl_control_analog_output_t ctrl;
ioctl(fd, EP1_AG08A_IOCTL_CONTROL_ANALOG_OUTPUT, &ctrl);
```

#### Inputs

`ep1_ag08a_ioctl_control_analog_output_t` 型で入力します。

#### Outputs

none
