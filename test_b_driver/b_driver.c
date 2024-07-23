#include <linux/init.h>
#include <linux/module.h>

static int test_item = 0;
module_param( test_item, int, 0644 );
MODULE_PARM_DESC(test_item,"which test item?");

int testfun_for_anthor_driver(void){
	printk("%s\n","testfun_for_anthor_driver");
	return 0;
}


static int __init my_init(void){
	printk("my init\n");
	printk("test item : %d\n", test_item);
	testfun_for_anthor_driver();
	return 0;

}

void __exit my_exit(void){
	printk("my_exit\n");
}

module_init(my_init);
module_exit(my_exit);



MODULE_LICENSE("GPL");
MODULE_AUTHOR("waterdog");
MODULE_DESCRIPTION("TEST b");
MODULE_ALIAS("mytest");
