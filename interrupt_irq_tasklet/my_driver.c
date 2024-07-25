#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/miscdevice.h>

#define MAX_USER_BUF_SIZE (64)
static char device_buff[MAX_USER_BUF_SIZE];

static struct device *demodrv_device;
static struct tasklet_struct mytasklet;
unsigned int irqnum = 19;
void *irq_dev_id = (void *)&irqnum;


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
demodrv_read(struct file *file, char __user *buf, size_t lbuf, loff_t *ppos)
{
	loff_t offset;
	int max_free;
	int need_read;
	int ret;

	if (ppos == NULL) {
        offset = file->f_pos;
    } else {
        offset = *ppos;
    }
	printk("ppos=%lld", *ppos);
	max_free = MAX_USER_BUF_SIZE - *ppos;
	need_read = max_free > lbuf ? lbuf : max_free;
	if(need_read == 0){
		dev_warn(demodrv_device,"no space for read");
	}

	ret = copy_to_user( buf, device_buff + *ppos, need_read );
	if(ret == need_read){
		return -EFAULT;
	}

	*ppos += need_read;
	printk("read %d\n", need_read);

	return need_read;
}
static ssize_t 
demodrv_write(struct file *file,const char __user *buf, size_t count, loff_t *f_pos)
{
	loff_t offset;
	int max_free;
	int need_write;
	int ret;

	if (f_pos == NULL) {
        offset = file->f_pos;
    } else {
        offset = *f_pos;
    }
	printk("fpos=%lld", *f_pos);
	max_free = MAX_USER_BUF_SIZE - *f_pos;
	need_write = max_free > count ? count : max_free;
	if(need_write == 0){
		dev_warn(demodrv_device,"no space for write");
	}

	ret = copy_from_user( device_buff + *f_pos, buf , need_write );
	if(ret == need_write){
		return -EFAULT;
	}

	*f_pos += need_write;
	printk("write %d\n", need_write);






	return need_write;
}

static void myprint( unsigned long data ){
	// for(int i =0;i<count;i++){
	// 	printk("%")
	// }
	printk("13");

}

irqreturn_t sample_isr( int irq, void *dev_instance ){

    printk("%s: irq %d dev_instance %p\n",__func__,  irq, dev_instance );

    tasklet_schedule( &mytasklet );

    return IRQ_NONE;
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

	tasklet_init( &mytasklet, myprint, 0);

	ret = request_irq( irqnum, sample_isr, IRQF_SHARED, "SAMPLE", irq_dev_id );
	if(ret){
		printk("ret %d\n", ret);
		goto error;
	}
	return 0;

error:
	misc_deregister(&demo_miscdev);
	tasklet_kill(&mytasklet);
	return -1;
}

void __exit my_exit(void){
	printk("my_exit\n");
	misc_deregister(&demo_miscdev);
	tasklet_kill(&mytasklet);
	free_irq( irqnum, irq_dev_id );

}

module_init(my_init);
module_exit(my_exit);


MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("waterdog");
MODULE_DESCRIPTION("simple character device");
MODULE_ALIAS("mytest");
