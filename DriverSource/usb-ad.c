/*
 * USB Skeleton driver - 2.2
 *
 * Copyright (C) 2001-2004 Greg Kroah-Hartman (greg@kroah.com)
 *
 *      This program is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU General Public License as
 *      published by the Free Software Foundation, version 2.
 *
 * This driver is based on the 2.6.3 version of drivers/usb/usb-skeleton.c
 * but has been rewritten to be easier to read and use.
 *
 */

#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/kref.h>
#include <linux/uaccess.h>
#include <linux/usb.h>
#include <linux/mutex.h>
#include "client.h"       
#include "constants.h"     
#include "usb-ioctl.h"


/* Define these values to match your devices */
#define USB_AD_VENDOR_ID      0xa5a5    //hex(42405)
#define USB_AD_PRODUCT_ID     0x01      //hex(1)

/* table of devices that work with this driver */
static const struct usb_device_id ad_table[] = {
        { USB_DEVICE(USB_AD_VENDOR_ID, USB_AD_PRODUCT_ID) },
        { }                                     /* Terminating entry */
};
MODULE_DEVICE_TABLE(usb, ad_table);

/* Get a minor range for your devices from the usb maintainer */
#define USB_AD_MINOR_BASE     192   //:TODO:

/* our private defines. if this grows any larger, use your own .h file */
#define MAX_TRANSFER            (PAGE_SIZE - 512)
/* MAX_TRANSFER is chosen so that the VM is not stressed by
   allocations > PAGE_SIZE and the number of packets in a page
   is an integer 512 is the largest possible packet on EHCI */
#define WRITES_IN_FLIGHT        8
/* arbitrarily chosen */

/* tablica wskaznikow na programy - klientow */
Client** gpClients_array;
//int Clients_number=0;
//DECLARE_WAIT_QUEUE_HEAD(queue);



/* Structure to hold all of our device specific stuff */
struct usb_ad {
        struct usb_device       *udev;                  /* the usb device for this device */
        struct usb_interface    *interface;             /* the interface for this device */
        struct semaphore        limit_sem;              /* limiting the number of writes in progress */
        struct usb_anchor       submitted;              /* in case we need to retract our submissions */
        struct urb              *bulk_in_urb;           /* the urb to read data with */
        unsigned char           *bulk_in_buffer;        /* the buffer to receive data */
        size_t                  bulk_in_size;           /* the size of the receive buffer */
        size_t                  bulk_in_filled;         /* number of bytes in the buffer */
        size_t                  bulk_in_copied;         /* already copied to user space */
        __u8                    bulk_in_endpointAddr;   /* the address of the bulk in endpoint */
        __u8                    bulk_out_endpointAddr;  /* the address of the bulk out endpoint */
        int                     errors;                 /* the last request tanked */
        int                     open_count;             /* count the number of openers */
        bool                    ongoing_read;           /* a read is going on */
        bool                    processed_urb;          /* indicates we haven't processed the urb */
        spinlock_t              err_lock;               /* lock for errors */
        struct kref             kref;
        struct mutex            io_mutex;               /* synchronize I/O with disconnect */
        struct completion       bulk_in_completion;     /* to wait for an ongoing read */
};
#define to_ad_dev(d) container_of(d, struct usb_ad, kref)

static struct usb_driver ad_driver;
static void ad_draw_down(struct usb_ad *dev);

static void ad_delete(struct kref *kref)
{
        struct usb_ad *dev = to_ad_dev(kref);

        usb_free_urb(dev->bulk_in_urb);
        usb_put_dev(dev->udev);
        kfree(dev->bulk_in_buffer);
        kfree(dev);
}

static int ad_open(struct inode *inode, struct file *file)
{
        struct usb_ad *dev;
        struct usb_interface *interface;
        int subminor;
        int retval = 0;

        subminor = iminor(inode);

        interface = usb_find_interface(&ad_driver, subminor);
        if (!interface) {
                err("%s - error, can't find device for minor %d",
                     __func__, subminor);
                retval = -ENODEV;
                goto exit;
        }

        dev = usb_get_intfdata(interface);
        if (!dev) {
                retval = -ENODEV;
                goto exit;
        }

        /* increment our usage count for the device */
        kref_get(&dev->kref);

        /* lock the device to allow correctly handling errors
         * in resumption */
        mutex_lock(&dev->io_mutex);

        if (!dev->open_count++) {
                retval = usb_autopm_get_interface(interface);
                        if (retval) {
                                dev->open_count--;
                                mutex_unlock(&dev->io_mutex);
                                kref_put(&dev->kref, ad_delete);
                                goto exit;
                        }
        } /* else { //uncomment this block if you want exclusive open
                retval = -EBUSY;
                dev->open_count--;
                mutex_unlock(&dev->io_mutex);
                kref_put(&dev->kref, ad_delete);
                goto exit;
        } */
        /* prevent the device from being autosuspended */

        /* save our object in the file's private structure */
        file->private_data = dev;
        mutex_unlock(&dev->io_mutex);

exit:
        return retval;
}

static int ad_release(struct inode *inode, struct file *file)
{
        struct usb_ad *dev;

        dev = (struct usb_ad *)file->private_data;
        if (dev == NULL)
                return -ENODEV;

        /* allow the device to be autosuspended */
        mutex_lock(&dev->io_mutex);
        if (!--dev->open_count && dev->interface)
                usb_autopm_put_interface(dev->interface);
        mutex_unlock(&dev->io_mutex);

        /* decrement the count on our device */
        kref_put(&dev->kref, ad_delete);
        return 0;
}

static int ad_flush(struct file *file, fl_owner_t id)
{
        struct usb_ad *dev;
        int res;

        dev = (struct usb_ad *)file->private_data;
        if (dev == NULL)
                return -ENODEV;

        /* wait for io to stop */
        mutex_lock(&dev->io_mutex);
        ad_draw_down(dev);

        /* read out errors, leave subsequent opens a clean slate */
        spin_lock_irq(&dev->err_lock);
        res = dev->errors ? (dev->errors == -EPIPE ? -EPIPE : -EIO) : 0;
        dev->errors = 0;
        spin_unlock_irq(&dev->err_lock);

        mutex_unlock(&dev->io_mutex);

        return res;
}

static void ad_read_bulk_callback(struct urb *urb)  //completion handler. in interrupt context!
{
        struct usb_ad *dev;

        dev = urb->context;

        spin_lock(&dev->err_lock);
        /* sync/async unlink faults aren't errors */
        if (urb->status) {
                if (!(urb->status == -ENOENT ||
                    urb->status == -ECONNRESET ||
                    urb->status == -ESHUTDOWN))
                        err("%s - nonzero write bulk status received: %d",
                            __func__, urb->status);

                dev->errors = urb->status;
        } else {
                dev->bulk_in_filled = urb->actual_length;
        }
        dev->ongoing_read = 0;
        spin_unlock(&dev->err_lock);

        complete(&dev->bulk_in_completion);
}

static int ad_do_read_io(struct usb_ad *dev, size_t count)
{
        int rv;

        /* prepare a read */
        usb_fill_bulk_urb(dev->bulk_in_urb,
                        dev->udev,
                        usb_rcvbulkpipe(dev->udev,
                                dev->bulk_in_endpointAddr),
                        dev->bulk_in_buffer,
                        min(dev->bulk_in_size, count),
                        ad_read_bulk_callback,
                        dev);
        /* tell everybody to leave the URB alone */
        spin_lock_irq(&dev->err_lock);
        dev->ongoing_read = 1;
        spin_unlock_irq(&dev->err_lock);

        /* do it */
        rv = usb_submit_urb(dev->bulk_in_urb, GFP_KERNEL);
        if (rv < 0) {
                err("%s - failed submitting read urb, error %d",
                        __func__, rv);
                dev->bulk_in_filled = 0;
                rv = (rv == -ENOMEM) ? rv : -EIO;
                spin_lock_irq(&dev->err_lock);
                dev->ongoing_read = 0;
                spin_unlock_irq(&dev->err_lock);
        }

        return rv;
}

static ssize_t ad_read(struct file *file, char *buffer, size_t count,
                         loff_t *ppos)
{
        struct usb_ad *dev;
        int rv;
        bool ongoing_io;

        dev = (struct usb_ad *)file->private_data;

        /* if we cannot read at all, return EOF */
        if (!dev->bulk_in_urb || !count)
                return 0;

        /* no concurrent readers */
        rv = mutex_lock_interruptible(&dev->io_mutex);
        if (rv < 0)
                return rv;

        if (!dev->interface) {          /* disconnect() was called */
                rv = -ENODEV;
                goto exit;
        }

        /* if IO is under way, we must not touch things */
retry:
        spin_lock_irq(&dev->err_lock);
        ongoing_io = dev->ongoing_read;
        spin_unlock_irq(&dev->err_lock);

        if (ongoing_io) {
                /* nonblocking IO shall not wait */
                if (file->f_flags & O_NONBLOCK) {
                        rv = -EAGAIN;
                        goto exit;
                }
                /*
                 * IO may take forever
                 * hence wait in an interruptible state
                 */
                rv = wait_for_completion_interruptible(&dev->bulk_in_completion);
                if (rv < 0)
                        goto exit;
                /*
                 * by waiting we also semiprocessed the urb
                 * we must finish now
                 */
                dev->bulk_in_copied = 0;
                dev->processed_urb = 1;
        }

        if (!dev->processed_urb) {
                /*
                 * the URB hasn't been processed
                 * do it now
                 */
                wait_for_completion(&dev->bulk_in_completion);
                dev->bulk_in_copied = 0;
                dev->processed_urb = 1;
        }

        /* errors must be reported */
        rv = dev->errors;
        if (rv < 0) {
                /* any error is reported once */
                dev->errors = 0;
                /* to preserve notifications about reset */
                rv = (rv == -EPIPE) ? rv : -EIO;
                /* no data to deliver */
                dev->bulk_in_filled = 0;
                /* report it */
                goto exit;
        }

        /*
         * if the buffer is filled we may satisfy the read
         * else we need to start IO
         */

        if (dev->bulk_in_filled) {
                /* we had read data */
                size_t available = dev->bulk_in_filled - dev->bulk_in_copied;
                size_t chunk = min(available, count);

                if (!available) {
                        /*
                         * all data has been used
                         * actual IO needs to be done
                         */
                        rv = ad_do_read_io(dev, count);
                        if (rv < 0)
                                goto exit;
                        else
                                goto retry;
                }
                /*
                 * data is available
                 * chunk tells us how much shall be copied
                 */

                if (copy_to_user(buffer,
                                 dev->bulk_in_buffer + dev->bulk_in_copied,
                                 chunk))
                        rv = -EFAULT;
                else
                        rv = chunk;

                dev->bulk_in_copied += chunk;

                /*
                 * if we are asked for more than we have,
                 * we start IO but don't wait
                 */
                if (available < count)
                        ad_do_read_io(dev, count - chunk);
        } else {
                /* no data in the buffer */
                rv = ad_do_read_io(dev, count);
                if (rv < 0)
                        goto exit;
                else if (!(file->f_flags & O_NONBLOCK))
                        goto retry;
                rv = -EAGAIN;
        }
exit:
        mutex_unlock(&dev->io_mutex);
        return rv;
}

static void ad_write_bulk_callback(struct urb *urb) //completion handler. called in interrupt context!
{
        struct usb_ad *dev;

        dev = urb->context;

        /* sync/async unlink faults aren't errors */
        if (urb->status) {
                if (!(urb->status == -ENOENT ||
                    urb->status == -ECONNRESET ||
                    urb->status == -ESHUTDOWN))
                        err("%s - nonzero write bulk status received: %d",
                            __func__, urb->status);

                spin_lock(&dev->err_lock);
                dev->errors = urb->status;
                spin_unlock(&dev->err_lock);
        }

        /* free up our allocated buffer */
        usb_buffer_free(urb->dev, urb->transfer_buffer_length,
                        urb->transfer_buffer, urb->transfer_dma);
        up(&dev->limit_sem);
}

static ssize_t ad_write(struct file *file, const char *user_buffer,
                          size_t count, loff_t *ppos)
{
        struct usb_ad *dev;
        int retval = 0;
        struct urb *urb = NULL;
        char *buf = NULL;
        size_t writesize = min(count, (size_t)MAX_TRANSFER);

        dev = (struct usb_ad *)file->private_data;

        /* verify that we actually have some data to write */
        if (count == 0)
                goto exit;

        /*
         * limit the number of URBs in flight to stop a user from using up all
         * RAM
         */
        if (!(file->f_flags & O_NONBLOCK)) {
                if (down_interruptible(&dev->limit_sem)) {
                        retval = -ERESTARTSYS;
                        goto exit;
                }
        } else {
                if (down_trylock(&dev->limit_sem)) {
                        retval = -EAGAIN;
                        goto exit;
                }
        }

        spin_lock_irq(&dev->err_lock);
        retval = dev->errors;
        if (retval < 0) {
                /* any error is reported once */
                dev->errors = 0;
                /* to preserve notifications about reset */
                retval = (retval == -EPIPE) ? retval : -EIO;
        }
        spin_unlock_irq(&dev->err_lock);
        if (retval < 0)
                goto error;

        /* create a urb, and a buffer for it, and copy the data to the urb */
        urb = usb_alloc_urb(0, GFP_KERNEL);
        if (!urb) {
                retval = -ENOMEM;
                goto error;
        }

        buf = usb_buffer_alloc(dev->udev, writesize, GFP_KERNEL,
                               &urb->transfer_dma);
        if (!buf) {
                retval = -ENOMEM;
                goto error;
        }

        if (copy_from_user(buf, user_buffer, writesize)) {
                retval = -EFAULT;
                goto error;
        }

        /* this lock makes sure we don't submit URBs to gone devices */
        mutex_lock(&dev->io_mutex);
        if (!dev->interface) {          /* disconnect() was called */
                mutex_unlock(&dev->io_mutex);
                retval = -ENODEV;
                goto error;
        }

        /* initialize the urb properly */
        usb_fill_bulk_urb(urb, dev->udev,
                          usb_sndbulkpipe(dev->udev, dev->bulk_out_endpointAddr),
                          buf, writesize, ad_write_bulk_callback, dev);
        urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;
        usb_anchor_urb(urb, &dev->submitted);

        /* send the data out the bulk port */
        retval = usb_submit_urb(urb, GFP_KERNEL);
        mutex_unlock(&dev->io_mutex);
        if (retval) {
                err("%s - failed submitting write urb, error %d", __func__,
                    retval);
                goto error_unanchor;
        }

        /*
         * release our reference to this urb, the USB core will eventually free
         * it entirely
         */
        usb_free_urb(urb);


        return writesize;

error_unanchor:
        usb_unanchor_urb(urb);
error:
        if (urb) {
                usb_buffer_free(dev->udev, writesize, buf, urb->transfer_dma);
                usb_free_urb(urb);
        }
        up(&dev->limit_sem);

exit:
        return retval;
}
/*
 *IOCTL
 *Funkcja obsługująca ioctle
 *
 *
 */
int ad_ioctl(struct inode *indoe, struct file *file, unsigned int ioctl_num, unsigned long ioctl_param)
{
//zmienne
int pid;
int i,j;
int channels = 0;
int new = 1;
unsigned char pom[USB_AD_CHANNELS_NUM];

//sprawdzenie czy to nasz
if (access_ok(VERIFY_READ,(char *)ioctl_param,2 * sizeof(int) ) != 0)
        return -EFAULT;  
if (((int*)ioctl_param)[0] != USB_AD_IOCTL_MAGIC_NUMBER) {
        printk("<1>USB_AD: Nie nasz IOCTL\n");
        return -EIO;
        }
pid = ((int*)ioctl_param)[1];
//dorwanie klienta {czy tworzymy nowego czy staremu zmieniamy parametry probkowania}
for (i = 0;i < USB_AD_MAX_CLIENTS_NUM;i++)
        if(gpClients_array[i] != NULL)
                if (gpClients_array[i]->pid == pid) {
                        new = 0;
                        break;
                        }
//swybranie tego co mamy klientowi zrobic
switch (ioctl_num) {
        case IOCTL_SET_PARAMS:
        //czy wskaxnik jest ok
                if (access_ok(VERIFY_READ,(char *)ioctl_param,(4+USB_AD_CHANNELS_NUM) * sizeof(int)) != 0) 
                        return -EFAULT;
                for (j = 0;j < USB_AD_CHANNELS_NUM;j++)
                        pom[j] = ((int *)ioctl_param)[j + 4];
                if (new == 1) {
                        for (j = 0;j < USB_AD_MAX_CLIENTS_NUM; j++)
                                if (gpClients_array[j] == NULL)
                                        break;
                        gpClients_array[j] = kmalloc(sizeof(Client),GFP_KERNEL);
                        if (gpClients_array[j] == NULL) 
                                return -EFAULT;
                        if (init_client(gpClients_array[j],pid,((int *)ioctl_param)[3],((int *)ioctl_param)[4],pom) != 0)
                                return -EFAULT;        
                        }
                        else {
                        /*TODO: zmina parametrow*/
                        }
                break;
        case IOCTL_GET_DATA:
        //czy wskaxnik jest ok
                if (new == 1) {
                        printk("<1>USB_AD: Najpierw trzeba sie przedstawic sterownikowi, potem pobierac dane");
                        return -EFAULT;
                        }
                for (j = 0;j < USB_AD_CHANNELS_NUM;j++)
                        if (gpClients_array[i]->channels[j] == 1)
                                channels++;
                if (access_ok(VERIFY_WRITE,(char *)ioctl_param,gpClients_array[i]->buffer_size*channels) != 0 )
                        return -EFAULT;
                //sprawdzic czy jest co wyslac jak nie to zawiesic
                wait_event(gpClients_array[i]->queue,(gpClients_array[i]->buffer_full_number == 1 || gpClients_array[i]->buffer_full_number == 2));
                //wait_event(queue,(gpClients_array[i]->buffer_full_number == 1 || gpClients_array[i]->buffer_full_number == 2));
                //sprawdzenie ktory bufor mamy wyslac	
                switch (gpClients_array[i]->buffer_full_number) {
                        case 1:return copy_to_user(gpClients_array[i]->first_buf.buf,(char *)ioctl_param,gpClients_array[i]->first_buf.size);break;
                        case 2:return copy_to_user(gpClients_array[i]->second_buf.buf,(char *)ioctl_param,gpClients_array[i]->second_buf.size);break;
                        }
                break;
        default: printk("<1>USB_AD: TO NIE MOZE SIE ZDAZYC (AKURAT)\n");
        }


return 0;
}
//--------------------------------------------------------------------IOCTL_end

static const struct file_operations ad_fops = {
        .owner =        THIS_MODULE,
        .read =         ad_read,
        .write =        ad_write,
	.ioctl =	ad_ioctl,
        .open =         ad_open,
        .release =      ad_release,
        .flush =        ad_flush,
};

/*
 * usb class driver info in order to get a minor number from the usb core,
 * and to have the device registered with the driver core
 */
static struct usb_class_driver ad_class = {
        .name =         "USB_AD%d",
        .fops =         &ad_fops,
        .minor_base =   USB_AD_MINOR_BASE,
};

static int ad_probe(struct usb_interface *interface,
                      const struct usb_device_id *id)
{
        struct usb_ad *dev;
        struct usb_host_interface *iface_desc;
        struct usb_endpoint_descriptor *endpoint;
        size_t buffer_size;
        int i;
        int retval = -ENOMEM;
        
        printk("<1>USB_AD : ad_probe executed\n");
        /* allocate memory for our device state and initialize it */
        dev = kzalloc(sizeof(*dev), GFP_KERNEL);
        if (!dev) {
                err("Out of memory");
                goto error;
        }
        kref_init(&dev->kref);
        sema_init(&dev->limit_sem, WRITES_IN_FLIGHT);
        mutex_init(&dev->io_mutex);
        spin_lock_init(&dev->err_lock);
        init_usb_anchor(&dev->submitted);
        init_completion(&dev->bulk_in_completion);

        dev->udev = usb_get_dev(interface_to_usbdev(interface));
        dev->interface = interface;

        /* set up the endpoint information */
        /* use only the first bulk-in and bulk-out endpoints */
        iface_desc = interface->cur_altsetting;
        for (i = 0; i < iface_desc->desc.bNumEndpoints; ++i) {
                endpoint = &iface_desc->endpoint[i].desc;

                if (!dev->bulk_in_endpointAddr &&
                    usb_endpoint_is_bulk_in(endpoint)) {
                        /* we found a bulk in endpoint */
                        buffer_size = le16_to_cpu(endpoint->wMaxPacketSize);
                        dev->bulk_in_size = buffer_size;
                        dev->bulk_in_endpointAddr = endpoint->bEndpointAddress;
                        dev->bulk_in_buffer = kmalloc(buffer_size, GFP_KERNEL);
                        if (!dev->bulk_in_buffer) {
                                err("Could not allocate bulk_in_buffer");
                                goto error;
                        }
                        dev->bulk_in_urb = usb_alloc_urb(0, GFP_KERNEL);
                        if (!dev->bulk_in_urb) {
                                err("Could not allocate bulk_in_urb");
                                goto error;
                        }
                }

                if (!dev->bulk_out_endpointAddr &&
                    usb_endpoint_is_bulk_out(endpoint)) {
                        /* we found a bulk out endpoint */
                        dev->bulk_out_endpointAddr = endpoint->bEndpointAddress;
                }
        }
        if (!(dev->bulk_in_endpointAddr && dev->bulk_out_endpointAddr)) {
                err("Could not find both bulk-in and bulk-out endpoints");
                goto error;
        }

        /* save our data pointer in this interface device */
        usb_set_intfdata(interface, dev);

        /* we can register the device now, as it is ready */
        retval = usb_register_dev(interface, &ad_class);
        if (retval) {
                /* something prevented us from registering this driver */
                err("Not able to get a minor for this device.");
                usb_set_intfdata(interface, NULL);
                goto error;
        }

        /* let the user know what node this device is now attached to */
        dev_info(&interface->dev,
                 "USB Skeleton device now attached to USBSkel-%d",
                 interface->minor);
        return 0;

error:
        if (dev)
                /* this frees allocated memory */
                kref_put(&dev->kref, ad_delete);
        return retval;
}

static void ad_disconnect(struct usb_interface *interface)
{
        struct usb_ad *dev;
        int minor = interface->minor;

        dev = usb_get_intfdata(interface);
        usb_set_intfdata(interface, NULL);

        /* give back our minor */
        usb_deregister_dev(interface, &ad_class);

        /* prevent more I/O from starting */
        mutex_lock(&dev->io_mutex);
        dev->interface = NULL;
        mutex_unlock(&dev->io_mutex);

        usb_kill_anchored_urbs(&dev->submitted);

        /* decrement our usage count */
        kref_put(&dev->kref, ad_delete);

        dev_info(&interface->dev, "USB Skeleton #%d now disconnected", minor);
}

static void ad_draw_down(struct usb_ad *dev)
{
        int time;

        time = usb_wait_anchor_empty_timeout(&dev->submitted, 1000);
        if (!time)
                usb_kill_anchored_urbs(&dev->submitted);
        usb_kill_urb(dev->bulk_in_urb);
}

static int ad_suspend(struct usb_interface *intf, pm_message_t message)
{
        struct usb_ad *dev = usb_get_intfdata(intf);

        if (!dev)
                return 0;
        ad_draw_down(dev);
        return 0;
}

static int ad_resume(struct usb_interface *intf)
{
        return 0;
}

static int ad_pre_reset(struct usb_interface *intf)
{
        struct usb_ad *dev = usb_get_intfdata(intf);

        mutex_lock(&dev->io_mutex);
        ad_draw_down(dev);

        return 0;
}

static int ad_post_reset(struct usb_interface *intf)
{
        struct usb_ad *dev = usb_get_intfdata(intf);

        /* we are sure no URBs are active - no locking needed */
        dev->errors = -EPIPE;
        mutex_unlock(&dev->io_mutex);

        return 0;
}

static struct usb_driver ad_driver = {
        .name =         "USB_AD",   
        .probe =        ad_probe,           
        .disconnect =   ad_disconnect,  
        .suspend =      ad_suspend,
        .resume =       ad_resume,
        .pre_reset =    ad_pre_reset,
        .post_reset =   ad_post_reset,
        .id_table =     ad_table,
        .supports_autosuspend = 1,
};

static int __init usb_ad_init(void)
{
        int result;
        unsigned char test_channels[USB_AD_CHANNELS_NUM] = {1,0,0,0,0,0,0};
        printk("<1>USB_AD : usb_ad_init executed\n");
        /* register this driver with the USB subsystem */
        result = usb_register(&ad_driver);
        if (result)
                err("usb_register failed. Error number %d", result);
        
        /* alokuj i zeruj pamiec dla tablicy wskaznikow na klientow */
        gpClients_array = kzalloc(sizeof(Client*) * USB_AD_MAX_CLIENTS_NUM, GFP_KERNEL);
        if(gpClients_array == NULL)
            err("usb_register failed. Error number %d", USB_AD_ERROR_ALLOCATION);
            
        /* alokuj pamiec dla testowego klienta */
        gpClients_array[0] = kmalloc(sizeof(Client), GFP_KERNEL);
        if(gpClients_array[0] == NULL)
            err("usb_register failed. Error number %d", USB_AD_ERROR_ALLOCATION);

    
        init_client(gpClients_array[0], 555, 128, 100, test_channels);
            
        printk("<1>USB_AD : usb_ad_init successful\n");
        return result;
}

static void __exit usb_ad_exit(void)
{
        printk("<1>USB_AD : usb_ad_exit executed\n");
        /* deregister this driver with the USB subsystem */
        usb_deregister(&ad_driver);
}

module_init(usb_ad_init);
module_exit(usb_ad_exit);

MODULE_LICENSE("GPL");
