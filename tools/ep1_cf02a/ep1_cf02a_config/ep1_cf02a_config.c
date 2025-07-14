/*
 * ep1_cf02a_config.c - Configure EP1-CF02A device.
 *
 * Copyright (C) 2024 aptpod Inc.
 */

#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "apt_usbtrx_ioctl.h"

/*!
 * @brief
 */
static void
help()
{
    printf("usage: ep1_cf02a_config [options]\n");
    printf("options: \n");
    printf("  --device <path>,                      Set device path.\n");
    printf(
      "  --get-config,                         Get current configuration.\n");
    printf("  --silent <on|off>,                      Set silent mode.\n");
    printf("  --bitrate <bps>,                      Set CAN bitrate.\n");
    printf("  --sample-point <sample-point>,        Set CAN sample point.\n");
    printf("  --fd <on|off>,                        Set CAN FD mode.\n");
    printf("  --dbitrate <bps>,                     Set CAN data bitrate\n");
    printf(
      "  --dsample-point <sample-point>,       Set CAN data sample point.\n");
    printf(
      "  --store <on|off>,                     Enable/disable store data.\n");
    printf("  --help,                               Show this help text.\n");
}

int
main(int argc, char* argv[])
{
    int fd;
    int result;

    const char* devpath = "";
    bool get_config = false;
    bool silent_mode = false;
    int bitrate = 500000;
    int sample_point = 0;
    bool fd_mode = true;
    int dbitrate = 2000000;
    int dsample_point = 0;
    bool store_enable = false;

    static struct option long_options[] = {
        { "device", required_argument, NULL, 'd' },
        { "get-config", no_argument, NULL, 'g' },
        { "silent", required_argument, NULL, 'i' },
        { "bitrate", required_argument, NULL, 'b' },
        { "sample-point", required_argument, NULL, 's' },
        { "fd", required_argument, NULL, 'f' },
        { "dbitrate", required_argument, NULL, 'B' },
        { "dsample-point", required_argument, NULL, 'S' },
        { "store", required_argument, NULL, 'o' },
        { "help", no_argument, NULL, 'h' },
        { 0, 0, 0, 0 }
    };

    int opt;
    while ((opt = getopt_long(
              argc, argv, "d:gi:b:s:f:B:S:o:h", long_options, NULL)) != -1) {
        switch (opt) {
        case 'd':
            devpath = optarg;
            break;
        case 'g':
            get_config = true;
            break;
        case 'i':
            silent_mode = (strcmp(optarg, "on") == 0) ? true : false;
            break;
        case 'b':
            bitrate = atoi(optarg);
            break;
        case 's':
            sample_point = atoi(optarg);
            break;
        case 'f':
            fd_mode = (strcmp(optarg, "on") == 0) ? true : false;
            break;
        case 'B':
            dbitrate = atoi(optarg);
            break;
        case 'S':
            dsample_point = atoi(optarg);
            break;
        case 'o':
            store_enable = (strcmp(optarg, "on") == 0) ? true : false;
            break;
        case 'h':
            help();
            return EXIT_SUCCESS;
        default:
            help();
            return EXIT_FAILURE;
        }
    }

    if (devpath[0] == '\0') {
        printf("ERROR: Device path is required.\n");
        help();
        return EXIT_FAILURE;
    }

    /* Open device */
    fd = open(devpath, O_RDONLY);
    if (fd == -1) {
        printf("open().. Error, <errno:%d> devpath=%s\n", errno, devpath);
        return EXIT_FAILURE;
    }

    if (get_config) {
        /*
         * Get current configuration
         */

        ep1_cf02a_ioctl_get_silent_mode_t get_silent_mode;
        result = ioctl(fd, EP1_CF02A_IOCTL_GET_SILENT_MODE, &get_silent_mode);
        if (result != 0) {
            printf("ioctl().. Error, <errno:%d> cmd=%s\n",
                   errno,
                   "EP1_CF02A_IOCTL_GET_SILENT_MODE");
            close(fd);
            return EXIT_FAILURE;
        }
        silent_mode = get_silent_mode.silent;

        ep1_cf02a_ioctl_get_bitrate_t get_bitrate;
        result = ioctl(fd, EP1_CF02A_IOCTL_GET_BITRATE, &get_bitrate);
        if (result != 0) {
            printf("ioctl().. Error, <errno:%d> cmd=%s\n",
                   errno,
                   "EP1_CF02A_IOCTL_GET_BITRATE");
            close(fd);
            return EXIT_FAILURE;
        }
        bitrate = get_bitrate.bitrate;
        sample_point = get_bitrate.sample_point;

        ep1_cf02a_ioctl_get_fd_mode_t get_fd_mode;
        result = ioctl(fd, EP1_CF02A_IOCTL_GET_FD_MODE, &get_fd_mode);
        if (result != 0) {
            printf("ioctl().. Error, <errno:%d> cmd=%s\n",
                   errno,
                   "EP1_CF02A_IOCTL_GET_FD_MODE");
            close(fd);
            return EXIT_FAILURE;
        }
        fd_mode = get_fd_mode.fd;

        if (fd_mode) {
            ep1_cf02a_ioctl_get_data_bitrate_t get_dbitrate;
            result = ioctl(fd, EP1_CF02A_IOCTL_GET_DATA_BITRATE, &get_dbitrate);
            if (result != 0) {
                printf("ioctl().. Error, <errno:%d> cmd=%s\n",
                       errno,
                       "EP1_CF02A_IOCTL_GET_DATA_BITRATE");
                close(fd);
                return EXIT_FAILURE;
            }
            dbitrate = get_dbitrate.bitrate;
            dsample_point = get_dbitrate.sample_point;
        }
        ep1_cf02a_ioctl_get_store_enable_t get_store_enable;
        result = ioctl(fd, EP1_CF02A_IOCTL_GET_STORE_ENABLE, &get_store_enable);
        if (result != 0) {
            printf("ioctl().. Error, <errno:%d> cmd=%s\n",
                   errno,
                   "EP1_CF02A_IOCTL_GET_STORE_ENABLE");
            close(fd);
            return EXIT_FAILURE;
        }
        store_enable = get_store_enable.enable;

        printf("Current configuration:\n");
        printf("Device path: %s\n", devpath);
        printf("Silent mode: %s\n", silent_mode ? "on" : "off");
        printf("CAN bitrate: %d\n", bitrate);
        printf("CAN sample point: %d\n", sample_point);
        printf("CAN FD mode: %s\n", fd_mode ? "on" : "off");
        if (fd_mode) {
            printf("CAN data bitrate: %d\n", dbitrate);
            printf("CAN data sample point: %d\n", dsample_point);
        }
        printf("Store data: %s\n", store_enable ? "on" : "off");
    } else {
        /*
         * Set configuration
         */
        printf("Setting configuration:\n");
        printf("Device path: %s\n", devpath);
        printf("Silent mode: %s\n", silent_mode ? "on" : "off");
        printf("CAN bitrate: %d\n", bitrate);
        printf("CAN sample point: %d\n", sample_point);
        printf("CAN FD mode: %s\n", fd_mode ? "on" : "off");
        if (fd_mode) {
            printf("CAN data bitrate: %d\n", dbitrate);
            printf("CAN data sample point: %d\n", dsample_point);
        }
        printf("Store data: %s\n", store_enable ? "on" : "off");

        {
            /*
             * Setting up Silent mode
             */
            ep1_cf02a_ioctl_set_silent_mode_t set_silent_mode;
            set_silent_mode.silent = silent_mode;
            result =
              ioctl(fd, EP1_CF02A_IOCTL_SET_SILENT_MODE, &set_silent_mode);
            if (result != 0) {
                printf("ioctl().. Error, <errno:%d> cmd=%s\n",
                       errno,
                       "EP1_CF02A_IOCTL_SET_SILENT_MODE");
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
            if (result != 0) {
                printf("ioctl().. Error, <errno:%d> cmd=%s\n",
                       errno,
                       "EP1_CF02A_IOCTL_SET_BITRATE");
                close(fd);
                return EXIT_FAILURE;
            }
        }

        {
            /*
             * Setting up FD mode
             */
            ep1_cf02a_ioctl_set_fd_mode_t set_fd_mode;
            set_fd_mode.fd = fd_mode;
            result = ioctl(fd, EP1_CF02A_IOCTL_SET_FD_MODE, &set_fd_mode);
            if (result != 0) {
                printf("ioctl().. Error, <errno:%d> cmd=%s\n",
                       errno,
                       "EP1_CF02A_IOCTL_SET_FD_MODE");
                close(fd);
                return EXIT_FAILURE;
            }
        }

        if (fd_mode) {
            {
                /*
                 * Setting up ISO mode
                 */
                ep1_cf02a_ioctl_set_iso_mode_t iso_mode;
                iso_mode.non_iso_mode = false;
                result = ioctl(fd, EP1_CF02A_IOCTL_SET_ISO_MODE, &iso_mode);
                if (result != 0) {
                    printf("ioctl().. Error, <errno:%d> cmd=%s\n",
                           errno,
                           "EP1_CF02A_IOCTL_SET_ISO_MODE");
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
                result =
                  ioctl(fd, EP1_CF02A_IOCTL_SET_DATA_BITRATE, &set_dbitrate);
                if (result != 0) {
                    printf("ioctl().. Error, <errno:%d> cmd=%s\n",
                           errno,
                           "EP1_CF02A_IOCTL_SET_DATA_BITRATE");
                    close(fd);
                    return EXIT_FAILURE;
                }
            }
        }

        {
            /*
             * Setting up Store data enable
             */
            ep1_cf02a_ioctl_set_store_enable_t set_store_enable;
            set_store_enable.enable = store_enable;
            result =
              ioctl(fd, EP1_CF02A_IOCTL_SET_STORE_ENABLE, &set_store_enable);
            if (result != 0) {
                printf("ioctl().. Error, <errno:%d> cmd=%s\n",
                       errno,
                       "EP1_CF02A_IOCTL_SET_STORE_ENABLE");
                close(fd);
                return EXIT_FAILURE;
            }
        }
    }

    close(fd);

    return EXIT_SUCCESS;
}
