/*
 * apt_usbtrx_serial_no.c - Getting serial number.
 *
 * Copyright (C) 2018 aptpod Inc.
 */

#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "apt_usbtrx_ioctl.h"

#define PRGNAME "apt_usbtrx_serial_no"
#define PRGVERSION "1.0.0"
#define SHOWFLG_CHANNEL 0x01
#define SHOWFLG_SYNCPULSE 0x02
#define SHOWFLG_SERIALNO 0x04

enum
{
    RESULT_Unknown = -1,
    RESULT_Success = 0,
    RESULT_Failure
};

/*!
 * @brief
 */
static int
version()
{
    printf("%s\n", PRGNAME " ver " PRGVERSION);

    return RESULT_Success;
}

/*!
 * @brief
 */
static int
help()
{
    printf("usage: %s [-f <dev name>] [-h] [-v] [-s]\n", PRGNAME);
    printf("options: \n");
    printf("  -f DEV_NAME,      Set device name\n");
    printf("  -h,               This help text\n");
    printf("  -v,               Show version number\n");
    printf("  -c,               Show channel\n");
    printf("  -s,               Show serial no\n");
    printf("  -p,               Show synchronization pulse\n");

    return RESULT_Success;
}

int
main(int argc, char* argv[])
{
    int fd;
    char* devname = NULL;
    int result;
    int n = 0;
    int i = 0;
    apt_usbtrx_ioctl_get_serial_no_t serial_no;
    int showflg = 0;

    /*** Handle options ***/
    while (n >= 0) {
        n = getopt(argc, argv, "f:hvcsp");
        switch (n) {
        case 'f':
            devname = optarg;
            break;
        case 'h':
            result = help();
            if (result != RESULT_Success) {
                return EXIT_FAILURE;
            } else {
                return EXIT_SUCCESS;
            }
        case 'v':
            result = version();
            if (result != RESULT_Success) {
                return EXIT_FAILURE;
            } else {
                return EXIT_SUCCESS;
            }
        case 'c':
            showflg |= SHOWFLG_CHANNEL;
            break;
        case 'p':
            showflg |= SHOWFLG_SYNCPULSE;
            break;
        case 's':
            showflg |= SHOWFLG_SERIALNO;
            break;
        default:
            break;
        }
    }
    if (devname == NULL) {
        result = help();
        if (result != RESULT_Success) {
            return EXIT_FAILURE;
        } else {
            return EXIT_SUCCESS;
        }
    }

    fd = open(devname, O_RDONLY);
    if (fd == -1) {
        printf("open().. Error, <errno:%d> devname=%s\n", errno, devname);
        return EXIT_FAILURE;
    }

    result = ioctl(fd, APT_USBTRX_IOCTL_GET_SERIAL_NO, &serial_no);
    if (result == -1) {
        printf("ioctl().. Error, <errno:%d> cmd=%s\n", errno, "GET_SERIAL_NO");
        return EXIT_FAILURE;
    }

    if (showflg & SHOWFLG_CHANNEL) {
        printf("%d\n", serial_no.channel);
    }
    if (showflg & SHOWFLG_SYNCPULSE) {
        printf("%s\n", (serial_no.sync_pulse == 0) ? "source" : "external");
    }
    if (showflg & SHOWFLG_SERIALNO) {
        for (i = 0; i < APT_USBTRX_SERIAL_NO_LENGTH && serial_no.sn[i] != 0;
             i++) {
            printf("%c", serial_no.sn[i]);
        }
        printf("\n");
    }

    close(fd);

    return EXIT_SUCCESS;
}
