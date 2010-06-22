/*
 * USB AVR Noise Generator Driver - 0.1
 *
 * Copyright (C) 2010 Maciej "roobyck" Rubikowski (roobyck@gmail.com)
 *
 * Dzia³anie sterownika opiera siê o demona rngd. Sterownik tworzy nowe 
 * urz±dzenie znakowe, które zapewnia "przyzwoity" poziom losowo¶ci 
 * generowanych przezeñ liczb. W idealnym przypadku rngtest powinien
 * pokazaæ 100% sukcesów. Po za³adowaniu sterownika poleceniem:
 *
 * insmod noise-gen.ko
 *
 * mo¿na zacz±æ testowaæ generowane losowe bajty przy u¿yciu polecenia:
 * 
 * rngtest < /dev/noise-gen0
 *
 * 
 * Opcjonalnie: mo¿na ³atwo zamieniæ istniej±cy modu³ w w±tek automatycznie
 * powiêkszaj±cy systemowe zasoby entropii. W tym celu nale¿y odkomentowaæ 
 * fragmenty kodu oznaczone XXX.
 *
 *
 * FIXME: fragmenty obs³uguj±ce ten jeden jedyny endpoint typu interrupt s± 
 *	  na dobr± sprawê niepotrzebne (bo i tak korzystamy tylko z endpoint 0) 
 *
 *
 * ===============================================================
 * LICENSE:
 *
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, version 2.
 * ================================================================
 *
 */

#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/kref.h>
#include <asm/uaccess.h>
#include <linux/usb.h>
#include <linux/mutex.h>
#include <linux/cryptohash.h>
#include <linux/random.h>
#include <linux/kthread.h>
#include "noise-gen.h"


MODULE_LICENSE("GPL"); 
MODULE_DEVICE_TABLE (usb, noise_gen_table);

char *big_buffer;
size_t offset;
struct usb_ctrlrequest *cmd;
struct urb *urb1 = NULL;		/* do transmisji control */


/* {{{ XXX: obs³uga w±tku j±dra 
 * 
struct task_struct *filling_thread;

int fill_func ( void *data ) {
    struct usb_noise* dev;
    int result, i, j;
    __u32 *digest;
    __u32 *workspace;
    char *out_buffer;
    
    digest = kmalloc (5 * sizeof (__u32), GFP_KERNEL);
    workspace = kmalloc (80 * sizeof (__u32), GFP_KERNEL);
    memset (workspace, 0, 80 * sizeof (__u32));
    out_buffer = kmalloc (BUF_SIZE * sizeof (char), GFP_KERNEL);
    dev = (struct usb_noise*) data;
    
    while (1) {
    	if (kthread_should_stop ())
	   break;
    
    	mutex_lock (&dev->io_mutex);
    	if (!dev->interface) {
	   result = -ENODEV;
	   mutex_unlock (&dev->io_mutex);
	   return result;
    	}
    
    	result = usb_control_msg (dev->udev, usb_rcvctrlpipe (dev->udev, 0), 
				0xa5, 0xc0, 0, 0, dev->buffer, BUF_SIZE, 1000);
    	j = 0;
    	for (i = 0; i < BUF_SIZE; i+=2) {
	    out_buffer[j] = dev->buffer[i];
	    ++j;
    	}
    	result = usb_control_msg (dev->udev, usb_rcvctrlpipe (dev->udev, 0), 
    			0xa5, 0xc0, 0, 0, dev->buffer, BUF_SIZE, 1000);
    	for (i = 0; i < BUF_SIZE; i+=2) {
	   out_buffer[j] = dev->buffer[i];
	   ++j;
    	}
    
    	//sha_init (digest);
    	//sha_transform (digest, out_buffer, workspace);
    	//memcpy (out_buffer, digest, 20);
    	for (i = 0; i < 64; i++)
	   add_input_randomness (out_buffer[i], out_buffer[64 - i], out_buffer[i]);
	mutex_unlock (&dev->io_mutex);
		
	msleep (50);
    }
    return 0;
}
*
 }}} */

/* {{{ noise_init */
static int __init noise_init (void) {
    int result;
    result = usb_register (&noise_usb_driver);
    if (!result) 
    	printk ("noise_init successful!\n");
    else
    	printk ("noise_init unsuccessful!\n");
    big_buffer = kzalloc (BIG_BUF_SIZE * sizeof (char), GFP_KERNEL);
    offset = 0;
    return result;
} /* }}} */

/* {{{ noise_exit */
static void __exit noise_exit (void) {
    printk ("exiting!\n");
    /* XXX: zatrymywanie w±tku
    kthread_stop (filling_thread); */
    kzfree (big_buffer);
    usb_kill_urb (urb1);
    usb_free_urb (urb1);
    usb_deregister (&noise_usb_driver);
} /* }}} */

/* {{{ noise_probe */
int noise_probe (struct usb_interface *intf, const struct usb_device_id *id) {
    struct usb_noise *noise_dev;
    struct usb_host_interface* iface_desc;
    struct usb_endpoint_descriptor* endpoint;
    int res = -ENOMEM;
   
    cmd = kmalloc (sizeof (struct usb_ctrlrequest), GFP_KERNEL);
    noise_dev = kzalloc (sizeof (*noise_dev), GFP_KERNEL);
    if (!noise_dev) {
    	printk ("Out of memory!\n");
    	return res;
    }
    kref_init (&noise_dev->kref);
    mutex_init (&noise_dev->io_mutex);
    
    noise_dev->udev = usb_get_dev (interface_to_usbdev (intf));
    noise_dev->interface = intf;
    iface_desc = intf->cur_altsetting;
    
    /* mamy tu tylko jeden u¿yteczny endpoint 
     * TODO: w sumie to on nie jest nam jako¶ bardzo potrzebny */
    endpoint = &iface_desc->endpoint[0].desc;
    noise_dev->endpoint_addr = endpoint->bEndpointAddress;
    
    usb_set_intfdata (intf, noise_dev);
    res = usb_register_dev (intf, &noise_class);
    
    if (res) {
    	printk ("problem with getting a minor\n");
    	usb_set_intfdata (intf, NULL);
    	goto err;
    }/*
    else {
    	printk ("successfully registered, minor: %d\n", intf->minor);
    }*/
    noise_dev->buffer = kmalloc (BUF_SIZE, GFP_KERNEL);
    noise_dev->buffer_size = BUF_SIZE;
    
    
    printk ("%s\n", noise_dev->udev->manufacturer);

    /* wype³nijmy strukturê usb_ctrlrequest w³a¶ciwymi warto¶ciami */
    cmd->bRequestType = 0xc0;
    cmd->bRequest = 0xa5;
    cmd->wValue = cpu_to_le16(0);
    cmd->wIndex = cpu_to_le16(0);
    cmd->wLength = BUF_SIZE;
    /* XXX: uruchamianie w±tku
    filling_thread = kthread_run ( fill_func, noise_dev,
    "entropy_fill_thread" ); */

    urb1 = usb_alloc_urb (0, GFP_NOIO);
    if (!urb1) {
	res = -ENOMEM;
	goto err;
    }
    usb_fill_control_urb (urb1, noise_dev->udev, 
	    usb_rcvctrlpipe (noise_dev->udev, 0),
	    (unsigned char *) cmd, noise_dev->buffer, 
	    BUF_SIZE, noise_urb_callback, noise_dev);
    
    if (0 == usb_submit_urb (urb1, GFP_KERNEL))
	printk ("control urb sent successfully\n");
    else
	printk ("error submitting control urb!\n");

    
    return 0;

err: 	
    if (noise_dev) {
    	kref_put (&noise_dev->kref, noise_delete);
    }
    
    return res;
} /* }}} */

/* {{{ noise_disconnect - od³±czamy urz±dzenie */
void noise_disconnect (struct usb_interface *intf) {
    int minor = intf->minor;
    lock_kernel ();
    usb_set_intfdata (intf, NULL);
    usb_deregister_dev (intf, &noise_class);
    unlock_kernel ();
    printk ("Noise Generator (minor %d) disconnected!\n", minor);
} /* }}} */

/* {{{ noise_delete */
static void noise_delete (struct kref* kref) {
    struct usb_noise *dev = to_noise_dev (kref);
    usb_put_dev (dev->udev);
    kfree (dev->buffer);
    kfree (dev);
} /* }}} */

/* {{{ noise_open */
static int noise_open (struct inode* inode, struct file* file) {
    struct usb_noise* dev;
    struct usb_interface* interface;
    int subminor;
    int result = 0;
    
    subminor = iminor (inode);
    
    interface = usb_find_interface (&noise_usb_driver, subminor);
    if (!interface) {
    	printk ("%s - error, can't find device for minor%d", 
    			__func__, subminor);
    	result = -ENODEV;
    	return result;
    }
    
    dev = usb_get_intfdata (interface);
    if (!dev) {
    	result = -ENODEV;
    	return result;
    }
    
    kref_get (&dev->kref);
    mutex_lock (&dev->io_mutex);
    
    if (!dev->open_count++) {
    	result = usb_autopm_get_interface (interface);
    	if (result) {
    		dev->open_count--;
    		mutex_unlock (&dev->io_mutex);
    		kref_put (&dev->kref, noise_delete);
    		return result;
    	}
    }
    
    file->private_data = dev;
    mutex_unlock (&dev->io_mutex);
    return result;
} /* }}} */

/* {{{ noise_release */
static int noise_release (struct inode* inode, struct file* file) {
    struct usb_noise* dev;
    dev = (struct usb_noise*) file->private_data;
    if (dev == NULL)
    	return -ENODEV;
    /* zezwól na wstrzymywanie */
    mutex_lock (&dev->io_mutex);
    if (!--dev->open_count && dev->interface)
    	usb_autopm_put_interface (dev->interface);
    mutex_unlock (&dev->io_mutex);
    
    kref_put (&dev->kref, noise_delete);
    return 0;
} /* }}} */

/* {{{ noise_read */
static ssize_t noise_read (struct file* file, char* buffer, 
			size_t count, loff_t* ppos) {
    struct usb_noise* dev;	/* reprezentacja urz±dzenia */
    int result, i, j;		/* zmienne pomocnicze */
    __u32 *digest;		/* wynik dzia³ania funkcji skrótu */
    __u32 *workspace;		/* przestrzeñ robocza dla sha_transform */
    char *out_buffer;		/* bufor wyj¶ciowy */
    
    /* alokacja pamiêci, inicjalizacja zmiennych */
    digest = kmalloc (5 * sizeof (__u32), GFP_KERNEL);
    workspace = kmalloc (80 * sizeof (__u32), GFP_KERNEL);
    memset (workspace, 0, 80 * sizeof (__u32));
    out_buffer = kmalloc (BUF_SIZE * sizeof (char), GFP_KERNEL);
    dev = (struct usb_noise*) file->private_data;
    j = 0;
    
    /* --- SEKCJA KRYTYCZNA - POCZ¡TEK --- */
    mutex_lock (&dev->io_mutex);
    if (!dev->interface) {
    	result = -ENODEV;
    	mutex_unlock (&dev->io_mutex);
    	return result;
    }

SK: if (offset < 2*BUF_SIZE) {
	usb_submit_urb (urb1, GFP_KERNEL);
	//mutex_unlock (&dev->io_mutex);
	msleep (100);
	goto SK;
    }
    else {
	memcpy (dev->buffer, big_buffer + offset, BUF_SIZE);
	for (i = 0; i < BUF_SIZE; i+=2) {
	    out_buffer[j] = dev->buffer[i];
	    ++j;
	}
	offset -= BUF_SIZE;
	memcpy (dev->buffer, big_buffer + offset, BUF_SIZE);
	for (i = 0; i < BUF_SIZE; i+=2) {
	    out_buffer[j] = dev->buffer[i];
	    ++j;
	}
    }
    
    /* wykonaj skrót */
    sha_init (digest);
    sha_transform (digest, out_buffer, workspace);
    memcpy (out_buffer, digest, 20);
    
    /*for (i = 0; i < 20; i++)
    	add_input_randomness (out_buffer[i], out_buffer[i],
    	out_buffer[i]);*/
    
    /* skopiuj wygenerowany skrót do przestrzeni u¿ytkownika */
    if (copy_to_user (buffer, out_buffer, 20))
    	result = -EFAULT;
    else if (count < 20)
    	result = count;
    else
    	result = 20;
    /* --- SEKCJA KRYTYCZNA - KONIEC --- */
    mutex_unlock (&dev->io_mutex);
    return result;
} /* }}} */

/* {{{ sha_init - wykorzystywane przez SHA */
void sha_init(__u32 *buf) {
    buf[0] = 0x67452301;
    buf[1] = 0xefcdab89;
    buf[2] = 0x98badcfe;
    buf[3] = 0x10325476;
    buf[4] = 0xc3d2e1f0;
} /* }}} */

/* {{{ noise_urb_callback - urb wraca z urz±dzenia */
static void noise_urb_callback (struct urb *urb) {
    struct usb_noise *dev;

    dev = urb->context;
    printk ("%d\n", offset);
    /* --- SEKCJA KRYTYCZNA - POCZ¡TEK --- */
    //mutex_lock ( &dev->io_mutex );
    if (offset < BIG_BUF_SIZE) {
	memcpy (big_buffer + offset, urb->transfer_buffer, BUF_SIZE);
	offset += BUF_SIZE;
	//mutex_unlock ( &dev->io_mutex );
	usb_submit_urb (urb1, GFP_ATOMIC);
    }
    else {
	;//mutex_unlock ( &dev->io_mutex );
    }
    /* --- SEKCJA KRYTYCZNA - KONIEC --- */
} /* }}} */

module_init (noise_init);
module_exit (noise_exit);
