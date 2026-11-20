#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/err.h>

#define CLASS_NAME  "my_class"
#define DEVICE_NAME "my_dev"
#define BASE_MINOR  0
#define MINOR_COUNT 1
#define BUF_LEN     256

static dev_t dev_num;
static struct cdev my_cdev;
static struct class *my_class;
static struct device *my_device;

static char kbuf[BUF_LEN];
static size_t kbuf_len = 0;

static int dev_open(struct inode *inode, struct file *file)
{
    pr_info("step5: %s opened\n", DEVICE_NAME);
    return 0;
}

static int dev_release(struct inode *inode, struct file *file)
{
    pr_info("step5: %s closed\n", DEVICE_NAME);
    return 0;
}

static ssize_t dev_read(struct file *file, char __user *user_buf, size_t count, loff_t *ppos)
{
    size_t to_copy;

    // Return 0 (EOF) once we reached the end of stored kernel data
    if (*ppos >= kbuf_len) {
        return 0;
    }

    to_copy = min(count, (size_t)(kbuf_len - *ppos));

    // Copy data safely from kernel space to user space
    if (copy_to_user(user_buf, kbuf + *ppos, to_copy) != 0) {
        return -EFAULT;
    }

    *ppos += to_copy;
    return (ssize_t)to_copy;
}

static ssize_t dev_write(struct file *file, const char __user *user_buf, size_t count, loff_t *ppos)
{
    size_t to_copy;

    if (count == 0) {
        return 0;
    }

    if (count >= BUF_LEN) {
        to_copy = BUF_LEN - 1;
    } else {
        to_copy = count;
    }

    // Copy data safely from user space to kernel space
    if (copy_from_user(kbuf, user_buf, to_copy) != 0) {
        return -EFAULT;
    }

    kbuf[to_copy] = '\0';
    kbuf_len = to_copy;
    *ppos = 0;

    pr_info("step5: wrote %zu bytes to %s\n", to_copy, DEVICE_NAME);
    return (ssize_t)to_copy;
}

static const struct file_operations fops = {
    .owner   = THIS_MODULE,
    .open    = dev_open,
    .release = dev_release,
    .read    = dev_read,
    .write   = dev_write,
};

static int __init step5_init(void)
{
    int ret;

    ret = alloc_chrdev_region(&dev_num, BASE_MINOR, MINOR_COUNT, DEVICE_NAME);
    if (ret < 0) {
        pr_err("step5: alloc_chrdev_region failed (%d)\n", ret);
        return ret;
    }

    cdev_init(&my_cdev, &fops);
    my_cdev.owner = THIS_MODULE;
    ret = cdev_add(&my_cdev, dev_num, MINOR_COUNT);
    if (ret < 0) {
        pr_err("step5: cdev_add failed (%d)\n", ret);
        goto err_cdev;
    }

    my_class = class_create(CLASS_NAME);
    if (IS_ERR(my_class)) {
        ret = PTR_ERR(my_class);
        pr_err("step5: class_create failed (%d)\n", ret);
        goto err_class;
    }

    my_device = device_create(my_class, NULL, dev_num, NULL, DEVICE_NAME);
    if (IS_ERR(my_device)) {
        ret = PTR_ERR(my_device);
        pr_err("step5: device_create failed (%d)\n", ret);
        goto err_device;
    }

    pr_info("step5: ready on /dev/%s (major=%d, minor=%d)\n",
            DEVICE_NAME, MAJOR(dev_num), MINOR(dev_num));
    return 0;

err_device:
    class_destroy(my_class);
err_class:
    cdev_del(&my_cdev);
err_cdev:
    unregister_chrdev_region(dev_num, MINOR_COUNT);
    return ret;
}

static void __exit step5_exit(void)
{
    device_destroy(my_class, dev_num);
    class_destroy(my_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev_num, MINOR_COUNT);
    pr_info("step5: unloaded\n");
}

module_init(step5_init);
module_exit(step5_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Student");
MODULE_DESCRIPTION("Step 5: read and write with copy_to_user and copy_from_user");