#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h> 
//jesli nie widzisz wynikow dzialania fcji printk(),
// uzyj "dmesg" w linii polecen

//MODULE_LICENSE("Dual BSD/GPL");

static int ster_init(void) {
   printk("<1> Hello world from kernel!\n"); //<1> : priorytet print'a
   return 0;
}

static void ster_exit(void) {
   printk("<1> Bye from kernel\n");
}

module_init(ster_init); //zostanie wywolane przez insmod
module_exit(ster_exit); //zostanie wywolane przez rmmod

