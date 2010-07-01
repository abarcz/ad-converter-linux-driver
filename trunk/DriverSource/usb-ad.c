/*
 * USB PIC18F4550 A/D Converter driver
 *
 * Authors: Aleksy Barcz, Grzegorz Rz¹ca
 *
 * Heavily based on: USB Skeleton driver - 2.2 
 * by Greg Kroah-Hartman (greg@kroah.com)
 *
 *      This program is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU General Public License as
 *      published by the Free Software Foundation, version 2.
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

/* table of devices that work with this driver */
static const struct usb_device_id ad_table[] = {
        { USB_DEVICE(USB_AD_VENDOR_ID, USB_AD_PRODUCT_ID) },
        { }                                     /* Terminating entry */
};
MODULE_DEVICE_TABLE(usb, ad_table);

/* tablica wskaznikow na programy - klientow */
static Client** gpClients_array;

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
        bool                    running;                /* indicates that the device is already sending data */
        int                     already_printed;        /* for debug */
};
#define to_ad_dev(d) container_of(d, struct usb_ad, kref)

static struct usb_driver ad_driver;
static void ad_draw_down(struct usb_ad *dev);
static void ad_read_bulk_callback(struct urb *urb);  //completion handler. in interrupt context!

static void ad_delete_internal(struct usb_ad *dev) 
{
        int i;
        printk("<1>USB_AD : usb_ad_delete executed\n");
        if (!dev) {
            printk("<1>USB_AD : delete : no device!\n");
            return;
        }
        usb_free_urb(dev->bulk_in_urb);
        usb_put_dev(dev->udev);
        if (dev->bulk_in_buffer)
            kfree(dev->bulk_in_buffer);
        if (dev)
            kfree(dev);
        /* usuniecie struktur klientow */
        if (gpClients_array) {
                for (i = 0; i < USB_AD_MAX_CLIENTS_NUM; i ++)
                        if (gpClients_array[i]) {
                                wake_up(&gpClients_array[i]->queue);
                                kfree(gpClients_array[i]);
                                gpClients_array[i] = NULL;
                                printk("<1>USB_AD : usb_ad_delete removed a client\n");
                        }
                kfree(gpClients_array);
                gpClients_array = NULL;
                printk("<1>USB_AD : usb_ad_delete removed the Clients array\n");
        }
}

static void ad_delete(struct kref *kref)
{
        struct usb_ad *dev = to_ad_dev(kref);
        ad_delete_internal(dev);
}

static int ad_do_read_io(struct usb_ad *dev, size_t count)
{
        int rv;
        memset(dev->bulk_in_buffer, 0, dev->bulk_in_size);
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

        /* do it in ATOMIC way */
        rv = usb_submit_urb(dev->bulk_in_urb, GFP_ATOMIC);
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

/* puszcza w ruch urzadzenie */
static int ad_initialize(struct inode *inode)
{
        struct usb_ad *dev;
        struct usb_interface *interface;
        int subminor;
        int retval = 0;
        char *buffer;
        int buff_len = 30;              //w bajtach
        int actual_len = 1;

        printk("<1>USB_AD : usb_ad_initialize executed\n");

        subminor = iminor(inode);

        interface = usb_find_interface(&ad_driver, subminor);
        if (!interface) {
                err("%s - error, can't find device for minor %d",
                     __func__, subminor);
                retval = -ENODEV;
                return retval;
        }

        dev = usb_get_intfdata(interface);
        if (!dev) {
                printk("<1>USB_AD : usb_ad_initialize error : no device\n");
                retval = -ENODEV;
                return retval;
        }


        /* if the device isn't running yet, make it run */
        buffer = kzalloc(sizeof(char) * buff_len, GFP_KERNEL);
        if (buffer == NULL) {
                    printk("<1>USB_AD_open failed. Error number %d\n", USB_AD_ERROR_ALLOCATION);
                    dev->open_count--;
                    //mutex_unlock(&dev->io_mutex);
                    return retval;
        }
        buffer[0] = (char) 0;
        if (dev->running == 0) {
                retval = usb_bulk_msg(dev->udev, usb_sndbulkpipe(dev->udev, dev->bulk_out_endpointAddr),
                            buffer, buff_len, &actual_len, USB_AD_MAX_MSG_WAIT);
                if (retval != 0) {
                    printk("<1>USB_AD : usb_ad_open error couldn't send(1) %d\n", retval);
                    kfree(buffer);
                    dev->open_count--;
                    //mutex_unlock(&dev->io_mutex);
                    return retval;
                }
                memset(buffer, 0, buff_len);
                retval = usb_bulk_msg(dev->udev, usb_rcvbulkpipe(dev->udev, dev->bulk_in_endpointAddr),
                            buffer, buff_len, &actual_len, USB_AD_MAX_MSG_WAIT);
                if (retval != 0) {
                    printk("<1>USB_AD : usb_ad_open error couldn't rcv(1) %d\n", retval);
                    kfree(buffer);
                    dev->open_count--;
                    //mutex_unlock(&dev->io_mutex);
                    return retval;
                }
                buffer[buff_len - 1] = '\0';
                printk("<1>USB_AD_open received %s\n", buffer);
                memset(buffer, 0, buff_len);
                buffer[0] = (char) 1;
                /* ustawienie czestotliwosci */
                buffer[1] = (char) USB_AD_FQ_L;
                buffer[2] = (char) USB_AD_FQ_H;
                buffer[3] = (char) USB_AD_FQ_DIV;
                /* ustawienie probkowanych kanalow */
                buffer[4] = (char) 0;
                buffer[5] = (char) 1;
                buffer[6] = (char) 2;
                buffer[7] = (char) 3;
                buffer[8] = (char) 4;
                buffer[9] = (char) 8;
                buffer[10] = (char) 9;
                buffer[11] = (char) 32;         //konczy liste kanalow, musi byc > 15
                retval = usb_bulk_msg(dev->udev, usb_sndbulkpipe(dev->udev, dev->bulk_out_endpointAddr),
                            buffer, buff_len, &actual_len, USB_AD_MAX_MSG_WAIT);
                if (retval != 0) {
                    printk("<1>USB_AD : usb_ad_open error couldn't send(2) %d\n", retval);
                    kfree(buffer);
                    dev->open_count--;
                    //mutex_unlock(&dev->io_mutex);
                    return retval;
                }
                memset(buffer, 0, buff_len);
                retval = usb_bulk_msg(dev->udev, usb_rcvbulkpipe(dev->udev, dev->bulk_in_endpointAddr),
                            buffer, buff_len, &actual_len, USB_AD_MAX_MSG_WAIT);
                if (retval != 0) {
                    printk("<1>USB_AD : usb_ad_open error couldn't rcv(2) %d\n", retval);
                    //mutex_unlock(&dev->io_mutex);
                    dev->open_count--;
                    kfree(buffer);
                    return retval;
                }
                buffer[buff_len - 1] = '\0';
                printk("<1>USB_AD_open received %s\n", buffer);

                /* zlecenie startu probkowania */
                memset(buffer, 0, buff_len);
                buffer[0] = (char) 2;
                retval = usb_bulk_msg(dev->udev, usb_sndbulkpipe(dev->udev, dev->bulk_out_endpointAddr),
                            buffer, buff_len, &actual_len, USB_AD_MAX_MSG_WAIT);
                if (retval != 0) {
                    printk("<1>USB_AD : usb_ad_open error couldn't send(3) %d\n", retval);
                    kfree(buffer);
                    dev->open_count--;
                    //mutex_unlock(&dev->io_mutex);
                    return retval;
                }
                dev->running = 1;
                /* zlec odebranie "OK" i kontynuacje odbierania */
                dev->already_printed = 0;
                ad_do_read_io(dev, 25);
        }
        kfree(buffer);
        //mutex_unlock(&dev->io_mutex);

        if (retval)
            printk("<1>USB_AD : usb_ad_initialize exit with error: %d\n", retval);
        return retval;

}

static void ad_read_bulk_callback(struct urb *urb)  //completion handler. in interrupt context!
{
        struct usb_ad *dev;
        int i = 0;
        int write_res = 0;
        size_t count = 35;
        Client *client;
        unsigned char *temp_buffer;
        //printk("<1>USB_AD rclback!\n");

        dev = urb->context;

        if (!dev) {
                printk("<1>USB_AD : rclback : no device, exiting..\n");
                return;
        }
        spin_lock(&dev->err_lock);
        /* sync/async unlink faults aren't errors */
        if (urb->status) {
                if (!(urb->status == -ENOENT ||
                    urb->status == -ECONNRESET ||
                    urb->status == -ESHUTDOWN))
                        err("%s - nonzero read bulk status received: %d",
                            __func__, urb->status);

                dev->errors = urb->status;
                printk("<1>USB_AD_read_completion other error %d\n", urb->status);
        } else {
                dev->bulk_in_filled = urb->actual_length;
                //if (dev->already_printed < 20) {
                    //dev->bulk_in_buffer[dev->bulk_in_size - 1] = '\0';
                    if (dev->bulk_in_buffer[0] == 'E') {
                            printk("<1>USB_AD : E:OVERRUN received, exiting.....\n");
                            ad_delete_internal(dev);
                            printk("<1>USB_AD : E:OVERRUN deleted device\n");
                            return;
                    }
                    if (dev->bulk_in_buffer[0] != 'D')
                            goto finish;
                    /*printk("<1>USB_AD_read_completion received %c ",
                        dev->bulk_in_buffer[0]);
                    for (i = 1; i < dev->bulk_in_filled; i++)
                        printk("%d ", (int)dev->bulk_in_buffer[i]);
                    printk("<1>\n");*/
                    temp_buffer = kzalloc(dev->bulk_in_filled, GFP_ATOMIC);
                    if (!temp_buffer)
                            goto finish;
                    for (i = 0; i < USB_AD_MAX_CLIENTS_NUM; i++) {
                            client = gpClients_array[i];
                            if (client) {
                                    //printk("<1>USB_AD rclback entered client pid : %d\n", client->pid);
									if(!client->ready_for_input)
											continue;
                                    if (client->warning) {
                                            /* jesli klient zostal wczesniej ostrzezony */
                                            if (client->loops_after_warn_count <
                                                USB_AD_WAIT_LOOPS_AFTER_WARNING) {
                                                    client->loops_after_warn_count++;
                                            } else {
                                                    printk("<1>USB_AD : rclback : client with PID : %d has been removed\n", client->pid);
                                                    /* usuwamy klienta */
                                                    remove_client(client);
                                                    gpClients_array[i] = NULL;
                                            }
                                    } else {
                                            if (client->first_buf.full) {
                                                    if (client->second_buf.full) {
                                                            printk("<1>USB_AD : rclback : client with PID : %d has been warned\n", client->pid);
                                                            client->warning = 1;
                                                    } else {
                                                            //printk("<1>USB_AD rclback: choosing for sec_buf\n");
                                                            if (choose_bytes(client, dev->bulk_in_buffer, temp_buffer, dev->bulk_in_filled))
                                                                goto finish;
                                                            //printk("<1>USB_AD rclback: writing to sec_buf\n");
                                                            write_res = buffer_write(&client->second_buf,
                                                                temp_buffer,
                                                                client->channels_count * 2);
                                                            if (write_res < 0)
                                                                    printk("<1>USB_AD rclback : error writing to sec_buf %d\n", write_res);
                                                            else if (write_res == 1) {
                                                                    //printk("<1>USB_AD rclback : SECOND buf full\n");
                                                                    wake_up(&client->queue);
                                                            }
                                                    }
                                            } else {
                                                    //printk("<1>USB_AD rclback: choosing for first_buf\n");
                                                    if (choose_bytes(client, dev->bulk_in_buffer, temp_buffer,
                                                        dev->bulk_in_filled))
                                                            goto finish;
                                                    //printk("<1>USB_AD rclback: writing to first_buf\n");
                                                    write_res = buffer_write(&client->first_buf,
                                                        temp_buffer,
                                                        client->channels_count * 2);
                                                    if (write_res < 0)
                                                                    printk("<1>USB_AD rclback : error writing to first_buf %d\n", write_res);
                                                    else if (write_res == 1) {
                                                            //printk("<1>USB_AD rclback : FIRST buf full\n");
                                                            wake_up(&client->queue);
                                                    }
                                            }
                                    }
                            }
                    }
                   // dev->already_printed++;
               // }
                /*if (dev->already_printed == 5) {
                        client = gpClients_array[0];
                        if (!client)
                            goto finish;
                        print_client_buffers(client);
                        dev->already_printed++;
                }*/
        }
finish:
        dev->ongoing_read = 0;
        spin_unlock(&dev->err_lock);

        complete(&dev->bulk_in_completion);

        if (dev->running == 1) {
                //printk("<1>USB_AD_read_completion entered loop\n");
                ad_do_read_io(dev, count);
        }
}

static int ad_open(struct inode *inode, struct file *file)
{
        struct usb_ad *dev;
        struct usb_interface *interface;
        int subminor;
        int retval = 0;

        printk("<1>USB_AD : usb_ad_open executed\n");

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
                printk("<1>USB_AD : usb_ad_open error : no device\n");
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
                                printk("<1>USB_AD : usb_ad_open error(1) %d\n", retval);
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
        printk("<1>USB_AD : open number of clients is: %d device running: %d\n", dev->open_count, dev->running);
        /* save our object in the file's private structure */
        file->private_data = dev;


        mutex_unlock(&dev->io_mutex);

exit:
        if (retval)
            printk("<1>USB_AD : usb_ad_open exit with error: %d\n", retval);
        return retval;
}

static int ad_release(struct inode *inode, struct file *file)
{
        struct usb_ad *dev;
        char *buffer;
        int buff_len = 5;           //w bajtach
        int actual_len = 1;
        int retval = 0;
        int i;
        printk("<1>USB_AD : usb_ad_release executed\n");

        dev = (struct usb_ad *)file->private_data;
        if (dev == NULL) {
                printk("<1>USB_AD : usb_ad_release no device!\n");
                return -ENODEV;
        }

        /* allow the device to be autosuspended */
        mutex_lock(&dev->io_mutex);

        if (gpClients_array)
            for (i = 0; i < USB_AD_MAX_CLIENTS_NUM; i++)
                if (gpClients_array[i])
                    printk("<1>USB_AD : release client with PID = %d warning state : %d, number of warnings: %d\n",
                        gpClients_array[i]->pid,
                        gpClients_array[i]->warning,
                        gpClients_array[i]->loops_after_warn_count);
        if (!--dev->open_count && dev->interface) {
                printk("<1>USB_AD : usb_ad_release ostatni gasi swiatlo..\n");
                /* ostatni gasi swiatlo */
                buffer = kzalloc(sizeof(char) * buff_len, GFP_KERNEL);
                if (buffer == NULL) {
                        retval = USB_AD_ERROR_ALLOCATION;
                        printk("<1>USB_AD_release failed(1). Error number %d\n", USB_AD_ERROR_ALLOCATION);
                        mutex_unlock(&dev->io_mutex);
                        /* decrement the count on our device */
                        kref_put(&dev->kref, ad_delete);
                        return retval;
                }
                buffer[0] = (char) 3;               //zakoncz probkowanie
                retval = usb_bulk_msg(dev->udev, usb_sndbulkpipe(dev->udev, dev->bulk_out_endpointAddr),
                            buffer, buff_len, &actual_len, USB_AD_MAX_MSG_WAIT);
                /* instrukcja zostala przeslana, mozemy zakonczyc odbieranie */
                dev->running = 0;
                if (retval != 0) {
                        printk("<1>USB_AD_release failed(2). Error number %d\n", USB_AD_ERROR_ALLOCATION);
                        mutex_unlock(&dev->io_mutex);
                        kref_put(&dev->kref, ad_delete);
                        return retval;
                }
                retval = usb_bulk_msg(dev->udev, usb_rcvbulkpipe(dev->udev, dev->bulk_in_endpointAddr),
                            buffer, buff_len, &actual_len, USB_AD_MAX_MSG_WAIT);
                /*if (retval != 0) {
                        printk("<1>USB_AD_release failed(3). Error number %d\n", USB_AD_ERROR_ALLOCATION);
                        mutex_unlock(&dev->io_mutex);
                        kref_put(&dev->kref, ad_delete);
                        return retval;
                }*/
                buffer[buff_len - 1] = '\0';
                printk("<1>USB_AD_release received \"%s\"\n", buffer);
                usb_autopm_put_interface(dev->interface);
        }
        mutex_unlock(&dev->io_mutex);

        /* decrement the count on our device */
        kref_put(&dev->kref, ad_delete);
        return 0;
}

static int ad_flush(struct file *file, fl_owner_t id)
{
        struct usb_ad *dev;
        int res = 0;
        printk("<1>USB_AD : ad_flush executed\n");

        dev = (struct usb_ad *)file->private_data;
        if (dev == NULL)
                return -ENODEV;

        /* wait for io to stop */
        mutex_lock(&dev->io_mutex);
            printk("<1>USB_AD : ad_flush open count is : %d\n", dev->open_count);
            if (dev->open_count == 1) {     //to jest ostatni podlaczony klient
                ad_draw_down(dev);
            }
            /* read out errors, leave subsequent opens a clean slate */
            spin_lock_irq(&dev->err_lock);
            res = dev->errors ? (dev->errors == -EPIPE ? -EPIPE : -EIO) : 0;
            dev->errors = 0;
            spin_unlock_irq(&dev->err_lock);
        mutex_unlock(&dev->io_mutex);

        return res;
}

static ssize_t ad_read(struct file *file, char *buffer, size_t count,
                         loff_t *ppos)
{
        return -ENODEV;
}

static ssize_t ad_write(struct file *file, const char *user_buffer,
                          size_t count, loff_t *ppos)
{
        return -ENODEV;
}

/*
 * IOCTL
 * Funkcja obslugujaca ioctle
 *
 *
 */
int ad_ioctl(struct inode *inode, struct file *file, unsigned int ioctl_num, unsigned long ioctl_param)
{
        //zmienne
        int pid;
        int i,j;
        int too_slow = 0;
        int new = 1;
        int retval = 0;
        unsigned char pom[USB_AD_CHANNELS_NUM];
        struct usb_ad *dev;
        struct usb_interface *interface;
        int subminor;
        //printk("<1>USB_AD : usb_ad_ioctl executed\n");

        subminor = iminor(inode);

        interface = usb_find_interface(&ad_driver, subminor);
        if (!interface) {
                err("%s - error, can't find device for minor %d",
                     __func__, subminor);
                return -ENODEV;
        }

        dev = usb_get_intfdata(interface);
        if (!dev) {
                printk("<1>USB_AD : usb_ad_ioctl error : no device\n");
                return -ENODEV;
        }


        //sprawdzenie czy to nasz
        if (access_ok(VERIFY_READ,(char *)ioctl_param,2 * sizeof(int) ) == 0)
                return -EFAULT;
        if (((int*)ioctl_param)[0] != USB_AD_IOCTL_MAGIC_NUMBER) {
                printk("<1>USB_AD: Nie nasz IOCTL\n");
                return -EIO;
        }
        pid = ((int*)ioctl_param)[1];
        //dorwanie klienta {czy tworzymy nowego czy staremu zmieniamy parametry probkowania}
        printk("<1>USB_AD : usb_ad_ioctl entering mutex for PID %d\n", pid);
        mutex_lock(&dev->io_mutex);
        for (i = 0;i < USB_AD_MAX_CLIENTS_NUM;i++)
                if(gpClients_array[i] != NULL)
                        if (gpClients_array[i]->pid == pid) {
                                new = 0;
                                break;
                        }
        //wybranie tego co mamy klientowi zrobic
        switch (ioctl_num) {
            case IOCTL_SET_PARAMS:
                    printk("<1>USB_AD : usb_ad_ioctl SET_PARAMS\n");
                    //czy wskaznik jest ok
                    if (access_ok(VERIFY_READ,(char *)ioctl_param,(4+USB_AD_CHANNELS_NUM) * sizeof(int)) == 0) {
                            printk("<1>USB_AD : usb_ad_ioctl SET_PARAMS access not ok\n");
                            mutex_unlock(&dev->io_mutex);
                            return -EFAULT;
                    }
                    for (j = 0;j < USB_AD_CHANNELS_NUM;j++)
                            pom[j] = ((unsigned int *)ioctl_param)[j + 4];
                    if (new == 1) {
                            for (j = 0;j < USB_AD_MAX_CLIENTS_NUM; j++)
                                    if (gpClients_array[j] == NULL)
                                            break;
                            if(j == USB_AD_MAX_CLIENTS_NUM) {
                                    printk("<1>USB_AD : usb_ad_ioctl SET_PARAMS reached max nr of clients\n");
                                    mutex_unlock(&dev->io_mutex);
                                    return -EFAULT;
                            }
                            gpClients_array[j] = kmalloc(sizeof(Client),GFP_KERNEL);
                            if (gpClients_array[j] == NULL) {
                                    mutex_unlock(&dev->io_mutex);
                                    return -EFAULT;
                            }
							gpClients_array[j]->ready_for_input = 0;
                            if (init_client(gpClients_array[j],pid,((int *)ioctl_param)[2],((int *)ioctl_param)[3],pom) < 0) {
                                    printk("<1>USB_AD : usb_ad_ioctl SET_PARAMS couldn't init client\n");
                                    mutex_unlock(&dev->io_mutex);
                                    return -EFAULT;
                            }
							gpClients_array[j]->ready_for_input = 1;
                    } else {
                            //Wstepna implementacja: Zabijamy go i tworzymy nowego
                            remove_client(gpClients_array[i]);
                            //tworzenie nowego
                            gpClients_array[i] = kmalloc(sizeof(Client),GFP_KERNEL);
                            if (gpClients_array[i] == NULL) {
                                    mutex_unlock(&dev->io_mutex);
                                    return -EFAULT;
                            }
							gpClients_array[j]->ready_for_input = 0;
                            if (init_client(gpClients_array[i],pid,((int *)ioctl_param)[2],((int *)ioctl_param)[3],pom) < 0) {
                                    mutex_unlock(&dev->io_mutex);
                                    return -EFAULT;
                            }
							gpClients_array[j]->ready_for_input = 1;
                    }
                    ad_initialize(inode);
                    printk("<1>USB_AD : ad_ioctl SET PARAMS OK\n");
                    break;
            case IOCTL_GET_DATA:
                    printk("<1>USB_AD : usb_ad_ioctl GET_DATA for PID %d\n", pid);
                    if(!dev->open_count) {
                            printk("<1>USB_AD: Urzadzenie nie jest otwarte");
                            mutex_unlock(&dev->io_mutex);
                            return -EFAULT;
                    }

                    //czy wskaznik jest ok
                    if (new == 1) {
                            printk("<1>USB_AD: Najpierw trzeba sie przedstawic sterownikowi, potem pobierac dane");
                            mutex_unlock(&dev->io_mutex);
                            return -EFAULT;
                    }
                    if (access_ok(VERIFY_WRITE,(char *)ioctl_param,
                        gpClients_array[i]->buffer_size * gpClients_array[i]->channels_count) == 0 ) {
                            printk("<1>USB_AD : usb_ad_ioctl GET_DATA not access ok\n");
                            mutex_unlock(&dev->io_mutex);
                            return -EFAULT;
                    }
                    //sprawdzamy czy przypadkiem nie jestesmy za wolni
                    if (gpClients_array[i]-> warning == true)
                            too_slow = 1;
                        //return -TO_SLOW;
                    //sprawdzic czy jest co wyslac jak nie to zawiesic
                    mutex_unlock(&dev->io_mutex);
                    printk("<1>USB_AD : usb_ad_ioctl going to sleep for PID %d\n", pid);
                    wait_event(gpClients_array[i]->queue, (gpClients_array[i]->first_buf.full) ||
                        (gpClients_array[i]->second_buf.full));
                    printk("<1>USB_AD : usb_ad_ioctl awoken for PID %d\n", pid);
                    if(!dev)
                            return -ENODEV;
                    mutex_lock(&dev->io_mutex);
                    printk("<1>USB_AD : usb_ad_ioctl passed the mutex after wakeup for PID %d\n", pid);
                    if((!gpClients_array) || (!gpClients_array[i]))
                            return -ENODEV;
                    //sprawdzenie ktory bufor mamy wyslac
                    if (gpClients_array[i]->first_buf.full) {
                            printk("<1>USB_AD : usb_ad_ioctl going to copy first buf for PID %d\n", pid);
                            retval = buffer_copy_to_user(&(gpClients_array[i]->first_buf),
                                (char *)ioctl_param,gpClients_array[i]->first_buf.size);
                            printk("<1>USB_AD : usb_ad_ioctl copied first buf for PID %d\n", pid);
                            mutex_unlock(&dev->io_mutex);
                            if (too_slow == 1)
                                    return -USB_AD_CLIENT_TOO_SLOW;
                            return retval;
                    }
                    if (gpClients_array[i]->second_buf.full) {
                            printk("<1>USB_AD : usb_ad_ioctl going to copy second buf for PID %d\n", pid);
                            retval = buffer_copy_to_user(&(gpClients_array[i]->second_buf),
                                (char *)ioctl_param,gpClients_array[i]->second_buf.size);
                            printk("<1>USB_AD : usb_ad_ioctl copied second buf for PID %d\n", pid);
                            mutex_unlock(&dev->io_mutex);
                            if (too_slow == 1)
                                    return -USB_AD_CLIENT_TOO_SLOW;
                            return retval;
                    }
                    printk("<1>USB_AD : ad_ioctl GET DATA OK\n");
                    break;
            default:
                    printk("<1>USB_AD: TO NIE MOZE SIE ZDAZYC (AKURAT)\n");
        }
        mutex_unlock(&dev->io_mutex);
        return 0;
}
//--------------------------------------------------------------------IOCTL_end

static const struct file_operations ad_fops = {
        .owner =        THIS_MODULE,
        .read =         ad_read,
        .write =        ad_write,
        .ioctl =        ad_ioctl,
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

        //unsigned char test_channels[USB_AD_CHANNELS_NUM] = {1,0,0,0,0,0,1};

        printk("<1>USB_AD : ad_probe executed\n");
        /* allocate memory for our device state and initialize it */
        dev = kzalloc(sizeof(*dev), GFP_KERNEL);
        if (!dev) {
                err("Out of memory\n");
                goto error;
        }
        kref_init(&dev->kref);
        sema_init(&dev->limit_sem, USB_AD_WRITES_IN_FLIGHT);
        mutex_init(&dev->io_mutex);
        spin_lock_init(&dev->err_lock);
        init_usb_anchor(&dev->submitted);
        init_completion(&dev->bulk_in_completion);

        dev->udev = usb_get_dev(interface_to_usbdev(interface));
        dev->interface = interface;

        /*printk("<1>USB_AD : ad_probe : num of configurations is %d\n",
            dev->udev->descriptor.bNumConfigurations);
        printk("<1>USB_AD : ad_probe : conf_id : %d\n",
            dev->udev->actconfig->desc.bConfigurationValue);*/

        /* zmiana konfiguracji na wlasciwa (i 'reset' fcji probe) */
        if (dev->udev->actconfig->desc.bConfigurationValue == USB_AD_BOOT_CONF) {
                retval = usb_driver_set_configuration(dev->udev, USB_AD_ACTIVE_CONF);
                retval = retval ? retval : -ENODEV;
                goto error;
        }

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
                        printk("<1>USB_AD : ad_probe bulk in size is: %d\n",
                            buffer_size);
                        dev->bulk_in_endpointAddr = endpoint->bEndpointAddress;
                        dev->bulk_in_buffer = kmalloc(buffer_size, GFP_KERNEL);
                        if (!dev->bulk_in_buffer) {
                                err("Could not allocate bulk_in_buffer\n");
                                goto error;
                        }
                        dev->bulk_in_urb = usb_alloc_urb(0, GFP_KERNEL);
                        if (!dev->bulk_in_urb) {
                                err("Could not allocate bulk_in_urb\n");
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
                err("Could not find both bulk-in and bulk-out endpoints\n");
                goto error;
        }

        /* save our data pointer in this interface device */
        usb_set_intfdata(interface, dev);

        /* we can register the device now, as it is ready */
        retval = usb_register_dev(interface, &ad_class);
        if (retval) {
                /* something prevented us from registering this driver */
                err("Not able to get a minor for this device.\n");
                usb_set_intfdata(interface, NULL);
                goto error;
        }

        /* let the user know what node this device is now attached to */
        dev_info(&interface->dev,
                 "USB AD device now attached to USB_AD-%d\n",
                 interface->minor);

        /* alokuj i zeruj pamiec dla tablicy wskaznikow na klientow */
        gpClients_array = kzalloc(sizeof(Client*) * USB_AD_MAX_CLIENTS_NUM, GFP_KERNEL);
        if (gpClients_array == NULL)
            err("usb_probe failed. Error number %d\n", USB_AD_ERROR_ALLOCATION);

        /* alokuj pamiec dla testowego klienta */
        /*gpClients_array[0] = kmalloc(sizeof(Client), GFP_KERNEL);
        if (gpClients_array[0] == NULL)
            err("usb_probe failed. Error number %d\n", USB_AD_ERROR_ALLOCATION);


        init_client(gpClients_array[0], 555, usb_ad_fq(), 10, test_channels);
        */
        printk("<1>USB_AD : ad_probe successful!\n");
        return 0;

error:
        if (dev)
                /* this frees allocated memory */
                kref_put(&dev->kref, ad_delete);
        printk("<1>USB_AD : ad_probe failed\n");
        if (retval == 0)
            retval = -ENODEV;
        return retval;
}

static void ad_disconnect(struct usb_interface *interface)
{
        struct usb_ad *dev;
        int minor = interface->minor;
        printk("<1>USB_AD : ad_disconnect executed\n");

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

        dev_info(&interface->dev, "USB_AD #%d now disconnected\n", minor);
}

static void ad_draw_down(struct usb_ad *dev)
{
        int time;
        printk("<1>USB_AD : ad_draw_down executed\n");
        time = usb_wait_anchor_empty_timeout(&dev->submitted, 1000);
        if (!time)
                usb_kill_anchored_urbs(&dev->submitted);
        usb_kill_urb(dev->bulk_in_urb);
}

static int ad_suspend(struct usb_interface *intf, pm_message_t message)
{
        struct usb_ad *dev = usb_get_intfdata(intf);
        printk("<1>USB_AD : ad_suspend executed\n");

        if (!dev)
                return 0;
        ad_draw_down(dev);
        return 0;
}

static int ad_resume(struct usb_interface *intf)
{
        printk("<1>USB_AD : ad_resume executed\n");
        return 0;
}

static int ad_pre_reset(struct usb_interface *intf)
{
        struct usb_ad *dev = usb_get_intfdata(intf);
        printk("<1>USB_AD : ad_pre_reset executed\n");
        mutex_lock(&dev->io_mutex);
        ad_draw_down(dev);

        return 0;
}

static int ad_post_reset(struct usb_interface *intf)
{
        struct usb_ad *dev = usb_get_intfdata(intf);
        printk("<1>USB_AD : ad_post_reset executed\n");
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
        printk("<1>USB_AD : usb_ad_init executed\n");
        /* register this driver with the USB subsystem */
        result = usb_register(&ad_driver);
        if (result)
                err("usb_register failed. Error number %d\n", result);

        printk("<1>USB_AD : usb_ad_init successful\n");
        return result;
}

static void __exit usb_ad_exit(void)
{
        int i;
        printk("<1>USB_AD : usb_ad_exit executed\n");
        /* usuniecie struktur klientow */
        if (gpClients_array) {
                for (i = 0; i < USB_AD_MAX_CLIENTS_NUM; i ++)
                        if (gpClients_array[i]) {
                                wake_up(&gpClients_array[i]->queue);
                                kfree(gpClients_array[i]);
                                gpClients_array[i] = NULL;
                                printk("<1>USB_AD : usb_ad_exit removed a client\n");
                        }
                kfree(gpClients_array);
                gpClients_array = NULL;
                printk("<1>USB_AD : usb_ad_exit removed the Clients array\n");
        }
        /* deregister this driver with the USB subsystem */
        usb_deregister(&ad_driver);
}

module_init(usb_ad_init);
module_exit(usb_ad_exit);

MODULE_LICENSE("GPL");
