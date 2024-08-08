#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/workqueue.h>

struct my_work {
    struct work_struct work;
    int data;
};

static struct workqueue_struct *my_workqueue;

static void my_work_func(struct work_struct *work)
{
    struct my_work *my_work = container_of(work, struct my_work, work);
    printk(KERN_INFO "Executing work with data: %d\n", my_work->data);
	printk(KERN_INFO "Work executed on CPU %d\n", raw_smp_processor_id());
    kfree(my_work);
}

static int example_init(void)
{
    my_workqueue = alloc_workqueue("my_workqueue", WQ_UNBOUND, 0);
    if (!my_workqueue)
        return -ENOMEM;

    struct my_work *my_work = kmalloc(sizeof(*my_work), GFP_KERNEL);
    if (!my_work) {
        destroy_workqueue(my_workqueue);
        return -ENOMEM;
    }

    my_work->data = 42;
    INIT_WORK(&my_work->work, my_work_func);
    queue_work(my_workqueue, &my_work->work);

    printk(KERN_INFO "Example module loaded.\n");
    return 0;
}

static void example_exit(void)
{
    flush_workqueue(my_workqueue);
    destroy_workqueue(my_workqueue);
    printk(KERN_INFO "Example module unloaded.\n");
}

module_init(example_init);
module_exit(example_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Example usage of work_queue in Linux kernel");