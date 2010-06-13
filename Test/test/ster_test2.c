//Autor: Aleksy Barcz
/* Zbiory naglowkowe dla urzadzen znakowych */
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/cdev.h>
//#include <linux/mm.h>
//#include <asm/io.h>

#include <linux/module.h>
#include <linux/kernel.h> 		/* printk() */
#include <asm/uaccess.h> 		/* copy_from/to_user */
#include "ster_test2.h"

MODULE_LICENSE("GPL v2");

dev_t my_dev=0;
struct cdev * my_cdev = NULL;
/* Uwaga!!! opis dotycz±cy korzystania z "class_simple" w LDD3 jest nieaktualny!!! */
/* Musia³em przerobiæ kod opieraj±c siê na implementacji w dvbdev.c */
static struct class *class_my_vga = NULL;

/* Declaration of memory.c functions */
int memory_open(struct inode *inode, struct file *filp);
int memory_release(struct inode *inode, struct file *filp);
ssize_t memory_read(struct file *filp, char *buf, size_t count, loff_t *f_pos);
ssize_t memory_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);
int memory_ioctl (struct inode *, struct file *, unsigned int, unsigned long);

void memory_exit(void);
int memory_init(void);

/* Structure that declares the usual file */
/* access functions */
struct file_operations memory_fops = {
  .owner = THIS_MODULE,
  .read=memory_read,
  .write=memory_write,
  .open=memory_open,
  .release=memory_release,
  .ioctl=memory_ioctl
};

/* Declaration of the init and exit functions */
module_init(memory_init);
module_exit(memory_exit);

/* Buffer to store data */
char *memory_buffer;

/* Module initialization */
int memory_init( void )
{
  int res;
  /* Tworzymy klasê opisuj±c± nasze urz±dzenie - aby móc wspó³pracowaæ z systemem udev */
  class_my_vga = class_create(THIS_MODULE, "ster_test2");
  if (IS_ERR(class_my_vga)) {
    printk(KERN_ERR "Error creating rs_class.\n");
    res=PTR_ERR(class_my_vga);
    goto err1;
  }
  /* Alokujemy numer urz±dzenia */
  res=alloc_chrdev_region(&my_dev, 0, 1, DEVICE_NAME);
  if(res) {
    printk ("<1>Alocation of the device number for %s failed\n",
            DEVICE_NAME);
    goto err1; 
  };
  /* Alokujemy strukturê opisuj±c± urz±dzenie znakowe */
  my_cdev = cdev_alloc( );
  my_cdev->ops = &memory_fops;
  my_cdev->owner = THIS_MODULE;
  /* Dodajemy urz±dzenie znakowe do systemu */
  res=cdev_add(my_cdev, my_dev, 1);
  if(res) {
    printk ("<1>Registration of the device number for %s failed\n",
            DEVICE_NAME);
    goto err1;
  };
  device_create(class_my_vga,NULL,my_dev,NULL,"ster_test2%d",MINOR(my_dev));
  
  /* Allocating memory for the buffer */
  memory_buffer = kmalloc(BUFFER_SIZE, GFP_KERNEL); 
  if (!memory_buffer) { 
    res = -ENOMEM;
    goto err1; 
  } 
  memset(memory_buffer, 0, BUFFER_SIZE);

  printk("<1>Ster_test2 driver initialized. The major device number is %d\n",
         MAJOR(my_dev)); 
  return 0;

 err1:
  /* Zwalniamy numer urz±dzenia */
  memory_exit();
  return res;
}

/* Usuwanie modu³u - sprz±tamy po sobie */
void memory_exit( void )
{
  /* Usuwamy urz±dzenie z klasy */
  if(my_dev && class_my_vga) {
    device_destroy(class_my_vga,my_dev);
  }
  /* Odrejestrowujemy urz±dzenie */
  if(my_cdev) cdev_del(my_cdev);
  my_cdev=NULL;
  /* Zwalniamy numer urz±dzenia */
  unregister_chrdev_region(my_dev, 1);
  /* Wyrejestrowujemy klasê */
  if(class_my_vga) {
    class_destroy(class_my_vga);
    class_my_vga=NULL;
  }
  /* Zwolnienie bufora danych */
  if (memory_buffer) {
    kfree(memory_buffer);
  }
  printk("<1>Removed ster_test2 module\n");

}

int memory_open(struct inode *inode, struct file *filp) {
	memory_buffer[0] = 9;
	memory_buffer[1] = 8;
	printk("<1>Ster: opened device\n");
  return 0;
}

int memory_release(struct inode *inode, struct file *filp) {
	printk("<1>Ster: closed device\n");
  return 0;
}

ssize_t memory_read(struct file *filp, char *buf, 
                    size_t count, loff_t *f_pos) { 
 
  /* Transfering data to user space */ 
  copy_to_user(buf,memory_buffer,1);

  /* Changing reading position as best suits */ 
  if (*f_pos == 0) { 
    *f_pos+=1; 
    return 1; 
  } else { 
    return 0; 
  }
}

int memory_ioctl (struct inode *inode, struct file *filp, 
					unsigned int code, unsigned long param) {
   int buffer_index = 0;
	printk("<1>Ster: entered ioctl\n");
	switch (code) {
      case IOCTL_ZERO_BYTE:      //odczyt pierwszego bajtu
         copy_to_user((char *)param,memory_buffer,1);
         break;

      case IOCTL_FIRST_BYTE:     //odczyt drugiego bajtu
         copy_to_user((char *)param,&(memory_buffer[1]),1);
         break;
         
      case IOCTL_READ_NTH_BYTE:  //odczyt ntego bajtu (param[0] = indeks)
         buffer_index = (int)(((char*)param)[0]);
         printk("<1>Ster: got index: %d in ioctl read\n", buffer_index);
         if((buffer_index < 0) || (buffer_index >= BUFFER_SIZE)) {
            printk("<1>Ster: ioctl nth byte out of range\n");
            return 0;
         }
         copy_to_user((char *)param,&(memory_buffer[buffer_index]),1);
         break;
      
      case IOCTL_WRITE_NTH_BYTE: //zapis ntego bajtu (p[0]=indeks, p[1]=wartosc)
         buffer_index = (int)(((char*)param)[0]);
         printk("<1>Ster: got index: %d in ioctl write\n", buffer_index);
         printk("<1>Ster: got value: %d in ioctl write\n", ((char*)param)[1]);
         if((buffer_index < 0) || (buffer_index >= BUFFER_SIZE)) {
            printk("<1>Ster: ioctl nth byte out of range\n");
            return 0;
         }
         copy_from_user(&(memory_buffer[buffer_index]),((char*)param + 1),1);
         break;
         
      default:
         printk("<1>Ster: unknown ioctl\n");
         return 0;
	}
	printk("<1>Ster: finished ioctl\n");
	return 1;
}


ssize_t memory_write( struct file *filp, const char *buf,
                      size_t count, loff_t *f_pos) {
  //char *tmp;
  //tmp=buf+count-1;
  //copy_from_user(memory_buffer,tmp,1);
  copy_from_user(memory_buffer,buf,1);
  return 1;
}

