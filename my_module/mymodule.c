#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple Raspberry Pi Linux kernel module");
MODULE_VERSION("1.0");

static int __init hello_init(void) {
    // This runs when the module is loaded into the kernel
    printk(KERN_INFO "Hello, Raspberry Pi!\n");
    return 0;
}

static void __exit hello_exit(void) {
    // This runs when the module is removed from the kernel
    printk(KERN_INFO "Goodbye, Raspberry Pi!\n");
}

module_init(hello_init);
module_exit(hello_exit);