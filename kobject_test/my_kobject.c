#include <linux/kobject.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sysfs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple kobject example");


static struct kobject *my_kobj;
static ssize_t my_kobj_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "Hello from my kobject!\n");
}
static ssize_t my_kobj_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    pr_info("Received input: %s", buf);
    return count;
}
static struct kobj_attribute my_kobj_attr = __ATTR(my_attr, 0664, my_kobj_show, my_kobj_store);
static int __init my_kobject_init(void)
{
    int error;
    my_kobj = kobject_create_and_add("my_kobject", kernel_kobj);
    if (!my_kobj) {
        pr_err("Failed to create kobject\n");
        return -ENOMEM;
    }
    error = sysfs_create_file(my_kobj, &my_kobj_attr.attr);
    if (error) {
        pr_err("Failed to create sysfs file\n");
        kobject_put(my_kobj);
        return error;
    }
    pr_info("Kobject created successfully\n");
    return 0;
}
static void __exit my_kobject_exit(void)
{
    kobject_put(my_kobj);
    pr_info("Kobject removed successfully\n");
}
module_init(my_kobject_init);
module_exit(my_kobject_exit);