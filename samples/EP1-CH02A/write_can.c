#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "apt_usbtrx_ioctl.h"

int
main(void)
{
    /*
     * Set device path.
     * You can check the serial number of the device path
     * and the channel number (interface number) with the following command.
     * e.g.
     *   udevadm info --query=property /dev/aptUSB1 | grep -e ID_SERIAL -e ID_USB_INTERFACE_NUM
     */
    const char* devpath = "/dev/aptUSB1";
    int fd;
    int result;

    /* Open device */
    fd = open(devpath, O_WRONLY);
    if (fd == -1) {
        printf("open().. Error, <errno:%d> devpath=%s\n", errno, devpath);
        return EXIT_FAILURE;
    }

    {
        /*
         * Setting up CAN bitrate, listen-only mode.
         * When you want to send CAN data, set listen-only(silent) to false.
         */
        ep1_ch02a_ioctl_set_mode_t mode;
        mode.silent = false;
        mode.baudrate = 500;
        result = ioctl(fd, EP1_CH02A_IOCTL_SET_MODE, &mode);
        if (result == -1) {
            close(fd);
            return EXIT_FAILURE;
        }
    }

    /*
     * Start CAN sending.
     * After this, the device can send CAN data to CAN bus.
     */
    result = ioctl(fd, EP1_CH02A_IOCTL_START_CAN);
    if (result == -1) {
        close(fd);
        return EXIT_FAILURE;
    }

    {
        /*
         * CAN Frame buffer
         * The data format of the CAN frame is as follows.
         * https://github.com/aptpod/apt-peripheral-linux-driver/blob/main/docs/EP1-CH02A.ja.md#can-frame
         */
        const unsigned char buf[] = {
            0x01, 0x00, 0x00, 0x00,                        // id and frame type
            0x08,                                          // dlc
            0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09 // Data
        };

        /* Write CAN data */
        ssize_t wsize = write(fd, buf, sizeof(buf));
        if (wsize == -1) {
            printf("write().. Error, <errno:%d>\n", errno);
        }
    }

    /* Stop CAN sending */
    ioctl(fd, EP1_CH02A_IOCTL_STOP_CAN);

    close(fd);

    return EXIT_SUCCESS;
}
