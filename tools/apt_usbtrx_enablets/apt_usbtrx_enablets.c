/*
 * apt_usbtrx_enablets.c - Enable timestamp reset.
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

#define PRGNAME "apt_usbtrx_enablets"
#define PRGVERSION "1.0.0"

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
    printf("usage: %s [-f <dev name>] [-h] [-v]\n", PRGNAME);
    printf("options: \n");
    printf("  -f DEV_NAME,      Set device name\n");
    printf("  -h,               This help text\n");
    printf("  -v,               Show version number\n");

    return RESULT_Success;
}

int
main(int argc, char* argv[])
{
    int fd;
    char* devname = NULL;
    int result;
    int n = 0;

    /*** Handle options ***/
    while (n >= 0) {
        n = getopt(argc, argv, "f:hv");
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

    result = ioctl(fd, APT_USBTRX_IOCTL_ENABLE_RESET_TS);
    if (result == -1) {
        printf(
          "ioctl().. Error, <errno:%d> cmd=%s\n", errno, "ENABLE_RESET_TS");
        return EXIT_FAILURE;
    }

    close(fd);

    return EXIT_SUCCESS;
}
