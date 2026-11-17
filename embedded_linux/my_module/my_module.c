#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple Raspberry Pi Linux kernel module");
MODULE_VERSION("1.0");

dev_t my_device_num;

static int __init hello_init(void) {
    int ret;

    ret = alloc_chrdev_region(&my_device_num, 0, 1, "my_custom_device");

    if (ret < 0) {
        printk(KERN_ALERT "Failed to allocate device number\n");
        return ret;
    }

    printk(KERN_INFO "Successfully allocated Major: %d, Minor: %d\n", 
           MAJOR(my_device_num), MINOR(my_device_num));

    
    return 0;
}

static void __exit hello_exit(void) {
    unregister_chrdev_region(my_device_num, 1);

    printk(KERN_INFO "Device number freed and module unloaded\n");

}

module_init(hello_init);
module_exit(hello_exit);