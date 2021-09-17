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

static short
array_to_short(unsigned char* array)
{
    return array[0] | (array[1] << 8);
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

static int
pop_count(unsigned char x)
{
    int n = 0;
    for (; x != 0; x &= (x - 1)) {
        ++n;
    }
    return n;
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
         * Setting up sampling rate, voltage ranges.
         */
        ep1_ag08a_ioctl_set_analog_input_t settings;
        settings.send_rate = 1250000; // 1.25kHz
        for (int i = 0; i < EP1_AG08A_CH_NUM; ++i) {
            settings.vmin[i] = -10000; // -10V
            settings.vmax[i] = 10000;  // +10V
        }
        result = ioctl(fd, EP1_AG08A_IOCTL_SET_ANALOG_INPUT, &settings);
        if (result == -1) {
            close(fd);
            return EXIT_FAILURE;
        }
    }

    {
        /*
         * Start analog data receiving.
         * After this, the device will send analog data to the host.
         */
        ep1_ag08a_ioctl_control_analog_input_t ctrl = {
            .start = { false, false, false, false, false, false, false, false }
        };

        /* Activate channel 0, 3, 7 */
        ctrl.start[0] = true;
        ctrl.start[3] = true;
        ctrl.start[7] = true;

        result = ioctl(fd, EP1_AG08A_IOCTL_CONTROL_ANALOG_INPUT, &ctrl);
        if (result == -1) {
            close(fd);
            return EXIT_FAILURE;
        }
    }

    {
        /* Read analog data */
        unsigned int sec, usec;

        unsigned char buf[128];
        ssize_t rsize = read(fd, buf, sizeof(buf));

        /* Multiple analog data will be stored in the receive buffer. */
        for (int pos = 0; pos < rsize;) {
            struct timespec timestamp = basetime;
            int ch_num;

            /* Getting timestamp from receiving data. */
            sec = array_to_uint(&buf[pos]);
            pos += 4;
            usec = array_to_uint(&buf[pos]);
            pos += 4;

            timespec_add(&timestamp, sec, usec * 1000);

            /* Count the number of active channels. */
            ch_num = pop_count(buf[pos]);
            pos += 1;

            printf("[%ld.%09ld] ch num: %d, data:",
                   timestamp.tv_sec,
                   timestamp.tv_nsec,
                   ch_num);

            /* Convert analog data array to short value */
            for (int i = 0; i < ch_num; ++i) {
                short value = array_to_short(&buf[pos]);
                printf(" %d,", value);
                pos += 2;
            }
            printf("\n");
        }
    }

    {
        /*
         * Stop analog data receiving.
         */
        ep1_ag08a_ioctl_control_analog_input_t ctrl = {
            .start = { false, false, false, false, false, false, false, false }
        };
        ioctl(fd, EP1_AG08A_IOCTL_CONTROL_ANALOG_INPUT, &ctrl);
    }

    close(fd);

    return EXIT_SUCCESS;
}
