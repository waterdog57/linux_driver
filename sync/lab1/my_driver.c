#include <linux/init.h>
#include <linux/module.h>
#include <linux/spinlock_types.h>



int testfun_for_anthor_driver(void){
	printk("%s\n","testfun_for_anthor_driver");
	return 0;
}


static int __init my_init(void){
	printk("my init\n");

	return 0;

}

void __exit my_exit(void){
	printk("my_exit\n");
}

module_init(my_init);
module_exit(my_exit);

EXPORT_SYMBOL(testfun_for_anthor_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("waterdog");
MODULE_DESCRIPTION("TEST DRIVER");
MODULE_ALIAS("mytest");
