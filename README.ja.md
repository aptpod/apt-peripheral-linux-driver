# Device driver for EDGEPLANT USB Peripherals

## About

EDGEPLANT USB ペリフェラルを Linux 上で利用するためのデバイスドライバです。
本デバイスドライバをインストールすることで、以下の製品でデータの受信や送信を行えるようになります。

- EDGEPLANT CAN-USB Interface
- EDGEPLANT CAN FD USB Interface
- EDGEPLANT ANALOG-USB Interface

EDGEPLANT Peripherals の詳細については[製品のホームページ](https://www.aptpod.co.jp/products/edgeplant/edgeplant-peripherals)を参照してください。

## Getting Started

### Prerequisites

このデバイスドライバをビルドするためには、以下のパッケージが必要となります。

- GNU C
- GNU make
- Linux headers (kernel v4.4 以上)

このデバイスドライバを使用するためには、以下のカーネルコンフィグが必要となります。

- SocketCAN
  - CONFIG_NET
  - CONFIG_CAN
  - CONFIG_CAN_DEV
- IIO
  - CONFIG_IIO
  - CONFIG_IIO_BUFFER
  - CONFIG_IIO_KFIFO_BUF

### Installing

#### deb パッケージからのインストール

1. アプトポッドの公開リポジトリを設定します。  
   コマンド内の `${DISTRIBUTION}` には、ご使用の環境に応じて `ubuntu` または `debian` を指定してください。

   ```sh
   sudo apt-get update
   sudo apt-get install -y \
      apt-transport-https \
      ca-certificates \
      curl \
      gnupg-agent \
      lsb-release
   sudo mkdir -p /etc/apt/keyrings
   curl -fsSL https://repository.aptpod.jp/intdash-edge/linux/${DISTRIBUTION}/gpg | \
      sudo gpg --dearmor -o /etc/apt/keyrings/intdash-edge.gpg
   echo "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/intdash-edge.gpg] \
      https://repository.aptpod.jp/intdash-edge/linux/${DISTRIBUTION} $(lsb_release -cs) \
      stable" \
      | sudo tee /etc/apt/sources.list.d/intdash-edge.list
   sudo apt-get update
   ```

2. apt-usbtrx-dkms パッケージをインストールします。

   ```sh
   sudo apt-get install apt-usbtrx-dkms
   ```

3. モジュールを読み込みます。

   ```sh
   sudo modprobe apt_usbtrx
   ```

DKMS を使用することで、カーネルが更新されるたびに自動的にドライバが再ビルドされます。

#### ソースからビルドしてインストール

1. リポジトリをクローンします。

   ```sh
   git clone https://github.com/aptpod/apt-peripheral-linux-driver
   ```

1. デバイスドライバとツールをビルドします。

   ```sh
   make all
   ```

1. デバイスドライバとツールをインストールします。

   ```sh
   sudo make install
   sudo modprobe apt_usbtrx
   ```

   デフォルトではツールは `/usr/local/bin` にインストールされます。任意のディレクトリにインストールする場合は、`DESTDIR` を指定します。

   ```sh
   mkdir -p /path/to/dir
   make DESTDIR=/path/to/dir install
   ```

### Running the unit test

本プロジェクトでは [KUnit](https://www.kernel.org/doc/html/latest/dev-tools/kunit/index.html) を利用して単体テストを行なっています。自身の環境で単体テストを実施する場合、以下のスクリプトを実行してください。

```sh
cd kunit && ./run_test.sh
```

## Usage

ペリフェラルごとの利用方法については、[docs](./docs/README.md)を参照してください。

## License

Distributed under the GPL v2.0 License. See LICENSE for more information.

## Contact

https://www.aptpod.co.jp/contact/
