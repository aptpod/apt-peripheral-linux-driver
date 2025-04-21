/*
 * ep1_cf02a_get_store_data_list.c - Get store data list.
 *
 * Copyright (C) 2024 aptpod Inc.
 */

#include <errno.h>
#include <fcntl.h>
#include <linux/can.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "apt_usbtrx_ioctl.h"

#define PRGNAME "ep1_cf02a_get_store_data_list"
#define PRGVERSION "1.0.0"

enum
{
    RESULT_Unknown = -1,
    RESULT_Success = 0,
    RESULT_Failure
};

static int store_data_read_interval = 1000;

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
    for (int i = 0; i < sizeof(frame->data); ++i) {
        printf("%02X", frame->data[i]);
    }
    printf("\n");
}

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
    printf("usage: %s [-f <dev name>] [-r] [-h] [-v]\n", PRGNAME);
    printf("options: \n");
    printf("  -f DEV_NAME,      Set device name\n");
    printf("  -r                Read store data\n");
    printf("  -h,               This help text\n");
    printf("  -v,               Show version number\n");

    return RESULT_Success;
}

void
alloc_store_data_id_list(
  ep1_cf02a_ioctl_get_store_data_id_list_t* store_data_id_list)
{
    store_data_id_list->id_list =
      (char(*)[EP1_CF02A_STORE_DATA_ID_MAX_LENGTH])malloc(
        store_data_id_list->count * EP1_CF02A_STORE_DATA_ID_MAX_LENGTH);
    if (store_data_id_list->id_list == NULL) {
        store_data_id_list->count = 0;
    }
}

void
free_store_data_id_list(
  ep1_cf02a_ioctl_get_store_data_id_list_t* store_data_id_list)
{
    if (store_data_id_list->id_list != NULL) {
        free(store_data_id_list->id_list);
        store_data_id_list->id_list = NULL;
    }
}

int
get_store_data_id_list(
  int fd,
  ep1_cf02a_ioctl_get_store_data_id_list_t* store_data_id_list)
{
    ep1_cf02a_ioctl_get_store_data_id_list_count_t store_data_id_list_count;
    int result;

    result = ioctl(fd,
                   EP1_CF02A_IOCTL_GET_STORE_DATA_ID_LIST_COUNT,
                   &store_data_id_list_count);
    if (result != 0) {
        return RESULT_Failure;
    }

    if (store_data_id_list_count.count == 0) {
        store_data_id_list->count = 0;
        store_data_id_list->id_list = NULL;
        return RESULT_Success;
    }

    store_data_id_list->count = store_data_id_list_count.count;
    alloc_store_data_id_list(store_data_id_list);

    result =
      ioctl(fd, EP1_CF02A_IOCTL_GET_STORE_DATA_ID_LIST, store_data_id_list);
    if (result != 0) {
        free_store_data_id_list(store_data_id_list);
        return RESULT_Failure;
    }

    return RESULT_Success;
}

int
get_store_data_meta(int fd,
                    const char* id,
                    ep1_cf02a_ioctl_get_store_data_meta_t* store_data_meta)
{
    memcpy(store_data_meta->id, id, EP1_CF02A_STORE_DATA_ID_MAX_LENGTH);

    int result =
      ioctl(fd, EP1_CF02A_IOCTL_GET_STORE_DATA_META, store_data_meta);
    if (result != 0) {
        return RESULT_Failure;
    }

    return RESULT_Success;
}

int
get_store_data_rx_control(
  int fd,
  ep1_cf02a_ioctl_get_store_data_rx_control_t* store_data_rx_control)
{
    int result;

    result = ioctl(
      fd, EP1_CF02A_IOCTL_GET_STORE_DATA_RX_CONTROL, store_data_rx_control);
    if (result != 0) {
        printf("ioctl().. Error, <errno:%d> cmd=%s\n",
               errno,
               "EP1_CF02A_IOCTL_GET_STORE_DATA_RX_CONTROL");
        return RESULT_Failure;
    }

    return RESULT_Success;
}

int
set_store_data_rx_control(int fd, const char* id, bool start)
{
    ep1_cf02a_ioctl_set_store_data_rx_control_t store_data_rx_control;
    int result;

    memcpy(store_data_rx_control.id, id, EP1_CF02A_STORE_DATA_ID_MAX_LENGTH);
    store_data_rx_control.start = start;
    store_data_rx_control.interval = store_data_read_interval;

    result = ioctl(
      fd, EP1_CF02A_IOCTL_SET_STORE_DATA_RX_CONTROL, &store_data_rx_control);
    if (result != 0) {
        printf("ioctl().. Error, <errno:%d> cmd=%s start=%s\n",
               errno,
               "EP1_CF02A_IOCTL_SET_STORE_DATA_RX_CONTROL",
               start ? "true" : "false");
        return RESULT_Failure;
    }

    return RESULT_Success;
}

int
read_store_data(int fd, const char* id, unsigned long long can_frame_count)
{
    int result;
    bool start;
    long rsize = 0;
    ssize_t total_rsize = 0;
    ssize_t total_can_frame_size = can_frame_count * EP1_CF02A_CAN_PACKET_SIZE;
    size_t can_count = 1;

    /* Start */
    start = true;
    result = set_store_data_rx_control(fd, id, start);
    if (result != RESULT_Success) {
        return RESULT_Failure;
    }

    /* Start check */
    {
        ep1_cf02a_ioctl_get_store_data_rx_control_t store_data_rx_control = {
            0
        };

        memcpy(
          store_data_rx_control.id, id, EP1_CF02A_STORE_DATA_ID_MAX_LENGTH);
        result = get_store_data_rx_control(fd, &store_data_rx_control);
        if (result != RESULT_Success) {
            return RESULT_Failure;
        }
        if (start != store_data_rx_control.start) {
            printf("Start check failed. start=%d\n",
                   store_data_rx_control.start);
            return RESULT_Failure;
        }
    }

    /* Read store data */
    ep1_cf02a_ioctl_read_store_data_t read_store_data;
    read_store_data.count = 4096;
    read_store_data.buffer = (char*)malloc(read_store_data.count);
    if (read_store_data.buffer == NULL) {
        printf("malloc().. Error, <errno:%d>\n", errno);
        goto stop;
    }

    while (total_rsize < total_can_frame_size) {
        /* Read CAN data */
        unsigned int sec, usec;
        struct canfd_frame frame = { 0 };

        rsize = ioctl(fd, EP1_CF02A_IOCTL_READ_STORE_DATA, &read_store_data);
        total_rsize += rsize;

        if (rsize < 0) {
            printf("ioctl().. Error, <errno:%d> cmd=%s\n",
                   errno,
                   "EP1_CF02A_IOCTL_READ_STORE_DATA");
            free(read_store_data.buffer);
            break;
        }

        const int packet_num = rsize / EP1_CF02A_CAN_PACKET_SIZE;
        for (int num = 0, pos = 0; num < packet_num;
             ++num, pos += EP1_CF02A_CAN_PACKET_SIZE) {
            struct timespec timestamp = { 0 };
            unsigned char* buf = (unsigned char*)read_store_data.buffer;

            /* Getting timestamp from receiving data */
            sec = array_to_uint(&buf[pos]);
            usec = array_to_uint(&buf[pos + 4]);
            timespec_add(&timestamp, sec, usec * 1000);

            /* Getting CAN frame from receiving data */
            frame.can_id = array_to_uint(&buf[pos + 8]) & 0x1FFFFFFF;
            frame.len = can_dlc2len(buf[pos + 12] & 0x0F);
            frame.flags = buf[pos + 13];
            memcpy(frame.data, &buf[pos + 14], CANFD_MAX_DLEN);

            printf("%ld: ", can_count);
            print_canfd_frame(&timestamp, &frame);
            can_count++;
        }
    }

stop:
    /* Stop */
    start = false;
    result = set_store_data_rx_control(fd, id, start);
    if (result != RESULT_Success) {
        return RESULT_Failure;
    }

    /* Stop check */
    {
        ep1_cf02a_ioctl_get_store_data_rx_control_t store_data_rx_control = {
            0
        };

        memcpy(
          store_data_rx_control.id, id, EP1_CF02A_STORE_DATA_ID_MAX_LENGTH);
        result = get_store_data_rx_control(fd, &store_data_rx_control);
        if (result != RESULT_Success) {
            return RESULT_Failure;
        }
        if (start != store_data_rx_control.start) {
            printf("Stop check failed. start=%d\n",
                   store_data_rx_control.start);
            return RESULT_Failure;
        }
    }

    return RESULT_Success;
}

int
main(int argc, char* argv[])
{
    ep1_cf02a_ioctl_get_current_store_data_state_t current_store_data_state;
    ep1_cf02a_ioctl_get_store_data_id_list_t store_data_id_list;
    int fd;
    char* devname = NULL;
    bool is_read_store_data = false;
    int result;
    int n = 0;

    /*** Handle options ***/
    while (n >= 0) {
        n = getopt(argc, argv, "f:rhv");
        switch (n) {
        case 'f':
            devname = optarg;
            break;
        case 'r':
            is_read_store_data = true;
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

    result = ioctl(fd,
                   EP1_CF02A_IOCTL_GET_CURRENT_STORE_DATA_STATE,
                   &current_store_data_state);
    if (result != RESULT_Success) {
        printf(
          "get_current_store_data_state().. Error, <errno:%d> devname=%s\n",
          errno,
          devname);
        close(fd);
        return EXIT_FAILURE;
    }

    printf("     current store data state: ");
    switch (current_store_data_state.state) {
    case EP1_CF02A_STORE_DATA_STATE_STORING:
        printf("storing\n");
        break;
    case EP1_CF02A_STORE_DATA_STATE_NOT_STORING:
        printf("not storing\n");
        break;
    case EP1_CF02A_STORE_DATA_STATE_NOT_SUPPORTED:
        printf("not supported\n");
        break;
    default:
        printf("unknown\n");
        break;
    }

    printf("        current store data id: ");
    if (strlen(current_store_data_state.id) > 0) {
        printf("%.*s\n",
               EP1_CF02A_STORE_DATA_ID_MAX_LENGTH,
               current_store_data_state.id);
    } else {
        printf("none\n");
    }

    if (current_store_data_state.state ==
        EP1_CF02A_STORE_DATA_STATE_NOT_SUPPORTED) {
        printf("store data is not supported\n");
        close(fd);
        return EXIT_SUCCESS;
    }

    if (current_store_data_state.state == EP1_CF02A_STORE_DATA_STATE_STORING) {
        printf(
          "store data is storing now. stop storing before read store data\n");
        close(fd);
        return EXIT_SUCCESS;
    }

    result = get_store_data_id_list(fd, &store_data_id_list);
    if (result != RESULT_Success) {
        printf("get_store_data_list().. Error, <errno:%d> devname=%s\n",
               errno,
               devname);
        close(fd);
        return EXIT_FAILURE;
    }

    if (store_data_id_list.count == 0) {
        printf("store data id list is empty\n");
        close(fd);
        return EXIT_SUCCESS;
    }

    printf("             store data count: %d\n", store_data_id_list.count);

    for (unsigned int i = 0; i < store_data_id_list.count; i++) {
        ep1_cf02a_ioctl_get_store_data_meta_t store_data_meta = { 0 };

        result = get_store_data_meta(
          fd, store_data_id_list.id_list[i], &store_data_meta);
        if (result != RESULT_Success) {
            printf("get_store_data_meta().. Error, <errno:%d> devname=%s, "
                   "store_data_id=%.*s\n",
                   errno,
                   devname,
                   EP1_CF02A_STORE_DATA_ID_MAX_LENGTH,
                   store_data_id_list.id_list[i]);
            free_store_data_id_list(&store_data_id_list);
            close(fd);
            return EXIT_FAILURE;
        }

        printf(" --------------------------------------------------------------"
               "---------\n");
        printf("                        index: %d\n", i);
        printf("                store data id: %.*s\n",
               EP1_CF02A_STORE_DATA_ID_MAX_LENGTH,
               store_data_id_list.id_list[i]);
        printf(
          "             start time (RTC): %04d-%02d-%02d %02d:%02d %02d.%06d\n",
          2000 + store_data_meta.start_time.rtc.year,
          store_data_meta.start_time.rtc.month,
          store_data_meta.start_time.rtc.date,
          store_data_meta.start_time.rtc.hour,
          store_data_meta.start_time.rtc.minute,
          store_data_meta.start_time.rtc.second,
          store_data_meta.start_time.rtc.microsecond);
        printf("       start time (TimeStamp): %ld.%06ld\n",
               store_data_meta.start_time.ts.tv_sec,
               store_data_meta.start_time.ts.tv_nsec / 1000);
        printf("       reset time (TimeStamp): %ld.%06ld\n",
               store_data_meta.reset_time.device_ts.tv_sec,
               store_data_meta.reset_time.device_ts.tv_nsec / 1000);
        printf("  reset time (Host Monotonic): %ld.%09ld\n",
               store_data_meta.reset_time.host_monotonic.tv_sec,
               store_data_meta.reset_time.host_monotonic.tv_nsec);
        printf("              can_frame_count: %llu\n",
               store_data_meta.can_frame_count);

        if (is_read_store_data) {
            /* skip read store data if now storing */
            if (current_store_data_state.state ==
                  EP1_CF02A_STORE_DATA_STATE_STORING &&
                strncmp(current_store_data_state.id,
                        store_data_id_list.id_list[i],
                        EP1_CF02A_STORE_DATA_ID_MAX_LENGTH) == 0) {
                printf("skip read store data %.*s. now storing...\n",
                       EP1_CF02A_STORE_DATA_ID_MAX_LENGTH,
                       store_data_id_list.id_list[i]);
                continue;
            }

            result = read_store_data(fd,
                                     store_data_id_list.id_list[i],
                                     store_data_meta.can_frame_count);
            if (result != RESULT_Success) {
                printf("read_store_data().. Error, <errno:%d> devname=%s, "
                       "store_data_id=%.*s\n",
                       errno,
                       devname,
                       EP1_CF02A_STORE_DATA_ID_MAX_LENGTH,
                       store_data_id_list.id_list[i]);
            }
        }
    }

    free_store_data_id_list(&store_data_id_list);
    close(fd);

    return EXIT_SUCCESS;
}
