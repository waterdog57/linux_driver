#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/uaccess.h>

static dev_t dev;
static struct cdev *demo_cdev[2];
static struct class *my_class;
static signed int count = 2;

static int demodrv_open(struct inode *inode, struct file *file) {
  printk("open\n");
  int major = MAJOR(inode->i_rdev);
  int minor = MINOR(inode->i_rdev);
  printk("%s, major = %d, mirno = %d\n", __func__, major, minor);

  return 0;
}
static int demodrv_release(struct inode *inode, struct file *file) { return 0; }
static ssize_t demodrv_read(struct file *file, char __user *buf, size_t lbuf,
                            loff_t *ppos) {
  printk("read\n");
  return 0;
}
static ssize_t demodrv_write(struct file *file, const char __user *buf,
                             size_t count, loff_t *f_pos) {
  printk("write\n");
  return 0;
}

static const struct file_operations demodrv_fops = {.owner = THIS_MODULE,
                                                    .open = demodrv_open,
                                                    .release = demodrv_release,
                                                    .read = demodrv_read,
                                                    .write = demodrv_write};

static int __init my_init(void) {

  int ret;
  int i;
  int pass;

  ret = alloc_chrdev_region(&dev, 0, count, "my_char_dev");
  if (ret) {
    printk("allocate fail");
    return ret;
  }

  my_class = class_create("my_char_class");
  for (i = 0; i < count; i++) {
    printk("i = %d\n", i);
    demo_cdev[i] = cdev_alloc();
    if (!demo_cdev[i]) {
      printk("cdev alloc fail");
      goto unregister_chrdev;
    }

    cdev_init(demo_cdev[i], &demodrv_fops);

    ret = cdev_add(demo_cdev[i], dev, i);
    if (ret) {
      printk("cdev add fail");
      pass = i;
      goto cdev_fail;
    }

    device_create(my_class, NULL, dev, NULL, "my_char_dev%d", i);
  }
  return 0;

cdev_fail:
  for (; i >= 0; i--) {
    if (i <= pass - 1) {
      device_destroy(my_class, MKDEV(MAJOR(dev), MINOR(dev) + i));
    }
    if (demo_cdev[i]) {

      cdev_del(demo_cdev[i]);
    }
  }
unregister_chrdev:
  unregister_chrdev_region(dev, count);

  return ret;
}

static void __exit my_exit(void) {
  printk("my_exit\n");
  int i;
  for (i = 0; i < count; i++) {
    if (demo_cdev[i]) {
      cdev_del(demo_cdev[i]);
    }
  }

  if (my_class) {
    int j;
    for (j = 0; j < count; j++) {
      device_destroy(my_class, MKDEV(MAJOR(dev), MINOR(dev) + j));
    }
    class_destroy(my_class);
  }
  unregister_chrdev_region(dev, count);
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("waterdog");
MODULE_DESCRIPTION("simple character device");
MODULE_ALIAS("mytest");
