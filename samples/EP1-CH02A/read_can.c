#include <errno.h>
#include <fcntl.h>
#include <linux/can.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "apt_usbtrx_ioctl.h"

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
print_can_frame(struct timespec* timestamp, struct can_frame* frame)
{
    printf("[%ld.%09ld] id: %x, dlc: %d, data:",
           timestamp->tv_sec,
           timestamp->tv_nsec,
           frame->can_id,
           frame->can_dlc);
    for (int i = 0; i < sizeof(frame->data); ++i) {
        printf(" %02x", frame->data[i]);
    }
    printf("\n");
}

int
main(int argc, char* argv[])
{
    const char* devpath = "/dev/aptUSB0";
    int fd;
    int result;
    struct timespec basetime;

    /* Open device */
    fd = open(devpath, O_RDONLY);
    if (fd == -1) {
        printf("open().. Error, <errno:%d> devpath=%s\n", errno, devpath);
        return EXIT_FAILURE;
    }

    /*
     * Getting base time.
     * Base time is the time when the timestamp started.
     */
    result = ioctl(fd, APT_USBTRX_IOCTL_GET_BASETIME, &basetime);
    if (result == -1) {
        close(fd);
        return EXIT_FAILURE;
    }

    {
        /*
         * Setting up CAN bitrate, listen-only mode.
         */
        ep1_ch02a_ioctl_set_mode_t mode;
        mode.silent = true;
        mode.baudrate = 500;
        result = ioctl(fd, EP1_CH02A_IOCTL_SET_MODE, &mode);
        if (result == -1) {
            close(fd);
            return EXIT_FAILURE;
        }
    }

    /*
     * Start CAN receiving.
     * After this, the device will send CAN data to the host.
     */
    result = ioctl(fd, EP1_CH02A_IOCTL_START_CAN);
    if (result == -1) {
        close(fd);
        return EXIT_FAILURE;
    }

    {
        /* Read CAN data */
        unsigned int sec, usec;
        struct can_frame frame;

        unsigned char buf[128];
        ssize_t rsize = read(fd, buf, sizeof(buf));

        /* Multiple CAN data will be stored in the receive buffer. */
        const int packet_num = rsize / EP1_CH02A_CAN_PACKET_SIZE;
        for (int num = 0, pos = 0; num < packet_num;
             ++num, pos += EP1_CH02A_CAN_PACKET_SIZE) {
            struct timespec timestamp = basetime;

            /* Getting timestamp from receiving data */
            sec = array_to_uint(&buf[pos]);
            usec = array_to_uint(&buf[pos + 4]);
            timespec_add(&timestamp, sec, usec * 1000);

            /* Getting CAN frame from receiving data */
            frame.can_id = array_to_uint(&buf[pos + 8]) & 0x1FFFFFFF;
            frame.can_dlc = buf[pos + 12] & 0x0F;
            memcpy(frame.data, &buf[pos + 13], CAN_MAX_DLEN);

            print_can_frame(&timestamp, &frame);
        }
    }

    /* Stop CAN receiving */
    ioctl(fd, EP1_CH02A_IOCTL_STOP_CAN);

    close(fd);

    return EXIT_SUCCESS;
}
