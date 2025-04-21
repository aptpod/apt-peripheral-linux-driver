#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "apt_usbtrx_ioctl.h"

static volatile bool stop_requested = false;

static void
handle_signal(int signal)
{
    if (signal == SIGTERM || signal == SIGINT) {
        stop_requested = true;
    }
}

static int
help()
{
    printf("usage: write_can [options]");
    printf("options: \n");
    printf("  --device <path>, Set device path.\n");
    printf("  --bitrate <bps>, Set CAN bitrate.\n");
    printf("  --sample-point <sample-point>, Set CAN sample point.\n");
    printf("  --dbitrate <bps>, Set CAN data bitrate\n");
    printf("  --dsample-point <sample-point>, Set CAN data sample point.\n");
    printf("  --count <count>, Set CAN data count.\n");
    printf("  --interval <us>, Set interval time (us).\n");
    printf("  --help, Show this help text.\n");

    return EXIT_SUCCESS;
}

int
main(int argc, char* argv[])
{
    /*
     * Set device path.
     * You can check the serial number of the device path
     * and the channel number (interface number) with the following command.
     * e.g.
     *   udevadm info --query=property /dev/aptUSB1 | grep -e ID_SERIAL -e
     * ID_USB_INTERFACE_NUM
     */
    int fd;
    int result;

    const char* devpath = "/dev/aptUSB1";
    int bitrate = 500000;
    int sample_point = 0;
    int dbitrate = 2000000;
    int dsample_point = 0;
    unsigned int count = 1;
    int interval_us = 1000;

    static struct option long_options[] = {
        { "device", required_argument, NULL, 'd' },
        { "bitrate", required_argument, NULL, 'b' },
        { "sample-point", required_argument, NULL, 's' },
        { "dbitrate", required_argument, NULL, 'B' },
        { "dsample-point", required_argument, NULL, 'S' },
        { "count", required_argument, NULL, 'c' },
        { "interval", required_argument, NULL, 'i' },
        { "help", no_argument, NULL, 'h' },
        { 0, 0, 0, 0 }
    };

    int opt;
    while ((opt = getopt_long(
              argc, argv, "d:b:s:B:S:c:i:h", long_options, NULL)) != -1) {
        switch (opt) {
        case 'd':
            devpath = optarg;
            break;
        case 'b':
            bitrate = atoi(optarg);
            break;
        case 's':
            sample_point = atoi(optarg);
            break;
        case 'B':
            dbitrate = atoi(optarg);
            break;
        case 'S':
            dsample_point = atoi(optarg);
            break;
        case 'c':
            count = atoi(optarg);
            break;
        case 'i':
            interval_us = atoi(optarg);
            break;
        case 'h':
            return help();
        default:
            return help();
        }
    }

    struct sigaction sa;
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);

    printf("Device path: %s\n", devpath);
    printf("CAN bitrate: %d\n", bitrate);
    printf("CAN sample point: %d\n", sample_point);
    printf("CAN data bitrate: %d\n", dbitrate);
    printf("CAN data sample point: %d\n", dsample_point);
    printf("CAN data send count: %d\n", count);
    printf("Interval time: %d us\n", interval_us);

    /* Open device */
    fd = open(devpath, O_WRONLY);
    if (fd == -1) {
        printf("open().. Error, <errno:%d> devpath=%s\n", errno, devpath);
        return EXIT_FAILURE;
    }

    {
        /*
         * Setting up Silent mode
         */
        ep1_cf02a_ioctl_set_silent_mode_t silent_mode;
        silent_mode.silent = false;
        result = ioctl(fd, EP1_CF02A_IOCTL_SET_SILENT_MODE, &silent_mode);
        if (result == -1) {
            printf("ioctl().. Error, <errno:%d> cmd=%s\n",
                   errno,
                   "EP1_CF02A_IOCTL_SET_SILENT_MODE");
            close(fd);
            return EXIT_FAILURE;
        }
    }

#if 0 // TODO: FD mode is not supported by firmware yet.
    {
        /*
         * Setting up FD mode
         */
        ep1_cf02a_ioctl_set_fd_mode_t fd_mode;
        fd_mode.fd = true;
        result = ioctl(fd, EP1_CF02A_IOCTL_SET_FD_MODE, &fd_mode);
        if (result == -1) {
            printf("ioctl().. Error, <errno:%d> cmd=%s\n", errno, "EP1_CF02A_IOCTL_SET_FD_MODE");
            close(fd);
            return EXIT_FAILURE;
        }
    }
#endif

    {
        /*
         * Setting up ISO mode
         */
        ep1_cf02a_ioctl_set_iso_mode_t iso_mode;
        iso_mode.non_iso_mode = false;
        result = ioctl(fd, EP1_CF02A_IOCTL_SET_ISO_MODE, &iso_mode);
        if (result == -1) {
            printf("ioctl().. Error, <errno:%d> cmd=%s\n",
                   errno,
                   "EP1_CF02A_IOCTL_SET_ISO_MODE");
            close(fd);
            return EXIT_FAILURE;
        }
    }

    {
        /*
         * Setting up CAN bitrate
         */
        ep1_cf02a_ioctl_set_bitrate_t set_bitrate;
        set_bitrate.bitrate = bitrate;
        set_bitrate.sample_point = sample_point;
        result = ioctl(fd, EP1_CF02A_IOCTL_SET_BITRATE, &set_bitrate);
        if (result == -1) {
            printf("ioctl().. Error, <errno:%d> cmd=%s\n",
                   errno,
                   "EP1_CF02A_IOCTL_SET_BITRATE");
            close(fd);
            return EXIT_FAILURE;
        }
    }

    {
        /*
         * Setting up CAN data bitrate
         */
        ep1_cf02a_ioctl_set_data_bitrate_t set_dbitrate;
        set_dbitrate.bitrate = dbitrate;
        set_dbitrate.sample_point = dsample_point;
        result = ioctl(fd, EP1_CF02A_IOCTL_SET_DATA_BITRATE, &set_dbitrate);
        if (result == -1) {
            printf("ioctl().. Error, <errno:%d> cmd=%s\n",
                   errno,
                   "EP1_CF02A_IOCTL_SET_DATA_BITRATE");
            close(fd);
            return EXIT_FAILURE;
        }
    }

    {
        /*
         * Start CAN sending.
         * After this, the device can send CAN data to CAN bus.
         */
        ep1_cf02a_ioctl_set_tx_rx_control_t tx_rx_control;
        tx_rx_control.start = true;
        result = ioctl(fd, EP1_CF02A_IOCTL_SET_TX_RX_CONTROL, &tx_rx_control);
        if (result == -1) {
            printf("ioctl().. Error, <errno:%d> cmd=%s\n",
                   errno,
                   "EP1_CF02A_IOCTL_SET_TX_RX_CONTROL");
            close(fd);
            return EXIT_FAILURE;
        }
    }

    for (unsigned int i = 0; i < count; i++) {
        if (stop_requested) {
            break;
        }

        /*
         * CAN Frame buffer
         * The data format of the CAN frame is as follows.
         * https://github.com/aptpod/apt-peripheral-linux-driver/blob/main/docs/EP1-CF02A.ja.md#can-frame
         */
        unsigned char buf[4 + 1 + 1 + 64] = { 0 };

        /* id and frame type */
        buf[0] = 0x01;
        buf[1] = 0x00;
        buf[2] = 0x00;
        buf[3] = 0x00;

        /* dlc */
#if 1 /* CAN FD Frame*/
        buf[4] = 0x0F;
#else /* CAN Frame */
        buf[4] = 0x08;
#endif

        /* flags */
#if 1                  /* CAN FD Frame*/
        buf[5] = 0x05; /* BRS:1, ESI:0, FDF:1 */
#else                  /* CAN Frame */
        buf[5] = 0x00; /* BRS:0, ESI:0, FDF:0 */
#endif

        /* Data */
        memcpy(&buf[6], &i, sizeof(i));

        /* Write CAN data */
        ssize_t wsize = write(fd, buf, sizeof(buf));
        if (wsize == -1) {
            printf("write().. Error, <errno:%d>\n", errno);
            break;
        }

        usleep(interval_us);
    }

    {
        /*
         * Stop CAN receiving.
         */
        ep1_cf02a_ioctl_set_tx_rx_control_t tx_rx_control;
        tx_rx_control.start = false;
        result = ioctl(fd, EP1_CF02A_IOCTL_SET_TX_RX_CONTROL, &tx_rx_control);
        if (result == -1) {
            printf("ioctl().. Error, <errno:%d> cmd=%s\n",
                   errno,
                   "EP1_CF02A_IOCTL_SET_TX_RX_CONTROL");
            close(fd);
            return EXIT_FAILURE;
        }
    }

    close(fd);

    return EXIT_SUCCESS;
}
