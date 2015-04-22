#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <asm/io.h>
#include <linux/uaccess.h>
#include <linux/interrupt.h>
#include <linux/device.h>
#include <linux/irqreturn.h>
#include <linux/moduleparam.h>
#include <asm/siginfo.h>
#include <asm/signal.h>

#include "efm32gg.h"

#define DRIVER_NAME "gamepad"

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Platform driver module demo");
//MODULE_DEVICE_TABLE(of, my_of_match);


/*Base functions */
static int __init ourinitmodule(void);
static void __exit ourcleanupmodule(void);
static int gamepad_open(struct inode *inode, struct file *filp);
static int gamepad_release(struct inode *inode, struct file *filp);
static ssize_t gamepad_read(struct file *filp, char __user *buff, size_t count, loff_t *offp);
static irqreturn_t interrupt_handler(int irq, void *dev_id, struct pt_regs *regs);
static int gamepad_fasync(struct file *filp,int fd,int mode);

static struct file_operations my_fops = {
	.owner = THIS_MODULE,
	.read = gamepad_read,
	.open = gamepad_open,
	.release = gamepad_release,
	.fasync = gamepad_fasync,
};

struct cdev my_cdev;
struct class *cl;
dev_t devno = 1;
void __iomem *gpio;
struct fasync_struct* async_que;

static irqreturn_t interrupt_handler(int irq, void *dev_id, struct pt_regs * regs){

	iowrite32(0xff, GPIO_IFC);
	if(async_que){
		kill_fasync(SIGIO,&async_que,POLL_IN);
	}
	return IRQ_HANDLED;
}

static int gamepad_fasync(struct file *filp,int fd,int mode){
	return fasync_helper(fd,filp,mode,&async_que);
}

static int __init ourinitmodule(void){
	

	// Allocate memory for the device
	alloc_chrdev_region(&devno,0,1, DRIVER_NAME);
	
	// Memory setup
	check_mem_region(GPIO_BASE, GPIO_SIZE);
	request_mem_region(GPIO_BASE,GPIO_SIZE, DRIVER_NAME);
	
	// Dynamic Memory setup
	gpio = GPIO_BASE;
	ioremap_nocache(gpio, GPIO_SIZE);

	// GPIO setup
	iowrite32(0x33333333, GPIO_PC_MODEL);
	iowrite32(0xff, GPIO_PC_DOUT);
	iowrite32(0x22222222, GPIO_EXTIPSELL);



	// Interrupt setup
	iowrite32(0xff, GPIO_IEN);
	iowrite32(0xff, GPIO_IFC);
	iowrite32(0xFF, GPIO_EXTIFALL);
	request_irq(17, (irq_handler_t)interrupt_handler, 0, DRIVER_NAME, &my_cdev); //17 & 18 = GPIO EVEN & ODD
	request_irq(18, (irq_handler_t)interrupt_handler, 0, DRIVER_NAME, &my_cdev);

	
	// Actives the driver
	cdev_init(&my_cdev,&my_fops); 
	if(cdev_add(&my_cdev, devno, 1) < 0){
		printk(KERN_ERR "\n driver-gamepad: cdev_add failed \n");

	}
	my_cdev.owner = THIS_MODULE;
	cdev_add(&my_cdev, devno, 1);
	cl = class_create(THIS_MODULE, DRIVER_NAME);
	device_create(cl,NULL,devno,NULL, DRIVER_NAME);


	return 0;
}

static void __exit ourcleanupmodule(void){

	// Unregister Device
	device_destroy(cl, devno);
	class_destroy(cl);
	cdev_del(&my_cdev);

	// Release interrupt
	free_irq(17, NULL);
	free_irq(18, NULL);

	// Release GPIO
	iounmap(gpio);
	release_mem_region(GPIO_BASE, GPIO_SIZE);
	unregister_chrdev_region(&devno,1);

	return;
}

/*static int my_probe(struct platform_device *dev){
	//Empty probe function
}

static int my_remove(struct platform_device *dev){
	//Empty remove function
}*/


static ssize_t gamepad_read(struct file *filp, char __user *buff, size_t count, loff_t *offp){
	uint32_t input = ioread32(GPIO_PC_DIN);
	copy_to_user(buff, &input, 1);
	return 1;
}

static int gamepad_open(struct inode *inode, struct file *filp){
	return 0;
}

static int gamepad_release(struct inode *inode, struct file *filp){
	return 0;
}

module_init(ourinitmodule);
module_exit(ourcleanupmodule);
