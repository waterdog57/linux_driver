#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/kfifo.h>

static dev_t dev;
static struct cdev *demo_cdev;
static signed int count = 1;

struct mydemo_device{
	char name[64];
	struct device *dev;
	wait_queue_head_t read_queue;
	wait_queue_head_t write_queue;
	struct kfifo mydemo_kfifo;
};

struct mydemo_private_data{
	struct mydemo_device *device;
	char name[64];
};
#define MAX_CDEV_NR    ( 8 )
#define MAX_KFIFO_SIZE (64)
#define DEMO_NAME    "my_demo_dev"

static struct mydemo_device *mydemo_device[MAX_CDEV_NR];



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

	int ret, i;
	struct mydemo_device *device;
	

	ret = alloc_chrdev_region( &dev, 0, MAX_CDEV_NR, DEMO_NAME);
	if( ret ){
		printk("cdev MAJOR number allocate fail");
		return ret;
	}

	demo_cdev = cdev_alloc();
	if(!demo_cdev){
		printk("cdev alloc fail");
		goto unregister_chrdev;
	}

	cdev_init( demo_cdev, &demodrv_fops );

	ret = cdev_add( demo_cdev, dev, MAX_CDEV_NR);
	if(ret){
		printk("cdev add fail");
		goto cdev_fail;
	}
	printk("major = %d, mirno = %d\n",MAJOR(dev),MINOR(dev));

	for(i = 0; i < MAX_CDEV_NR; i++){
		device = kmalloc( sizeof(struct mydemo_device ), GFP_KERNEL );
		if(!device){
			ret = -ENOMEM;
			goto free_device;
		}
		sprintf(device->name, "%s%d", DEMO_NAME, i );
		mydemo_device[i] = device;
		init_waitqueue_head( &device->read_queue );
		init_waitqueue_head( &device->write_queue );

		ret = kfifo_alloc( &device->mydemo_kfifo, MAX_KFIFO_SIZE, GFP_KERNEL );
		if (ret)
			goto free_kfifo;

	}

	return 0;

free_kfifo:
	for(i = 0; i < MAX_CDEV_NR; i++){
		if( device->mydemo_kfifo.kfifo.data )
			kfifo_free( &device->mydemo_kfifo );
	}
free_device:
	for(i = 0; i < MAX_CDEV_NR; i++){
		if( mydemo_device[i] )
			kfree(mydemo_device[i]);
	}
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


MODULE_LICENSE("GPL");
MODULE_AUTHOR("waterdog");
MODULE_DESCRIPTION("simple character device");
MODULE_VERSION("1.0");
MODULE_ALIAS("mytest");
