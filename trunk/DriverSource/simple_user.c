//Autor: Aleksy Barcz
#define BUFFER_SIZE 1024

#include <stdio.h>
#include <unistd.h>     //read, write, close..
#include <fcntl.h>      //open, O_RDWR
#include <stdlib.h>     //exit
#include <unistd.h>     //getpid
#include <sys/ioctl.h>
#include "usb-ioctl.h"

int main()
{
        int pid = getpid();
        int ret_val,i;
        char buffer[BUFFER_SIZE];
        //Opening the device
        printf("Ready to open\n");
        fflush(stdout);

        int usb_ad = open("/dev/USB_AD0", O_RDWR);
        if(usb_ad == -1) {
                printf("Cannot open device...\n");
                fflush(stdout);
                exit(1);
        }
        printf("Opened device\n");
        fflush(stdout);

        sleep(2);
        close(usb_ad);
        printf("Closed device\n");
        return 0;
}
