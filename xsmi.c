
#include <linux/cdev.h>     
#include <linux/compiler.h> /* __must_check */
#include <linux/device.h>
#include <linux/err.h>
#include <linux/errno.h>    
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kdev_t.h>
#include <linux/kernel.h>   
#include <linux/module.h>   
#include <linux/uaccess.h>  

#include "xsmi.h"

#define MODULE_NAME "xsmi"

/***************************************************************************\
|* Static file-local variables
\***************************************************************************/
static const char testString[] 			= "Hello world from kernel mode!\n\0";
static const ssize_t testStringSize 	= sizeof(testString);

static int device_file_major_number 	= 0;
static const char device_name[] 		= MODULE_NAME;

// Automatic device file creation
static struct class *dev_class;
dev_t dev = 0;

// module parameter: Which pin to use as an interrupt
static int gpioPin = 27;
module_param(gpioPin, int, S_IRUGO);

/***************************************************************************\
|* Kernel module declarations
\***************************************************************************/
MODULE_DESCRIPTION("Extended SMI Linux driver");
MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("Moebius Tech, LLC");

#if 0
#  pragma mark - Kernel module registration
#endif

/***************************************************************************\
|* Kernel module registration: init the module
\***************************************************************************/
static int __init xsmi_init(void)
	{
	int result;
    printk( KERN_NOTICE MODULE_NAME ": Initialization started\n" );
    printk( KERN_NOTICE "    : Using GPIO ping %d for interrupts\n", gpioPin );

    result = register_device();
    return result;
	}

/***************************************************************************\
|* Kernel module registration: exit the module
\***************************************************************************/
static void __exit xsmi_exit(void)
	{
    printk( KERN_NOTICE "xsmi: Exiting\n" );
    unregister_device();
	}
   
module_init(xsmi_init);
module_exit(xsmi_exit);


#if 0
#  pragma mark - Device operations
#endif



/***************************************************************************\
|* Device op: read
\***************************************************************************/
static ssize_t device_file_read(struct file *file_ptr,
    							char __user *user_buffer,
    							size_t count,
    							loff_t *at)
	{
    printk( KERN_NOTICE "xsmi: Device file is read at offset = %i, "
    					"read bytes count = %u\n"
        				, (int)*at
        				, (unsigned int)count);

    if (*at >= testStringSize)
        return 0;

    if (*at + count > testStringSize)
        count = testStringSize - *at;

    if(copy_to_user(user_buffer, testString + *at, count) != 0)
        return -EFAULT;

    *at += count;
    return count;
	}

/***************************************************************************\
|* Device op: declare the operations that the module performs
|*
|*
|* Implement:
|*
|*  - poll  : returns whether buffers are available - one bit per buffer (fd)
|*  - read  : returns the contents of a DMA buffer, up to 1K * bufferSize (fd) 
|*  - write : accepts up to 1K * bufferSize bytes into a DMA buffer (fd)
|*  - open  : open creates a DMA buffer ready for read or write (as an fd)
|*  - ioctl	: configuration
|*  - close : shut down the DMA buffer (fd)
|* 
\***************************************************************************/
static struct file_operations xsmi_ops = 
	{
    .owner = THIS_MODULE,
    .read = device_file_read,
	};



#if 0
#  pragma mark - Device file 
#endif



/***************************************************************************\
|* Device file: register the device file with the system
\***************************************************************************/
int register_device(void)
	{
	int result = 0;

	
	/***********************************************************************\
	|* Allocating Major number
	\***********************************************************************/
	if ((alloc_chrdev_region(&dev, 0, 1, MODULE_NAME)) <0)
		{
		pr_err("Cannot allocate major number for device\n");
		return -1;
        }
    pr_info(MODULE_NAME ": Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));


 	/***********************************************************************\
	|* Creating struct class
	\***********************************************************************/
	dev_class = class_create(THIS_MODULE, MODULE_NAME "_class");
	if (IS_ERR(dev_class))
		{
    	pr_err("Cannot create the struct class for device\n");
    	result |= 1;
        }
	else
		{
        if(IS_ERR(device_create(dev_class,NULL,dev,NULL,MODULE_NAME)))
        	{
            pr_err("Cannot create the Device\n");
            result |= 2;
        	}
		}

	if (result & 2)
		class_destroy(dev_class);
	if (result & 1)
		unregister_chrdev_region(dev, 1);
	if (result == 0)
		pr_info("Kernel Module Inserted Successfully...\n");
	return -result;
	
	}

/***************************************************************************\
|* Device file: unregister the device file with the system
\***************************************************************************/
void unregister_device(void)
	{
	device_destroy(dev_class,dev);
	class_destroy(dev_class);
	unregister_chrdev_region(dev, 1);
	pr_info("Kernel Module Removed Successfully...\n");
	}
