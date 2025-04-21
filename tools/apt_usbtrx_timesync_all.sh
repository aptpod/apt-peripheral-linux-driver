#!/usr/bin/env bash
#
# Copyright (c) 2021, aptpod,Inc.
#

PATH="$PATH:/usr/local/bin"
ENABLETS=apt_usbtrx_enablets
RESETTS=apt_usbtrx_resetts
SET_HOST_RESETTS=ep1_cf02a_set_host_resetts
SERIALNO=apt_usbtrx_serial_no

: ${BASETIME_CLOCK_ID:=CLOCK_MONOTONIC_RAW}

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
	echo "${BASETIME_CLOCK_ID}" >"/sys/$(udevadm info --query=path --name=$device)/device/basetime_clock_id"
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

# Set host timestamp reset time for EP1-CF02A
for device in $devices; do
	model=$(udevadm info --query=property --name=$device | grep '^ID_MODEL=' | cut -d'=' -f2)
	if [ "$model" != "EP1-CF02A" ]; then
		continue
	fi

	${SET_HOST_RESETTS} -f "$device"
	if [ $? -eq 0 ]; then
		echo "set host timestamp reset time for $device"
	else
		echo "failed to set host timestamp reset time for $device"
	fi
done
