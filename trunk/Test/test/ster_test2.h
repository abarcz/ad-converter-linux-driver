//Autor: Aleksy Barcz
#define DEVICE_NAME "ster_test2"
#define BUFFER_SIZE 128			//in bytes
#define MAJOR_NUM 60
/* 
 * Set the message of the device driver 
 */
#define IOCTL_ZERO_BYTE _IOR(MAJOR_NUM, 0, char *)
#define IOCTL_FIRST_BYTE _IOR(MAJOR_NUM, 1, char *)
#define IOCTL_READ_NTH_BYTE _IOR(MAJOR_NUM, 2, char *)
#define IOCTL_WRITE_NTH_BYTE _IOR(MAJOR_NUM, 3, char *)

/*
 * _IOR means that we're creating an ioctl command 
 * number for passing information from a user process
 * to the kernel module. 
 *
 * The first arguments, MAJOR_NUM, is the major device 
 * number we're using.
 *
 * The second argument is the number of the command 
 * (there could be several with different meanings).
 *
 * The third argument is the type we want to get from 
 * the process to the kernel.
 */

