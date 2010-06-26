#ifndef USB_AD_IOCTL
#define USB_AD_IOCTL
#include <linux/ioctl.h>
/*
 *Bo tak?
 */
#define MAJOR_NUM 100

/*
 *Ustawiamy co i z jaka czestotliwoscia ma probkowac dany proces
 *param - 'wskaznik' na tablice int[9] {pid,fq,czy chan1,czy chan2,...}
 *zwraca true fq czyli maxfq/n n-co ile probek usredniamy
 */
#define IOCTL_SET_PARAMS _IOR(MAJOR_NUM,0, int*)

/*
 *Gwoxdx programu 
 *Użytkownik w paramie przekazuje wskaznik na bufor w ktorym jest pid na samym
 *poczatku 
 *zwraca sukces ie 0 albo kod błędu
 */
#define IOCTL_GET_DATA _IOWR(MAJOR_NUM,1, char*)



#endif /*USB_AD_IOCTL*/

