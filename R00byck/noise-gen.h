/*
 * USB AVR Noise Generator Driver - HEADER FILE
 *
 * Plik zawiera definicje struktur niezbêdne do poprawnego dzia³ania sterownika.
 * 
 * Rzecz opiera siê na pomys³ach z usb-skeleton.c
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


#ifndef _NOISEGEN_H
#define _NOISEGEN_H

/* === makrodefinicje === */
#define NOISE_VENDOR_ID 	0x16c0
#define NOISE_PRODUCT_ID 	0x05dc
#define NOISE_MINOR		192
#define BUF_SIZE		64
#define BIG_BUF_SIZE		1024
#define to_noise_dev(d)		container_of(d, struct usb_noise, kref)


/* === prototypy === */
int noise_probe (struct usb_interface *intf, const struct usb_device_id *id);
void noise_disconnect (struct usb_interface *intf);
static void noise_delete (struct kref* kref);
static int noise_open (struct inode* inode, struct file* file);
static int noise_release (struct inode* inode, struct file* file);
static ssize_t noise_read (struct file* file, char* buffer, 
		size_t count, loff_t* ppos);
static void noise_urb_callback (struct urb *urb);

/* === definicje struktur === */
static struct usb_device_id noise_gen_table [] = {
	{ USB_DEVICE(NOISE_VENDOR_ID, NOISE_PRODUCT_ID) } ,
	{ }
};

static struct usb_driver noise_usb_driver = {
	.name = "noise-gen",
	.id_table = noise_gen_table,
	.probe = noise_probe,
	.disconnect = noise_disconnect,
};

static const struct file_operations noise_fops = {
	.owner = THIS_MODULE,
	.open = noise_open,
	.release = noise_release,
	.read = noise_read,
};

static struct usb_class_driver noise_class = {
	.name = "noise-gen%d",
	.fops = &noise_fops,
	.minor_base = NOISE_MINOR,
};

struct usb_noise {
	struct usb_device	*udev;
	struct usb_interface	*interface;
	unsigned char		*buffer;
	size_t			buffer_size;
	__u8			endpoint_addr;
	int			err;
	int			open_count;
	struct kref		kref;
	struct mutex		io_mutex;
};

#endif 
