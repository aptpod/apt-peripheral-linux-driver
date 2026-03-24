/*
 * ep1_cf02a_get_store_data_list.c - Get store data list.
 *
 * Copyright (C) 2024 aptpod Inc.
 */

#include <endian.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/can.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
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

#define CAN_FRAME_SIZE 78 //  sizeof(ep1_cf02a_payload_notify_recv_can_frame_t)
#define BUFFER_FRAME_COUNT 64
#define BUFFER_SIZE (CAN_FRAME_SIZE * BUFFER_FRAME_COUNT)
#define MAX_CAN_IDS 256

typedef struct
{
    uint32_t can_id;
    uint64_t last_data;
    bool has_data;
} can_data_tracker_t;

typedef struct
{
    char id[EP1_CF02A_STORE_DATA_ID_MAX_LENGTH];
    uint64_t lost_frames;
    uint64_t error_frames;
    bool has_data_loss;
    bool has_error_frames;
} store_data_result_t;

static can_data_tracker_t can_trackers[MAX_CAN_IDS];
static int tracker_count = 0;
static uint64_t total_lost_frames = 0;
static uint64_t total_error_frames = 0;
static store_data_result_t* store_results = NULL;
static int store_data_count = 0;
static int processed_count = 0;

static volatile bool stop_requested = false;
char store_data_read_buffer[BUFFER_SIZE];

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

static uint64_t
parse_u64_be(const unsigned char* data, int len)
{
    if (len < 8) {
        return 0;
    }
    return be64toh(*(uint64_t*)data);
}

static can_data_tracker_t*
find_or_create_tracker(uint32_t can_id)
{
    for (int i = 0; i < tracker_count; i++) {
        if (can_trackers[i].can_id == can_id) {
            return &can_trackers[i];
        }
    }

    if (tracker_count >= MAX_CAN_IDS) {
        return NULL;
    }

    can_trackers[tracker_count].can_id = can_id;
    can_trackers[tracker_count].has_data = false;
    return &can_trackers[tracker_count++];
}

static void
reset_trackers()
{
    tracker_count = 0;
    total_lost_frames = 0;
    total_error_frames = 0;
    memset(can_trackers, 0, sizeof(can_trackers));
}

static bool
has_errors_or_losses(uint64_t* total_lost, uint64_t* total_error)
{
    bool has_any_loss = false;
    bool has_any_error = false;
    uint64_t total_all_lost = 0;
    uint64_t total_all_error = 0;

    for (int i = 0; i < processed_count; i++) {
        if (store_results[i].has_data_loss) {
            has_any_loss = true;
            total_all_lost += store_results[i].lost_frames;
        }
        if (store_results[i].has_error_frames) {
            has_any_error = true;
            total_all_error += store_results[i].error_frames;
        }
    }

    if (total_lost) *total_lost = total_all_lost;
    if (total_error) *total_error = total_all_error;

    return has_any_loss || has_any_error;
}

static void
print_summary(bool is_check_mode)
{
    if (!is_check_mode || processed_count == 0) {
        return;
    }

    printf("\n--- Summary ---\n");

    uint64_t total_all_lost = 0;
    uint64_t total_all_error = 0;
    bool has_any_issues = has_errors_or_losses(&total_all_lost, &total_all_error);

    if (!has_any_issues) {
        printf("All store data: OK (no data loss, no error frames detected)\n");
    } else {
        if (total_all_lost > 0) {
            printf("Data loss detected in the following store data:\n");
            for (int i = 0; i < processed_count; i++) {
                if (store_results[i].has_data_loss) {
                    printf("  %.*s: %lu lost frames\n",
                           EP1_CF02A_STORE_DATA_ID_MAX_LENGTH,
                           store_results[i].id,
                           store_results[i].lost_frames);
                }
            }
            printf("Total lost frames across all store data: %lu\n",
                   total_all_lost);
        }

        if (total_all_error > 0) {
            printf("Error frames detected in the following store data:\n");
            for (int i = 0; i < processed_count; i++) {
                if (store_results[i].has_error_frames) {
                    printf("  %.*s: %lu error frames\n",
                           EP1_CF02A_STORE_DATA_ID_MAX_LENGTH,
                           store_results[i].id,
                           store_results[i].error_frames);
                }
            }
            printf("Total error frames across all store data: %lu\n",
                   total_all_error);
        }
    }
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
    printf("usage: %s [-f <dev name>] [-r] [-c] [-h] [-v]\n", PRGNAME);
    printf("options: \n");
    printf("  -f DEV_NAME,      Set device name\n");
    printf("  -r                Read store data\n");
    printf("  -c                Incremental check mode (no data display)\n");
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

    printf("             store data count: %d\n", store_data_id_list->count);
    printf("get store data id list...\n");

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
read_store_data(int fd,
                const char* id,
                unsigned long long can_frame_count,
                bool is_check_mode)
{
    int result;
    bool error = false;
    bool start;
    long rsize = 0;
    ssize_t total_rsize = 0;
    ssize_t total_can_frame_size = can_frame_count * EP1_CF02A_CAN_PACKET_SIZE;
    size_t can_count = 1;

    /* Reset trackers for new read session */
    reset_trackers();

    /* Start */
    start = true;
    result = set_store_data_rx_control(fd, id, start);
    if (result != RESULT_Success) {
        return RESULT_Failure;
    }

    /* Read store data */
    while (total_rsize < total_can_frame_size && !stop_requested) {
        /* Read CAN data */
        unsigned int sec, usec;
        struct canfd_frame frame = { 0 };

        ep1_cf02a_ioctl_read_store_data_t read_store_data;
        read_store_data.buffer = store_data_read_buffer;
        read_store_data.count = sizeof(store_data_read_buffer);
        result = ioctl(fd, EP1_CF02A_IOCTL_READ_STORE_DATA, &read_store_data);
        if (result < 0) {
            printf("ioctl().. Error, <errno:%d> cmd=%s\n",
                   errno,
                   "EP1_CF02A_IOCTL_READ_STORE_DATA");
            error = true;
            goto stop;
        }

        rsize = read_store_data.count;
        if (rsize == 0) {
            printf("read timeout. no data received.\n");
            /* Stop check */
            {
                ep1_cf02a_ioctl_get_store_data_rx_control_t
                  store_data_rx_control = { 0 };
                result = get_store_data_rx_control(fd, &store_data_rx_control);
                if (!store_data_rx_control.start &&
                    total_rsize != total_can_frame_size) {
                    printf("read error (%ld / %ld frames)\n",
                           total_rsize,
                           total_can_frame_size);
                    error = true;
                    goto stop;
                }
            }
            continue;
        }

        total_rsize += rsize;

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

            /* Check data increment only in check mode */
            if (is_check_mode) {
                can_data_tracker_t* tracker =
                  find_or_create_tracker(frame.can_id);
                if (tracker == NULL) {
                    printf("Error: Too many CAN IDs (max %d supported)\n",
                           MAX_CAN_IDS);
                    total_error_frames++;
                }
                if (frame.len != 8) {
                    printf("Error: CAN frame data length must be 8 bytes (got "
                           "%d bytes for CAN ID %x)\n",
                           frame.len,
                           frame.can_id);
                    total_error_frames++;
                }
                if (tracker != NULL && frame.len == 8) {
                    uint64_t current_data = parse_u64_be(frame.data, frame.len);

                    if (tracker->has_data) {
                        if (current_data > tracker->last_data + 1) {
                            uint64_t lost_frames =
                              current_data - tracker->last_data - 1;
                            total_lost_frames += lost_frames;
                            printf(
                              "[%ld.%09ld] data mismatch for CAN ID %x: "
                              "expected %lu, got %lu, lost frame count %lu\n",
                              timestamp.tv_sec,
                              timestamp.tv_nsec,
                              frame.can_id,
                              tracker->last_data + 1,
                              current_data,
                              lost_frames);
                        } else if (current_data < tracker->last_data + 1) {
                            uint64_t rewind_count =
                              tracker->last_data - current_data;
                            printf(
                              "[%ld.%09ld] timestamp rewind for CAN ID %x: "
                              "expected %lu, got %lu, rewind count %lu\n",
                              timestamp.tv_sec,
                              timestamp.tv_nsec,
                              frame.can_id,
                              tracker->last_data + 1,
                              current_data,
                              rewind_count);
                        }
                    }

                    /* Update last_data if current_data is valid */
                    if (tracker->has_data) {
                        if (current_data >= tracker->last_data + 1) {
                            tracker->last_data = current_data;
                        }
                    } else {
                        tracker->last_data = current_data;
                    }
                    tracker->has_data = true;
                }
            }

            if (!is_check_mode) {
                printf("%ld: ", can_count);
                print_canfd_frame(&timestamp, &frame);
            }
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

    if (is_check_mode) {
        /* Store result for this store data */
        strncpy(store_results[processed_count].id,
                id,
                EP1_CF02A_STORE_DATA_ID_MAX_LENGTH);
        store_results[processed_count].lost_frames = total_lost_frames;
        store_results[processed_count].error_frames = total_error_frames;
        store_results[processed_count].has_data_loss = (total_lost_frames > 0);
        store_results[processed_count].has_error_frames =
          (total_error_frames > 0);
        processed_count++;

        printf("                 check result: ");
        if (total_lost_frames > 0 || total_error_frames > 0) {
            if (total_lost_frames > 0) {
                printf("DATA LOSS detected (%lu lost frames)",
                       total_lost_frames);
            }
            if (total_error_frames > 0) {
                if (total_lost_frames > 0)
                    printf(", ");
                printf("ERROR FRAMES detected (%lu error frames)",
                       total_error_frames);
            }
            printf("\n");
        } else {
            printf("OK (no data loss, no error frames)\n");
        }
    }

    return error ? RESULT_Failure : RESULT_Success;
}

int
main(int argc, char* argv[])
{
    ep1_cf02a_ioctl_get_current_store_data_state_t current_store_data_state;
    ep1_cf02a_ioctl_get_store_data_id_list_t store_data_id_list;
    int fd;
    char* devname = NULL;
    bool is_read_store_data = false;
    bool is_check_mode = false;
    int result;
    int n = 0;

    /*** Handle options ***/
    while (n >= 0) {
        n = getopt(argc, argv, "f:rchv");
        switch (n) {
        case 'f':
            devname = optarg;
            break;
        case 'r':
            is_read_store_data = true;
            break;
        case 'c':
            is_check_mode = true;
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

    struct sigaction sa;
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);

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

    store_data_count = store_data_id_list.count;

    if (store_data_count == 0) {
        printf("store data id list is empty\n");
        close(fd);
        return EXIT_SUCCESS;
    }

    /* Allocate memory for store results based on actual count */
    if (is_check_mode) {
        processed_count = 0;
        store_results = malloc(store_data_count * sizeof(store_data_result_t));
        if (store_results == NULL) {
            printf("Memory allocation failed\n");
            free_store_data_id_list(&store_data_id_list);
            close(fd);
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < store_data_count; i++) {
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

        if (is_read_store_data && store_data_meta.can_frame_count > 0) {
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
                                     store_data_meta.can_frame_count,
                                     is_check_mode);
            if (result != RESULT_Success) {
                printf("read_store_data().. Error, <errno:%d> devname=%s, "
                       "store_data_id=%.*s\n",
                       errno,
                       devname,
                       EP1_CF02A_STORE_DATA_ID_MAX_LENGTH,
                       store_data_id_list.id_list[i]);
            }

            if (stop_requested) {
                break;
            }
        }
    }

    print_summary(is_check_mode);

    /* Check for errors in check mode and return failure if detected */
    bool has_any_errors = false;
    if (is_check_mode && store_results != NULL && processed_count > 0) {
        has_any_errors = has_errors_or_losses(NULL, NULL);
    }

    if (store_results != NULL) {
        free(store_results);
        store_results = NULL;
    }

    free_store_data_id_list(&store_data_id_list);
    close(fd);

    return has_any_errors ? EXIT_FAILURE : EXIT_SUCCESS;
}
