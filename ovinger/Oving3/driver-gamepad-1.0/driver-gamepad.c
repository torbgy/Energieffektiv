
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/ioport.h>



#include "efm32gg.h"
static ssize_t my_read(struct file *filp, char __user *buff, size_t count, loff_t *offp);
static int my_open(struct inode *inode, struct file *filp);
static int my_release(struct inode *inode, struct file *filp);
static ssize_t my_write(struct file *filp, const char __user *buff, size_t count, loff_t *offp);
static int __init driver_init(void);
static void __exit driver_cleanup(void);

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = my_read,
	.write = my_write,
	.open = my_open,
	.release = my_release
};

dev_t devno;
struct class *cl;
struct cdev my_cdev;


static int __init driver_init(void){
	printk("driver-gamepad: Initializing\n");

// Character Device
	if (alloc_chrdev_region(&devno,0,1, "gamepad") < 0){
		printk(KERN_ERR "driver-gamepad: alloc_chrdev failed\n");
		return -1;
	}; 
	cl = class_create(THIS_MODULE, "gamepad");
	device_create(cl,NULL,devno,NULL, "gamepad");
	
	
// Memory allocation & Virtual memory
	if(request_mem_region(GPIO_PC_BASE, 0x24 ,"gamepad") < 0){
		printk(KERN_ERR "driver-gamepad: request_mem failed\n");
		return -1;
	}; 
	virt_mem = ioremap_nocache(GPIO_PC_BASE, 0x24); 
	
// Write to GPIO registers
	iowrite32(0x33333333, virt_mem + GPIO_PC_MODEL);
	iowrite32(0xff, virt_mem + GPIO_PC_DOUT);

	
	
	/*
	int request_irq(unsigned int irq,irqreturn_t (*handler)(int, void *, struct pt_regs *), unsigned long flags, const char *dev_name, void *dev_id); // Interrupts?
	*/

// Activation of the driver
	cdev_init(&my_cdev,&fops); 
	cdev_add(&my_cdev, devno,1);
	return 0;
}

static ssize_t my_read(struct file *filp, char __user *buff, size_t count, loff_t *offp){
	uint32_t data = ioread32(GPIO_PC_DIN);
	//printk("driver-gamepad: button pressed, sending data to user space");
	copy_to_user(buff,data,	1);
	return 0;
}

//Not needed, yet
static int my_open(struct inode *inode, struct file *filp){return 0;}
static int my_release(struct inode *inode, struct file *filp){return 0;}
static ssize_t my_write(struct file *filp, const char __user *buff, ize_t count, loff_t *offp){return 0;}

static void __exit driver_cleanup(void){
	printk("driver-gamepad: Terminating\n");
	void release_region(unsigned long start, unsigned long n);

}




module_init(driver_init);
module_exit(driver_cleanup);

MODULE_DESCRIPTION("Small module, demo only, not very useful.");
MODULE_LICENSE("GPL");

