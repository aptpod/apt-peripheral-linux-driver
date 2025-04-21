# Device Driver Documentation for EDGEPLANT ANALOG-USB Interface

## About

EDGEPLANT ANALOG-USB Interface はアナログセンサーに接続し、制御信号を取得するためのインターフェイスデバイスです。
製品の詳細については[こちら](https://www.aptpod.co.jp/products/edgeplant/edgeplant-peripherals/#edgeplant-analog-interface)を参照してください。

EDGEPLANT USB Peripherals で共通している機能などについては、[こちら](./general.ja.md)を参照してください。

[システムコールによる操作](#how-to-use-system-calls)と、[IIOによる操作](#how-to-use-iio-interface)をサポートしています。

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
| Time USec   |  4byte | LE    | U    | 基準時刻からの経過時間（マイクロ秒）                   |
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

## How to use IIO Interface

EDGEPLANT ANALOG-USB Interface はIndustrial I/O(IIO)に対応しています。

以下のインターフェイスを利用した操作が可能です。

| interface | description |
| ----- | ------------- |
| /sys/bus/iio/iio:deviceX/ | 設定変更および確認のためのインターフェイス |
| /dev/iio:deviceX/ | アナログ入力データを読み込むためのインターフェイス（キャラクタデバイス） |

### アナログ入力操作

#### サンプリング周波数

サンプリング周波数を10000Hzに設定する例です。

```sh
$ echo 10000.0 > /sys/bus/iio/devices/iio\:deviceX/in_voltage_sampling_frequency
```

設定可能な値は以下のコマンドで確認できます。（単位:Hz）

```sh
$ cat /sys/bus/iio/devices/iio\:deviceX/in_voltage_sampling_frequency_available
10000.0 5000.0 2500.0 1250.0 625.0 312.5 156.25 10.0 1.0 0.1 0.01
```

#### 入力電圧範囲

CH0の入力電圧範囲を「max: 10V、min: -10V」（+-10V）に設定する例です。

```sh
$ echo pm10v > /sys/bus/iio/devices/iio\:deviceX/in_voltage0_range
```

設定可能な値は以下のコマンドで確認できます。

```sh
$ cat /sys/bus/iio/devices/iio\:deviceX/in_voltage_range_available 
pm10v pm5v
```

##### Notes

IIOでは、入力電圧範囲「max: 5V、min: 0V」（+5V）の設定をサポートしていません。+5Vの設定を利用する場合はシステムコールを利用してください。

#### 入力チャンネル設定

CH0、1、2のみを有効化する例です。

デバイス接続後の初期状態では全CH無効になっています。

```sh
$ echo 1 > /sys/bus/iio/devices/iio\:deviceX/scan_elements/in_voltage0_en
$ echo 1 > /sys/bus/iio/devices/iio\:deviceX/scan_elements/in_voltage1_en
$ echo 1 > /sys/bus/iio/devices/iio\:deviceX/scan_elements/in_voltage2_en
$ echo 0 > /sys/bus/iio/devices/iio\:deviceX/scan_elements/in_voltage3_en
$ echo 0 > /sys/bus/iio/devices/iio\:deviceX/scan_elements/in_voltage4_en
$ echo 0 > /sys/bus/iio/devices/iio\:deviceX/scan_elements/in_voltage5_en
$ echo 0 > /sys/bus/iio/devices/iio\:deviceX/scan_elements/in_voltage6_en
$ echo 0 > /sys/bus/iio/devices/iio\:deviceX/scan_elements/in_voltage7_en
```

#### タイムスタンプ出力設定

タイムスタンプ出力を有効化する例です。

```sh
$ echo 1 > /sys/bus/iio/devices/iio\:deviceX/scan_elements/in_timestamp_en
```

##### Notes

タイムスタンプ出力を有効化した場合、サンプル毎のアナログ入力データの末尾にタイムスタンプ（単位はナノ秒）がs64_t型で格納されます。

#### タイムスタンプ種類設定

ハードウェアタイムスタンプ（デバイス打刻）を利用する例です。

```sh
$ echo 1 > /sys/bus/iio/devices/iio\:deviceX/hardware_timestamp_en
```

ソフトウェアタイムスタンプ（OS打刻）を利用する例です。

```sh
$ echo 0 > /sys/bus/iio/devices/iio\:deviceX/hardware_timestamp_en
```

ソフトウェアタイムスタンプの種類をmonotonicに変更する例です。

```sh
$ echo monotonic > /sys/bus/iio/devices/iio\:deviceX/current_timestamp_clock
```

##### Notes

デフォルトではハードウェアタイムスタンプが有効になっています。ソフトウェアタイムスタンプは以下の設定が可能です。各設定の説明については[こちら](https://www.kernel.org/doc/html/latest/core-api/timekeeping.html)を参照してください。

- realtime
- monotonic
- monotonic_raw
- realtime_coarse
- monotonic_coarse
- boottime
- tai

#### アナログ入力開始

```sh
$ echo 1 > /sys/bus/iio/devices/iio\:deviceX/buffer/enable
```

##### Notes
enableに1を書き込むと `/dev/iio:deviceX` のバッファへのアナログデータの入力が開始されます。


#### アナログ入力データ取得

アナログ入力データ（CH0-7有効、タイムスタンプ無効）を取得する例です。1サンプルとして、16bitのデータx8個が取得されます。

```sh
$ sudo cat /dev/iio\:deviceX | xxd -
00000000: 1111 2222 3333 4444 5555 6666 7777 8888
00000010: 1111 2222 3333 4444 5555 6666 7777 8888
00000020: 1111 2222 3333 4444 5555 6666 7777 8888
...
```

##### Notes
アナログ入力データは、有効にしたチャンネルのデータのみが格納されます。タイムスタンプ出力が有効な場合、パディングが入ることがあります。

例えば、CH0-2有効、タイムスタンプ有効の場合、以下のようにパディングが入ります。

```sh
$ sudo cat /dev/iio\:deviceX | xxd -
00000000: 1111 2222 3333 xxxx TTTT TTTT TTTT TTTT
00000010: 1111 2222 3333 xxxx TTTT TTTT TTTT TTTT
00000020: 1111 2222 3333 xxxx TTTT TTTT TTTT TTTT
...

# xxxx: パディング TTTT: タイムスタンプ
```

1サンプルに含まれるデータは以下の条件で出力されます。

- タイムスタンプはアナログ入力データの末尾にs64_t型のナノ秒単位で格納されます
- アナログ入力データとタイムスタンプの間に、2~6byteのパディングが入る場合があります
- サンプルデータ構造の中の最大サイズである64bitをまたがないようにパディングされます
  - 有効チャネルが1つの場合は、6Byteのパディング
  - 有効チャネルが2つの場合は、4Byteのパディング
  - 有効チャネルが3つの場合は、2Byteのパディング
  - 有効チャネルが4つの場合は、パディングなし
  - 有効チャネルが5つの場合は、6Byteのパディング
  - …
- タイムスタンプ出力が無効の場合はパディングは入りません（全サンプル同サイズのため）

#### アナログ入力停止

```sh
$ echo 0 > /sys/bus/iio/devices/iio\:deviceX/buffer/enable
```

#### バッファ長設定

バッファに保存するサンプル数を100個に設定する例です。

```sh
$ echo 100 > /sys/bus/iio/devices/iio\:deviceX/buffer/length
```

##### Notes

バッファに保存するサンプル数です、バイト数ではありません。

バッファ内のサンプル数が設定値を超えた場合、バッファへの新しいデータの書き込みは行われず、バッファ内の古いデータが保持されます。取りこぼし無くデータを取得し続けたい場合は、余裕を持った値を設定し、バッファが一杯になる前にバッファからデータを読み出してください。


#### watermark設定
サンプルが100個貯まるまで読み込みをブロックする例です。

```sh
$ echo 100 > /sys/bus/iio/devices/iio\:deviceX/buffer/watermark
```

#### Notes
watermarkのデフォルトは1になっています。（ブロックしない）

### アナログ出力操作

#### 波形タイプ設定

アナログ出力波形をサイン波に設定する例です。

```sh
$ echo sine > /sys/bus/iio/devices/iio\:deviceX/out_voltage_waveform_type
```

設定可能な値は以下のコマンドで確認できます。

```sh
$ cat /sys/bus/iio/devices/iio\:deviceX/out_voltage_waveform_type_available 
random sine triangle square dc
```

#### 電圧設定

アナログ出力電圧を1.5Vに設定する例です。(単位:V、分解能:0.02V)

```sh
$ echo 1.5 > /sys/bus/iio/devices/iio\:deviceX/out_voltage_raw
```

#### 周波数設定

アナログ出力周波数を50Hzに設定する例です。(単位:Hz、分解能:1Hz)

```sh
$ echo 50 > /sys/bus/iio/devices/iio\:deviceX/out_voltage_frequency
```

#### アナログ出力開始

```sh
$ echo 1 >  /sys/bus/iio/devices/iio\:deviceX/out_voltage_en
```

#### アナログ出力停止

```sh
$ echo 0 > /sys/bus/iio/devices/iio\:deviceX/out_voltage_en
```
