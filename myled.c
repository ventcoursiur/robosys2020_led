#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/delay.h>
MODULE_AUTHOR("Souma Yamabe and Ryuichi Ueda");
MODULE_DESCRIPTION("driver for LED control");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.0.1");
static int pin_n[6] = {20,25,24,23,21,22};
static dev_t dev;
static struct cdev cdv;
static struct class *cls = NULL;
static volatile u32 *gpio_base = NULL;

static ssize_t led_write(struct file* filp, const char* buf, size_t count, loff_t* pos){
	char c;
	int i, beat, bt;

	if(copy_from_user(&c,buf,sizeof(char)))
		return -EFAULT;

	if(c == '0'){
		for(i = 0; i < 5; i++){
			gpio_base[10] = 1 << pin_n[i];
		}
	}else if(c == '9'){
		for(i = 0; i < 5; i++){
			gpio_base[7] = 1 << pin_n[i];
		}
	}else if(c >= '1' && c <= '8'){
		i = 0;
		for(i = 0; i < 5; i++){
			gpio_base[10] = 1 << pin_n[i];
		}
		for(i = 0; i < 5; i++){
			gpio_base[7] = 1 << pin_n[i];
			udelay((c-'0')*1000*1000);
			gpio_base[10] = 1 << pin_n[i];
		}
		for(i = 3; i > -1; i--){
			gpio_base[7] = 1 << pin_n[i];
			udelay((c-'0')*1000*1000);
			gpio_base[10] = 1 << pin_n[i];
		}
	}else if(c >= 'a' && c <= 'z'){
		i = 0;
		beat = 0;
		bt = 6*1000*1000/((c-'a'+1)*4);
		udelay(bt);
		for(beat = 0; beat < 20; beat++){
			for(i = 0; i < 5; i++){
				if(i == 0 || i == 4){
					gpio_base[7] = 1 << pin_n[5];
				}
				gpio_base[7] = 1 << pin_n[i];
				udelay(bt);
				if(i == 3){
					gpio_base[10] = 1 << pin_n[3];
					gpio_base[10] = 1 << pin_n[0];
				}else if(i == 1){
					gpio_base[10] = 1 << pin_n[1];
					gpio_base[10] = 1 << pin_n[4];
				}else if(i == 2){
					gpio_base[10] = 1 << pin_n[2];
				}else{
					gpio_base[10] = 1 << pin_n[5];
				}
			}
			for(i = 3; i > 0; i--){
				gpio_base[7] = 1 << pin_n[i];
				udelay(bt);
				if(i == 3){
					gpio_base[10] = 1 << pin_n[3];
					gpio_base[10] = 1 << pin_n[0];
				}else if(i == 1){
					gpio_base[10] = 1 << pin_n[1];
					gpio_base[10] = 1 << pin_n[4];
				}else if(i == 2){
					gpio_base[10] = 1 << pin_n[2];
				}
			}
		}
		gpio_base[7] = 1 << pin_n[0];
	}
	printk(KERN_INFO "receive %c\n",c);
	return 1;
}

static struct file_operations led_fops = {
	.owner = THIS_MODULE,
	.write = led_write,
};

static int __init init_mod(void){
	int retval;

	gpio_base = ioremap_nocache(0xfe200000, 0xA0);

	const u32 led[6] = {20,25,24,23,21,22};
	const u32 index[6] = {led[0]/10, led[1]/10, led[2]/10, led[3]/10, led[4]/10, led[5]};
	const u32 shift[6] = {(led[0]%10)*3, (led[1]%10)*3, (led[2]%10)*3, (led[3]%10)*3, (led[4]%10)*3, (led[5]%10)*3};
	const u32 mask = ~(0x7 << shift[0] | 0x7 << shift[1] | 0x7 << shift[2] | 0x7 << shift[3] | 0x7 << shift[4] | 0x7 << shift[5]);
	gpio_base[index[0]] = (gpio_base[index[0]] & mask) | (0x1 << shift[0]) | (0x1 << shift[1]) | (0x1 << shift[2]) | (0x1 << shift[3]) | (0x1 << shift[4]) | (0x1 << shift[5]);

	retval = alloc_chrdev_region(&dev, 0, 1, "myled");
	if(retval < 0){
		printk(KERN_ERR "alloc_chrdev_region failed.\n");
		return retval;
	}
	printk(KERN_INFO "%s is loaded. major:%d\n",__FILE__,MAJOR(dev));

	cdev_init(&cdv, &led_fops);
	retval = cdev_add(&cdv, dev, 1);
	if(retval < 0){
		printk(KERN_ERR "cdev_add failed. major:%d, minor:%d",MAJOR(dev),MINOR(dev));
		return retval;
	}
	cls = class_create(THIS_MODULE,"myled");
	if(IS_ERR(cls)){
		printk(KERN_ERR "class_create failed.");
		return PTR_ERR(cls);
	}
	device_create(cls, NULL, dev, NULL, "myled%d",MINOR(dev));
	return 0;
}

static void __exit cleanup_mod(void){
	cdev_del(&cdv);
	device_destroy(cls, dev);
	class_destroy(cls);
	unregister_chrdev_region(dev, 1);
	printk(KERN_INFO "%s is unloaded. major:%d\n",__FILE__,MAJOR(dev));
}

module_init(init_mod);
module_exit(cleanup_mod);
