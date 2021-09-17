#!/usr/bin/env bash
#
# Copyright (c) 2021, aptpod,Inc.
#

PROGDIR=/usr/local/bin
ENABLETS=${PROGDIR}/apt_usbtrx_enablets
RESETTS=${PROGDIR}/apt_usbtrx_resetts
SERIALNO=${PROGDIR}/apt_usbtrx_serial_no

devices=$(find /dev -name "aptUSB*" | sort)

reset_masters=()

if [ -z "$devices" ]; then
	echo "no device found"
	exit 1
fi

if [ "$EUID" -ne 0 ]; then
	echo "please run as root"
	exit 1
fi

for device in $devices; do
	${ENABLETS} -f "$device"
	sync=$(${SERIALNO} -f "$device" -p)
	echo "enable timestamp reset for $device ($sync)"
done

for device in $devices; do
	sync=$(${SERIALNO} -f "$device" -p)
	if [ "$sync" != "source" ]; then
		continue
	fi

	serial=$(${SERIALNO} -f "$device" -s)
    if [[ ! $(printf '%s\n' "${reset_masters[@]}" | grep -qx "${serial}"; echo -n ${?} ) -eq 0 ]]; then
		echo "reset timestamp of $device"
		${RESETTS} -f "$device"
		reset_masters+=("${serial}")
	fi
done
