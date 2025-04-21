/*
 * ep1_cf02a_delete_store_data.c - Delete store data.
 *
 * Copyright (C) 2024 aptpod Inc.
 */

#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "apt_usbtrx_ioctl.h"

#define PRGNAME "ep1_cf02a_delete_store_data"
#define PRGVERSION "1.0.0"

enum
{
    RESULT_Unknown = -1,
    RESULT_Success = 0,
    RESULT_Failure
};

char (*delete_ids)[EP1_CF02A_STORE_DATA_ID_MAX_LENGTH] = NULL;
size_t delete_ids_size = 0;
size_t delete_ids_capacity = 0;

int
stop_storing(int fd)
{
    int result;
    ep1_cf02a_ioctl_set_tx_rx_control_t tx_rx_control;

    tx_rx_control.start = true;
    result = ioctl(fd, EP1_CF02A_IOCTL_SET_TX_RX_CONTROL, &tx_rx_control);
    if (result == -1) {
        close(fd);
        return EXIT_FAILURE;
    }

    tx_rx_control.start = false;
    result = ioctl(fd, EP1_CF02A_IOCTL_SET_TX_RX_CONTROL, &tx_rx_control);
    if (result == -1) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
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

static int
add_delete_id(const char* id)
{
    size_t new_capacity = delete_ids_capacity + 1;

    char(*tmp)[EP1_CF02A_STORE_DATA_ID_MAX_LENGTH] =
      realloc(delete_ids, new_capacity * sizeof(*delete_ids));
    if (tmp == NULL) {
        printf("realloc().. Error, <errno:%d>\n", errno);
        return EXIT_FAILURE;
    }

    delete_ids = tmp;
    delete_ids_capacity = new_capacity;

    strncpy(
      delete_ids[delete_ids_size], id, EP1_CF02A_STORE_DATA_ID_MAX_LENGTH - 1);
    delete_ids[delete_ids_size][EP1_CF02A_STORE_DATA_ID_MAX_LENGTH - 1] = '\0';

    delete_ids_size++;

    return EXIT_SUCCESS;
}

static int
add_delete_id_all(int fd)
{
    ep1_cf02a_ioctl_get_store_data_id_list_t store_data_id_list = { 0 };
    int result;

    result = get_store_data_id_list(fd, &store_data_id_list);
    if (result != RESULT_Success) {
        printf("get_store_data_list().. Error, <errno:%d>\n", errno);
        return EXIT_FAILURE;
    }

    for (size_t i = 0; i < store_data_id_list.count; i++) {
        result = add_delete_id(store_data_id_list.id_list[i]);
        if (result != RESULT_Success) {
            free_store_data_id_list(&store_data_id_list);
            return EXIT_FAILURE;
        }
    }

    free_store_data_id_list(&store_data_id_list);

    return EXIT_SUCCESS;
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
    printf("usage: %s [-f <dev name>] [-i <id>] [-a] [-h] [-v]\n", PRGNAME);
    printf("options: \n");
    printf("  -f DEV_NAME,      Set device name\n");
    printf("  -i ID,            Set store data id to delete\n");
    printf("  -a                Delete all store data\n");
    printf("  -h,               This help text\n");
    printf("  -v,               Show version number\n");

    return RESULT_Success;
}

int
main(int argc, char* argv[])
{
    int fd;
    char* devname = NULL;
    char* delete_id = NULL;
    bool delete_all = false;
    int result;
    int n = 0;

    /*** Handle options ***/
    while (n >= 0) {
        n = getopt(argc, argv, "f:i:ahv");
        switch (n) {
        case 'f':
            devname = optarg;
            break;
        case 'i':
            delete_id = optarg;
            break;
        case 'a':
            delete_all = true;
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

    if (devname == NULL || (delete_id == NULL && delete_all == false)) {
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

    result = stop_storing(fd);
    if (result != RESULT_Success) {
        printf("stop_storing().. Error, <errno:%d>\n", errno);
        close(fd);
        return EXIT_FAILURE;
    }

    if (delete_all) {
        result = add_delete_id_all(fd);
    } else {
        result = add_delete_id(delete_id);
    }
    if (result != RESULT_Success) {
        printf("add_delete_id().. Error, <errno:%d>\n", errno);
        close(fd);
        free(delete_ids);
        return EXIT_FAILURE;
    }

    for (size_t i = 0; i < delete_ids_size; i++) {
        ep1_cf02a_ioctl_delete_store_data_t delete_store_data = { 0 };

        memcpy(delete_store_data.id,
               delete_ids[i],
               EP1_CF02A_STORE_DATA_ID_MAX_LENGTH);

        result =
          ioctl(fd, EP1_CF02A_IOCTL_DELETE_STORE_DATA, &delete_store_data);
        if (result == -1) {
            printf("ioctl().. Error, <errno:%d> cmd=%s\n",
                   errno,
                   "EP1_CF02A_IOCTL_DELETE_STORE_DATA");
            close(fd);
            return EXIT_FAILURE;
        }
        printf("store data id: %s is deleted\n", delete_ids[i]);
    }

    close(fd);
    free(delete_ids);

    return EXIT_SUCCESS;
}
