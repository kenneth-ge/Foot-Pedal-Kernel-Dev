#include <linux/module.h>
#include <linux/init.h>

#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/input.h>

#include <asm/irq.h>
#include <asm/io.h>

// max Minor devices
#define MAX_DEV 1

static int demo_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "MYCHARDEV: Device open\n");
    return 0;
}

static int demo_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "MYCHARDEV: Device close\n");
    return 0;
}

static long demo_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    printk(KERN_INFO "MYCHARDEV: Device ioctl\n");
    return 0;
}

static ssize_t demo_read(struct file *file, char __user *buf, size_t count, loff_t *offset)
{
    printk(KERN_INFO "MYCHARDEV: Device read\n");
    return 0;
}

static ssize_t demo_write(struct file *file, const char __user *buf, size_t count, loff_t *offset)
{
    printk(KERN_INFO "MYCHARDEV: Device write\n");
    return 0;
}

// initialize file_operations
static const struct file_operations demo_fops = {
    .owner      = THIS_MODULE,
    .open       = demo_open,
    .release    = demo_release,
    .unlocked_ioctl = demo_ioctl,
    .read       = demo_read,
    .write       = demo_write
};

// device data holder, this structure may be extended to hold additional data
struct demo_dev_data {
    struct cdev cdev;
};

//global storage for device
static dev_t dev;

// global storage for device Major number
static int dev_major = 0;

// sysfs class structure
static struct class *demo_class = NULL;

// array of mychar_device_data for
static struct demo_dev_data demo_data[MAX_DEV];

int __init demo_init(void)
{
    int err, i;

    // allocate chardev region and assign Major number
    err = alloc_chrdev_region(&dev, 0, MAX_DEV, "demo");

    dev_major = MAJOR(dev);

    // create sysfs class
    demo_class = class_create(THIS_MODULE, "demo");

    // Create necessary number of the devices
    for (i = 0; i < MAX_DEV; i++) {
        // init new device
        cdev_init(&demo_data[i].cdev, &demo_fops);
        demo_data[i].cdev.owner = THIS_MODULE;

        // add device to the system where "i" is a Minor number of the new device
        cdev_add(&demo_data[i].cdev, MKDEV(dev_major, i), 1);

        // create device node /dev/demo-x where "x" is "i", equal to the Minor number
        device_create(demo_class, NULL, MKDEV(dev_major, i), NULL, "demo-%d", i);
    }

    return 0;
}

static void __exit demo_exit(void)
{
    int i;

    for (i = 0; i < MAX_DEV; i++) {
        cdev_del(&demo_data[i].cdev);
        device_destroy(demo_class, MKDEV(dev_major, i));
    }

    class_unregister(demo_class);
    class_destroy(demo_class);

    unregister_chrdev_region(dev, MAX_DEV);
}

module_init(demo_init);
module_exit(demo_exit);

#define DRIVER_AUTHOR "Kenneth Ge <kge@redhat.com>"
#define DRIVER_DESC   "A sample driver"

/* 
 * Get rid of taint message by declaring code as GPL. 
 */
MODULE_LICENSE("GPL");

/*
 * Or with defines, like this:
 */
MODULE_AUTHOR(DRIVER_AUTHOR);	/* Who wrote this module? */
MODULE_DESCRIPTION(DRIVER_DESC);	/* What does this module do */

/*  
 *  This module uses /dev/testdevice.  The MODULE_SUPPORTED_DEVICE macro might
 *  be used in the future to help automatic configuration of modules, but is 
 *  currently unused other than for documentation purposes.
 */
MODULE_SUPPORTED_DEVICE("demoinputdevice");