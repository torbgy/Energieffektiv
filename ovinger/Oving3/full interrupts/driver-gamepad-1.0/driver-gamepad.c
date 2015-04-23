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
// Signals
#include <asm/siginfo.h> // siginfo
#include <asm/signal.h>	
#include <linux/sched.h> // find_task_by_pid_type 
#include <linux/rcupdate.h> // rcu_read_lock
#include <linux/pid.h> // PID-ns

#include "efm32gg.h"

#define DRIVER_NAME "gamepad"
#define SIG_NUM 50 //Signal number



/*Base functions */
static int __init ourinitmodule(void);
static void __exit ourcleanupmodule(void);
static int gamepad_open(struct inode*, struct file*);
static int gamepad_release(struct inode*, struct file*);
static irqreturn_t interrupt_handler(int, void*, struct pt_regs*);
static ssize_t gamepad_write(struct file*,const char __user*,size_t,loff_t*);

static struct file_operations my_fops = {
	.owner = THIS_MODULE,
	.write = gamepad_write,
	.open = gamepad_open,
	.release = gamepad_release	
};

struct cdev my_cdev;
struct class *cl;
dev_t devno = 1;
void __iomem *gpio;

// Signals
int game_pid = 0;
struct siginfo info;
struct task_struct *t; 

static int __init ourinitmodule(void){
	

	// Allocate memory for the device
	alloc_chrdev_region(&devno,0,1, DRIVER_NAME);
	
	// Memory setup
	check_mem_region(GPIO_BASE, GPIO_SIZE);
	request_mem_region(GPIO_BASE,GPIO_SIZE, DRIVER_NAME);
	
	// GPIO setup
	iowrite32(0x33333333, GPIO_PC_MODEL);
	iowrite32(0xff, GPIO_PC_DOUT);
	iowrite32(0x22222222, GPIO_EXTIPSELL);

	// Interrupt setup
	iowrite32(0xff, GPIO_IEN);
	iowrite32(0xff, GPIO_IFC);
	iowrite32(0xFF, GPIO_EXTIFALL);
	request_irq(17, (irq_handler_t)interrupt_handler, 0, DRIVER_NAME, NULL); //17 & 18 = GPIO EVEN & ODD
	request_irq(18, (irq_handler_t)interrupt_handler, 0, DRIVER_NAME, NULL);


	// Signals setup
	memset(&info,0,sizeof(struct siginfo));
	info.si_signo = SIG_NUM;
	info.si_code = SI_QUEUE;


	// Actives the driver
	cl = class_create(THIS_MODULE, DRIVER_NAME);
	device_create(cl,NULL,devno,NULL, DRIVER_NAME);

	cdev_init(&my_cdev,&my_fops);
	my_cdev.owner = THIS_MODULE;
	cdev_add(&my_cdev, devno, 1);



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
	release_mem_region(GPIO_BASE, GPIO_SIZE);
	unregister_chrdev_region(&devno,1);

	return;
}


static irqreturn_t interrupt_handler(int irq, void *dev_id, struct pt_regs * regs){
 
	info.si_int = ioread32(GPIO_PC_DIN);
	send_sig_info(SIG_NUM,&info,t); 
	iowrite32(0xff, GPIO_IFC);

	return IRQ_HANDLED;
}

static ssize_t gamepad_write(struct file *filp,const char __user *buf,size_t count,loff_t *ppos){
	char mybuf[10];
	copy_from_user(mybuf,buf,count);
	sscanf(mybuf,"%d",&game_pid);
	rcu_read_lock();
	t = pid_task(find_pid_ns(game_pid, &init_pid_ns),PIDTYPE_PID);
	if(t == NULL){
		printk("Error in gamepad_write\n");
		rcu_read_unlock();
		iowrite32(0xff, GPIO_IFC);
		return -1;
	}
	rcu_read_unlock();
	


	return 0;
}

static int gamepad_open(struct inode *inode, struct file *filp){
	return 0;
}

static int gamepad_release(struct inode *inode, struct file *filp){
	return 0;
}

module_init(ourinitmodule);
module_exit(ourcleanupmodule);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Gamepad Driver");