#!/bin/bash -ex

DRIVER_FOLDER="drivers/aptpod"
APT_USBTRX_FOLDER="drivers/aptpod/apt_usbtrx"
TEST_APT_USBTRX_FOLDER="drivers/aptpod/test_apt_usbtrx"

if [ ! -d ./linux ]; then
    echo "Setup KUnit..."

    # install dependencies
    apt-get update -y && apt-get install git python3 libncurses-dev flex bison openssl libssl-dev dkms libelf-dev libudev-dev libpci-dev libiberty-dev autoconf bc -y

    # clone kernel v5.5
    git clone --depth 1 -b v5.5 git://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git
    cd linux

    # add apt_usbtrx config
    echo "CONFIG_KUNIT=y" > .kunitconfig
    echo "CONFIG_APT_USBTRX=y" >> .kunitconfig
    echo "CONFIG_APT_USBTRX_TEST=y" >> .kunitconfig

    # add Kconfig and Makefile
    mkdir -p ${APT_USBTRX_FOLDER}

    echo "obj-y += aptpod/" >> drivers/Makefile
    echo "source \"${DRIVER_FOLDER}/Kconfig\"" >> drivers/Kconfig

    echo -e "config APT_USBTRX\n \
            \tbool \"Kernel drivers for aptpod,Inc. products\"" >> ${DRIVER_FOLDER}/Kconfig
    echo -e "config APT_USBTRX_TEST\n \
            \tbool \"Test for apt_usbtrx\"\n \
            \tdepends on APT_USBTRX && KUNIT" >> ${DRIVER_FOLDER}/Kconfig

    echo "obj-\$(CONFIG_APT_USBTRX) += apt_usbtrx/" >> ${DRIVER_FOLDER}/Makefile
    echo "obj-\$(CONFIG_APT_USBTRX_TEST) += test_apt_usbtrx/" >> ${DRIVER_FOLDER}/Makefile

    echo "Setup KUnit complete!"
    cd ..
fi

# copy apt_usbtrx files
rm -rf linux/${APT_USBTRX_FOLDER}
mkdir -p linux/${APT_USBTRX_FOLDER}
cp -r ../module/* linux/${APT_USBTRX_FOLDER}
cp -r Makefile linux/${APT_USBTRX_FOLDER}

rm -rf linux/${TEST_APT_USBTRX_FOLDER}
mkdir -p linux/${TEST_APT_USBTRX_FOLDER}
cp -r test_apt_usbtrx/* linux/${TEST_APT_USBTRX_FOLDER}

# copy usb moc files
#  why: To build the apt_usbtrx driver.
#       UML (User Mode Linux) kernel used by KUnit fails to build because USB is not enabled.
cp -r mock/* linux/${APT_USBTRX_FOLDER}

# module could not be built with UML/KUnit, so change to the kernel built-in
sed -i -e 's/^obj-m/obj-y/g' linux/${APT_USBTRX_FOLDER}/Makefile

cd linux
./tools/testing/kunit/kunit.py run --jobs=`nproc --all`
cd ..
