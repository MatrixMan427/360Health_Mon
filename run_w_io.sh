#!/bin/bash

#This script will retrieve the I/O values for the disks and  run the sys_health kernel program concurrently

make

sudo insmod sys_health_monitor.ko mem_threshold=200

#check logs (possibly take out!?) 

dmesg | grep GiggleShits

cat /proc/sys_health

dstat -r

;;
