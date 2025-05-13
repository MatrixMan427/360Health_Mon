// SPDX-License-Identifier: GPL-2.0
/*
 * sys_health_monitor.c - System Health Monitor Kernel Module
 * Group Name: GiggleShits
 * Members: Michael Lavallee, Christian Capaccio, Gabriel Dugarte
 */

#include <linux/mm.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/sysinfo.h>
#include <linux/moduleparam.h>
#include <linux/sched/loadavg.h>
#include <linux/blkdev.h>


#define PROC_FILENAME "sys_health"

#define PROC_DISKSTATS "/proc/diskstats"

#define TIMER_INTERVAL_SEC 5
#define GROUP_TAG "[GiggleShits]"

static struct timer_list sys_health_timer;
static struct sysinfo cached_info;
static unsigned long cached_load;
static int mem_threshold = 1000; // in MB

module_param(mem_threshold, int, 0644);
MODULE_PARM_DESC(mem_threshold, "Memory usage threshold in MB");

static struct proc_dir_entry *proc_entry;

/* Timer callback */
static void collect_metrics(struct timer_list *t)
{
    si_meminfo(&cached_info);
    cached_load = (avenrun[0] * 100) >> FSHIFT;

    unsigned long free_ram_mb = cached_info.freeram * cached_info.mem_unit / 1024 / 1024;

    if (free_ram_mb < mem_threshold) {
        printk(KERN_WARNING GROUP_TAG "Alert: Free memory (%lu MB) below threshold (%d MB)\n",
               free_ram_mb, mem_threshold);
    }

    mod_timer(&sys_health_timer, jiffies + TIMER_INTERVAL_SEC * HZ);
}

/* /proc file output */
static int sys_health_proc_show(struct seq_file *m, void *v)
{
    unsigned long total_ram_mb = cached_info.totalram * cached_info.mem_unit / 1024 / 1024;
    unsigned long free_ram_mb = cached_info.freeram * cached_info.mem_unit / 1024 / 1024;
    unsigned long used_ram_mb = total_ram_mb - free_ram_mb;
    //ADD PERCENTAGES!!!
    seq_printf(m, "=== System Health Monitor ===\n");
    seq_printf(m, "Total RAM: %lu MB\n", total_ram_mb);
    seq_printf(m, "Free RAM: %lu MB", free_ram_mb);
    
    //Adding in Used RAM
    seq_printf(m, "Used RAM: %lu MB\n", used_ram_mb);
    seq_printf(m, "CPU Load (1 min avg): %lu.%02lu %%\n", cached_load / 100, cached_load % 100);
    
    //unsigned long disk_read_io = 
    //system(cat /proc/diskstats|grep "sda ");
    //seq_printf(m, "\ndisk_read_io\n");
    
    return 0;
}

static int sys_health_proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, sys_health_proc_show, NULL);
}

static const struct proc_ops proc_file_ops = {
    .proc_open = sys_health_proc_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

/* Init */
static int __init sys_health_init(void)
{
    printk(KERN_INFO GROUP_TAG "Loading System Health Monitor Module by Michael Lavallee, Christian Cappacio, and Gabriel Dugarte\n");

    proc_entry = proc_create(PROC_FILENAME, 0444, NULL, &proc_file_ops);
    if (!proc_entry) {
        printk(KERN_ERR GROUP_TAG "Failed to create /proc/%s\n", PROC_FILENAME);
        return -ENOMEM;
    }

    timer_setup(&sys_health_timer, collect_metrics, 0);
    mod_timer(&sys_health_timer, jiffies + TIMER_INTERVAL_SEC * HZ);

    return 0;
}

/* Exit */
static void __exit sys_health_exit(void)
{
    printk(KERN_INFO GROUP_TAG "Unloading System Health Monitor Module by Michael Lavallee, Christian Cappacio, and Gabriel Dugarte\n");

    del_timer_sync(&sys_health_timer);

    if (proc_entry)
        proc_remove(proc_entry);
}

module_init(sys_health_init);
module_exit(sys_health_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("GiggleShits - Michael Lavallee, Christian Cappacio, and Gabriel Dugarte");
MODULE_DESCRIPTION("System Health Monitor Kernel Module (Kernel 6.11 Compatible)");

