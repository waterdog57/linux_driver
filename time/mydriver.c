#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/jiffies.h>
#include <linux/delay.h>
#include <linux/ktime.h>

#define MYTAG "[MYDRV]"

static int __init my_ini(void) {
    unsigned long jiffies_now = jiffies;
    unsigned long jiffies_after = jiffies + 1000; // 1 second later
    unsigned long jiffies_diff = jiffies_after - jiffies_now;
    pr_info(MYTAG "Current jiffies: %lu\n", jiffies_now);
    pr_info(MYTAG "Jiffies after 1 second: %lu\n", jiffies_after);
    pr_info(MYTAG "Difference in jiffies: %lu\n", jiffies_diff);

    // basie time test
    unsigned long start,end,diff;
    start = jiffies;
    // Simulate some work
    msleep(1000); // Sleep for 1 second
    end = jiffies;
    diff = end - start;
    pr_info(MYTAG "Difference in jiffies: %lu\n", end-start);
    pr_info(MYTAG "jiffies_to_msecs: %u\n", jiffies_to_msecs(diff));


    // ktime test, < 1ms
    // ex. how long does it take to execute a dma
    ktime_t start_ktime, end_ktime, diff_ktime;
    start_ktime = ktime_get();
    // Simulate some work
    msleep(1000); // Sleep for 1 second
    end_ktime = ktime_get();
    diff_ktime = ktime_sub(end_ktime, start_ktime);
    pr_info(MYTAG "Difference in ktime: %lld ns\n", ktime_to_ns(diff_ktime));


    // pr_info(MYTAG "Module initialized\n");
    return 0;
}
static void __exit my_exit(void) { 
    pr_info(MYTAG "Module exited\n"); 
}

module_init(my_ini);
module_exit(my_exit);

MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple Linux driver example");
MODULE_LICENSE("GPL");
