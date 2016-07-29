#!/bin/sh
# from /system/bin/rtc_sync.sh

while true
do

	if [ -e "/dev/rtc0" ]; then
		echo "found /dev/rtc0"
	else
		echo "not found /dev/rtc0"
		echo "init ds1307 on 0x68"
		echo ds1307 0x68 > /sys/class/i2c-adapter/i2c-1/new_device
		sleep 1
	fi

	RESULT=`hwclock -r`
	echo $RESULT
	if [ `echo $RESULT | grep -c "No such file or directory"` -gt 0 ]; then
		rmmod rtc_pcf8563
		break
	elif [ "$RESULT" == "" ]; then
		rmmod rtc_pcf8563
		break
	fi

	DATE=`busybox date -R`
	echo $DATE
	YEAR=`echo $DATE | cut -d ' ' -f 4`
	echo $YEAR
	if [ "$YEAR" == "1969" ] || [ "$YEAR" == "1970" ]; then
		hwclock -s
		echo "Set system time from hardware clock"
	fi

	RESULT=`getprop dhcp.wlan0.result`
	echo $RESULT
	if [ "$RESULT" == "ok" ]; then
		echo "connected wifi"
		echo "Set hardware clock from system time"
		hwclock -w
	else
		RESULT=`getprop dhcp.eth0.result`
		echo $RESULT
		if [ "$RESULT" == "ok" ]; then
			echo "connected ethernet"
			echo "Set hardware clock from system time"
			hwclock -w
		fi
	fi

	sleep 600

done
