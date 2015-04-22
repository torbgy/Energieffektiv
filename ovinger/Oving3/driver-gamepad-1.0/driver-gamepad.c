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
//For platform drivers....
//#include <linux/platform_device.h>
#include "efm32gg.h"

#define DRIVER_NAME "gamepad"

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Platform driver module demo");
//MODULE_DEVICE_TABLE(of, my_of_match);

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

/*Base functions */
static int __init ourinitmodule(void);
static void __exit ourcleanupmodule(void);
//static int my_probe(struct platform_device *dev);
//static int my_remove(struct platform_device *dev);
static int my_open(struct inode *inode, struct file *filp);
static int my_release(struct inode *inode, struct file *filp);
static ssize_t my_read(struct file *filp, char __user *buff, size_t count, loff_t *offp);
static irqreturn_t interrupt_handler(int irq, void *dev_id, struct pt_regs *regs);


static struct file_operations my_fops = {
	.owner = THIS_MODULE,
	.read = my_read,
	.open = my_open,
	.release = my_release
};

struct cdev my_cdev;
struct class *cl;
dev_t devno = 1;

void __iomem *gpio;
uint8_t button;

//******NOTE*****
//ARM = mmap all func support, ports unsigned int
static int __init ourinitmodule(void){
	printk(KERN_INFO "\n Welcome to sample Platform driver... \n");

	// Allocate memory for the device
	alloc_chrdev_region(&devno,0,1, DRIVER_NAME);
	
	// Memory setup
	check_mem_region(GPIO_BASE, GPIO_SIZE);
	request_mem_region(GPIO_BASE,GPIO_SIZE, DRIVER_NAME);
	gpio = GPIO_BASE;
	// Dynamic Memory setup
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
	//platform_driver_register(&my_driver);
	cl = class_create(THIS_MODULE, DRIVER_NAME);
	device_create(cl,NULL,devno,NULL, DRIVER_NAME);


	return 0;
}

static void __exit ourcleanupmodule(void){
	printk(KERN_INFO "\n Thanks... Exiting sample Platform driver... \n");
	/* Unregistering from Kernel */
	//platform_driver_unregister(&my_driver);
	//Clean drivers
	device_destroy(cl, devno);
	class_destroy(cl);

	cdev_del(&my_cdev);

	//Release interrupt
	free_irq(17, NULL);
	free_irq(18, NULL);

	//GPIO
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


static ssize_t my_read(struct file *filp, char __user *buff, size_t count, loff_t *offp){
	copy_to_user(buff, &button, 1);
	return 1;
}




static irqreturn_t interrupt_handler(int irq, void *dev_id, struct pt_regs * regs){

	uint32_t input;
	iowrite32(0xff, GPIO_IFC);
	input = ioread32(GPIO_PC_DIN) | 0xff00;
	
	switch(input){
		case (0xfffe) :
			button = 1;
			break;
		case (0xfffd) :
			button = 4;
			break;
		case (0xfffb) :
			button = 3;
			break;
		case (0xfff7) :
			button = 2;
			break;
		case (0xffef) :
			button = 5;
			break;
		case (0xffdf) :
			button = 6;
			break;
		case (0xffbf) :
			button = 7;
			break;
		case (0xff7f) :
			button = 8;
			break;
	}


	return IRQ_HANDLED;
}


static int my_open(struct inode *inode, struct file *filp){
	return 0;
}

static int my_release(struct inode *inode, struct file *filp){
	return 0;
}

/*static struct platform_driver my_driver = { //my driver decleared not used
	.probe = my_probe,
	.remove = my_remove,
	.driver = {
		.name = my, //undecleared
		.owner = THIS_MODULE,
		.of_match_table = my_of_match, //undecleared
	},
};

static const struct of_device_id my_of_match[] = {
	{
		.compatible = "tdt4258",
	},
	{},
};*/

module_init(ourinitmodule);
module_exit(ourcleanupmodule);

/* Specifying my resources information */
//struct resource *res = platform_get_resource(dev, IORESOURCE_MEM, index); //dev, index undecleared

//int irq = platform_get_irq(dev, index); //