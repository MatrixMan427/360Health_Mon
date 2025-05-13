Due to the recent deprectation of kernel modules for disk I/O monitoring, we opted instead to use dstat/pcp in a shell script that also runs our kernel program concurrently

To run the full program, simply run "sudo sh run_w_io.sh", which will run the sys_health_monitor.c program first, then will print out the disk I/O stats until interrupted with Ctrl+C.
