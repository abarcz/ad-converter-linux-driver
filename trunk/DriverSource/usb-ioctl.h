#ifndef USB_AD_IOCTL
#define USB_AD_IOCTL
#include <linux/ioctl.h>
/*
 *Bo tak?
 */
#define MAJOR_NUM 100
#define USB_AD_IOCTL_MAGIC_NUMBER 474747

/*
 *Ustawiamy co i z jaka czestotliwoscia ma probkowac dany proces
 *param - 'wskaznik' na tablice int[11] 
 *{magic,pid,fq,buff_size,czy chan1,czy chan2,...}
 *zwraca true fq czyli maxfq/n n-co ile probek usredniamy
 */
#define IOCTL_SET_PARAMS _IOR(MAJOR_NUM,0, int*)

/*
 *Gwoxdx programu 
 *Użytkownik w paramie przekazuje wskaznik na bufor w ktorym
 *najpierw jest int magic number
 *potem jako 2 int jest pid
 *zwraca sukces ie 0 albo kod błędu
 */
#define IOCTL_GET_DATA _IOWR(MAJOR_NUM,1, char*)



#endif /*USB_AD_IOCTL*/
