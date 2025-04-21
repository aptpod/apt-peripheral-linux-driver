#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <linux/can.h>
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
can_dlc2len(int dlc)
{
    static const int dlc_to_len[] = { 0, 1,  2,  3,  4,  5,  6,  7,
                                      8, 12, 16, 20, 24, 32, 48, 64 };
    if (dlc >= 0 && dlc <= 15) {
        return dlc_to_len[dlc];
    } else {
        return -1;
    }
}

static unsigned int
array_to_uint(unsigned char* array)
{
    return array[0] | (array[1] << 8) | (array[2] << 16) | (array[3] << 24);
}

static void
timespec_add(struct timespec* ts, unsigned int sec, unsigned int nsec)
{
    ts->tv_sec += sec;
    ts->tv_nsec += nsec;
    if (ts->tv_nsec >= 1000000000) {
        ts->tv_sec++;
        ts->tv_nsec -= 1000000000;
    }
}

static void
print_canfd_frame(struct timespec* timestamp, struct canfd_frame* frame)
{
    printf("[%ld.%09ld] id: %x, len: %d, flags: %x, data: ",
           timestamp->tv_sec,
           timestamp->tv_nsec,
           frame->can_id,
           frame->len,
           frame->flags);
    for (int i = 0; i < frame->len; ++i) {
        printf("%02X", frame->data[i]);
    }
    printf("\n");
}

static int
help()
{
    printf("usage: read_can [options]");
    printf("options: \n");
    printf("  --device <path>, Set device path.\n");
    printf("  --bitrate <bps>, Set CAN bitrate.\n");
    printf("  --sample-point <sample-point>, Set CAN sample point.\n");
    printf("  --dbitrate <bps>, Set CAN data bitrate\n");
    printf("  --dsample-point <sample-point>, Set CAN data sample point.\n");
    printf("  --skip-ioctl,  Skip ioctl for simultaneous read/write on one "
           "device.\n");
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
     *   udevadm info --query=property /dev/aptUSB0 | grep -e ID_SERIAL -e
     * ID_USB_INTERFACE_NUM
     */
    int fd;
    int result;
    struct timespec basetime;

    const char* devpath = "/dev/aptUSB0";
    int bitrate = 500000;
    int sample_point = 0;
    int dbitrate = 2000000;
    int dsample_point = 0;
    bool skip_ioctl = false;

    static struct option long_options[] = {
        { "device", required_argument, NULL, 'd' },
        { "bitrate", required_argument, NULL, 'b' },
        { "sample-point", required_argument, NULL, 's' },
        { "dbitrate", required_argument, NULL, 'B' },
        { "dsample-point", required_argument, NULL, 'S' },
        { "skip-ioctl", no_argument, NULL, 'i' },
        { "help", no_argument, NULL, 'h' },
        { 0, 0, 0, 0 }
    };

    int opt;
    while ((opt = getopt_long(
              argc, argv, "d:b:s:B:S:ih", long_options, NULL)) != -1) {
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
        case 'i':
            skip_ioctl = true;
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
    printf("Skip ioctl: %s\n", skip_ioctl ? "true" : "false");

    /* Open device */
    fd = open(devpath, O_RDONLY);
    if (fd == -1) {
        printf("open().. Error, <errno:%d> devpath=%s\n", errno, devpath);
        return EXIT_FAILURE;
    }

    if (!skip_ioctl) {
        {
            /*
             * Setting up Silent mode
             */
            ep1_cf02a_ioctl_set_silent_mode_t silent_mode;
            silent_mode.silent = true;
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
             * Start CAN receiving.
             * After this, the device will send CAN data to the host.
             */
            ep1_cf02a_ioctl_set_tx_rx_control_t tx_rx_control;
            tx_rx_control.start = true;
            result =
              ioctl(fd, EP1_CF02A_IOCTL_SET_TX_RX_CONTROL, &tx_rx_control);
            if (result == -1) {
                printf("ioctl().. Error, <errno:%d> cmd=%s\n",
                       errno,
                       "EP1_CF02A_IOCTL_SET_TX_RX_CONTROL");
                close(fd);
                return EXIT_FAILURE;
            }
        }
    }

    while (!stop_requested) {
        /* Read CAN data */
        unsigned int sec, usec;
        struct canfd_frame frame = { 0 };

        unsigned char buf[1024];
        ssize_t rsize = read(fd, buf, sizeof(buf));
        if (rsize == -1) {
            if (errno != EINTR) {
                printf("read().. Error, <errno:%d>\n", errno);
            }
            break;
        }

        /*
         * Multiple CAN data will be stored in the receive buffer.
         * The data format of the CAN frame is as follows.
         * https://github.com/aptpod/apt-peripheral-linux-driver/blob/main/docs/EP1-CF02A.ja.md#can-frame
         */
        const int packet_num = rsize / EP1_CF02A_CAN_PACKET_SIZE;
        for (int num = 0, pos = 0; num < packet_num;
             ++num, pos += EP1_CF02A_CAN_PACKET_SIZE) {
            struct timespec timestamp = basetime;

            /* Getting timestamp from receiving data */
            sec = array_to_uint(&buf[pos]);
            usec = array_to_uint(&buf[pos + 4]);
            timespec_add(&timestamp, sec, usec * 1000);

            /* Getting CAN frame from receiving data */
            frame.can_id = array_to_uint(&buf[pos + 8]) & 0x1FFFFFFF;
            frame.len = can_dlc2len(buf[pos + 12] & 0x0F);
            frame.flags = buf[pos + 13];
            memcpy(frame.data, &buf[pos + 14], CANFD_MAX_DLEN);

            print_canfd_frame(&timestamp, &frame);
        }
    }

    if (!skip_ioctl) {
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
