#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/kfifo.h>
#include <linux/wait.h>
#include <linux/miscdevice.h>

DEFINE_KFIFO( device_buff, char , 8 );


static struct device *demodrv_device;


struct my_queue_driver
{
	struct miscdevice *miscdev;
	wait_queue_head_t read_queue;
	wait_queue_head_t write_queue;
};
static struct my_queue_driver device;

static int demodrv_open(struct inode *inode, struct file *file)
{
	printk("open\n");
	int major = MAJOR(inode->i_rdev);
	int minor = MINOR(inode->i_rdev);
	printk("%s, major = %d, mirno = %d\n",__func__,major,minor);

	return 0;
}
static int 
demodrv_release(struct inode *inode, struct file *file)
{
	return 0;
}
static ssize_t 
demodrv_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
	int ret;
	int actual_read;

	if(kfifo_is_empty(&device_buff)){
		if(file->f_flags & O_NONBLOCK)
			return -EAGAIN;
		printk("read pid %d, going to sleep\n", current->pid);
		ret = wait_event_interruptible( device.read_queue, !(kfifo_is_empty(&device_buff)));
		if (ret)
			return ret;
	}

	ret = kfifo_to_user( &device_buff, buf, count, &actual_read );
	if(ret){
		return -EFAULT;
	}

	if(!kfifo_is_full(&device_buff)){
		wake_up_interruptible(&device.write_queue);
	}

	printk("read %ld actual_read %d\n", count, actual_read);

	return actual_read;
}
static ssize_t 
demodrv_write(struct file *file,const char __user *buf, size_t count, loff_t *f_pos)
{
	int ret;
	unsigned int actual_write;

	if(kfifo_is_full(&device_buff)){
		if(file->f_flags & O_NONBLOCK)
			return -EAGAIN;
		printk("write pid %d, going to sleep\n", current->pid);
		ret = wait_event_interruptible(device.write_queue, !(kfifo_is_full(&device_buff)));
		if (ret)
			return ret;
	}

	ret = kfifo_from_user( &device_buff, buf , count, &actual_write );
	if( ret ){
		return -EFAULT;
	}

	if(!kfifo_is_empty(&device_buff)){
		wake_up_interruptible(&device.read_queue);
	}


	printk("write %ld, actuall wrie %d\n", count, actual_write);

	return actual_write;
}

static const struct file_operations demodrv_fops = {
	.owner = THIS_MODULE,
	.open = demodrv_open,
	.release = demodrv_release,
	.read = demodrv_read,
	.write = demodrv_write
};

static struct miscdevice demo_miscdev = {
	.name = "my_misc_driver",
	.fops = &demodrv_fops,
	.minor = MISC_DYNAMIC_MINOR,
};



static int __init my_init(void){

	int ret;

	ret = misc_register(&demo_miscdev);
	if(ret){
		printk("demo_miscdev add fail");
		return ret;
	}
	demodrv_device = demo_miscdev.this_device;

	printk("%s\n", demo_miscdev.name);

	init_waitqueue_head(&device.read_queue);
	init_waitqueue_head(&device.write_queue);



	return 0;

}

void __exit my_exit(void){
	printk("my_exit\n");
	misc_deregister(&demo_miscdev);

}

module_init(my_init);
module_exit(my_exit);


MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("waterdog");
MODULE_DESCRIPTION("simple character device");
MODULE_ALIAS("mytest");
