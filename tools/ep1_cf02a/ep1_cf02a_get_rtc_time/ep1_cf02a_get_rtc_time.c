/*
 * ep1_cf02a_get_rtc_time.c - Get RTC time.
 *
 * Copyright (C) 2024 aptpod Inc.
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

#define PRGNAME "ep1_cf02a_get_rtc_time"
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

/*!
 * @brief
 */
static const char*
weekday_to_string(int weekday)
{
    switch (weekday) {
    case 1:
        return "Mon";
    case 2:
        return "Tue";
    case 3:
        return "Wed";
    case 4:
        return "Thu";
    case 5:
        return "Fri";
    case 6:
        return "Sat";
    case 7:
        return "Sun";
    default:
        return "XXX";
    }
}

int
main(int argc, char* argv[])
{
    int fd;
    char* devname = NULL;
    int result;
    int n = 0;
    ep1_cf02a_ioctl_get_rtc_time_t rtc_time;

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

    result = ioctl(fd, EP1_CF02A_IOCTL_GET_RTC_TIME, &rtc_time);
    if (result == -1) {
        printf("ioctl().. Error, <errno:%d> cmd=%s\n", errno, "EP1_CF02A_IOCTL_GET_RTC_TIME");
        return EXIT_FAILURE;
    }

    printf("Get RTC Time(UTC): %04d-%02d-%02d %02d:%02d %02d.%06d %s\n",
           2000 + rtc_time.year, rtc_time.month, rtc_time.date,
           rtc_time.hour, rtc_time.minute, rtc_time.second, rtc_time.microsecond,
           weekday_to_string(rtc_time.weekday));

    close(fd);

    return EXIT_SUCCESS;
}
