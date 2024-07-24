#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>

static dev_t dev;
static struct cdev *demo_cdev;
static signed int count = 1;


static int demodrv_open(struct inode *inode, struct file *file)
{
	printk("open\n");
	int major = MAJOR(inode->i_rdev);
	int minor = MINOR(inode->i_rdev);
	printk("%s, major = %d, mirno = %d\n",__func__,major,minor);

	return 0;
}
static int demodrv_release(struct inode *inode, struct file *file)
{
	return 0;
}
static ssize_t demodrv_read(struct file *file, char __user *buf, size_t lbuf, loff_t *ppos)
{
	printk("read\n");
	return 0;
}
static ssize_t demodrv_write(struct file *file,const char __user *buf, size_t count, loff_t *f_pos)
{
	printk("write\n");
	return 0;
}


static const struct file_operations demodrv_fops = {
	.owner = THIS_MODULE,
	.open = demodrv_open,
	.release = demodrv_release,
	.read = demodrv_read,
	.write = demodrv_write
};



static int __init my_init(void){

	int ret;

	ret = alloc_chrdev_region(&dev,0,count, "my_char_dev");
	if( ret ){
		printk("allocate fail");
		return ret;
	}
	demo_cdev = cdev_alloc();
	if(!demo_cdev){
		printk("cdev alloc fail");
		goto unregister_chrdev;
	}

	cdev_init( demo_cdev, &demodrv_fops );

	ret = cdev_add(demo_cdev,dev,count);
	if(ret){
		printk("cdev add fail");
		goto cdev_fail;
	}
	printk("major = %d, mirno = %d\n",MAJOR(dev),MINOR(dev));

	return 0;

cdev_fail:
	cdev_del(demo_cdev);
unregister_chrdev:
	unregister_chrdev_region(dev, count);

	return ret;

}

void __exit my_exit(void){
	printk("my_exit\n");

	if(demo_cdev){
		cdev_del(demo_cdev);
	}
	unregister_chrdev_region(dev, count);

}

module_init(my_init);
module_exit(my_exit);


MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("waterdog");
MODULE_DESCRIPTION("simple character device");
MODULE_ALIAS("mytest");
