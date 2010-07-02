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
        //unsigned char byte;
        char stext[5] = {'a','b','c', '\n', '\0'};
        printf("%s",stext);
        int pid = getpid();  
        int ret_val,i;
        char buffer[BUFFER_SIZE];
        //Opening the device
        printf("Ready to open\n");
        fflush(stdout);
             
        int usb_ad = open("/dev/USB_AD0", O_RDWR);
        if (usb_ad == -1) {
                printf("Cannot open device...\n");
                fflush(stdout);
                exit(1);
        }
        printf("Opened device\n");
        fflush(stdout);
        //buduje zapytanie  magic number explicite
        ((int *)buffer)[0]=474747;  
        ((int *)buffer)[1]=pid;
        //fq
        ((int *)buffer)[2]=127;    
        //wielkosc paczki
        ((int *)buffer)[3]=50;
        //kanaly
        ((int *)buffer)[4]=1;   
        ((int *)buffer)[5]=0;  
        ((int *)buffer)[6]=0;
        ((int *)buffer)[7]=0;  
        ((int *)buffer)[8]=0;  
        ((int *)buffer)[9]=0;  
        ((int *)buffer)[10]=0;  
        printf("Started ioctl: fq 127 1 kanal\n");
        ret_val = ioctl(usb_ad, IOCTL_SET_PARAMS, buffer);
        printf("ioctl odpowiedx: %d\n", ret_val);
          
        printf("Wysyłam prosby o dane\n");
        while(1){
                ((int *)buffer)[0]=474747;  
                ((int *)buffer)[1]=pid;
         
                ret_val = ioctl(usb_ad, IOCTL_GET_DATA, buffer);
                printf("ioctl odpowiedx: %d\n", ret_val);
                printf("Zawartosc bufora [1bajt,2bajt\n]");
                for (i = 0; i < 50; i++) {
                        printf("%d,%d\n",
                                        buffer[i],
                                        buffer[i+1]);
                        }
                }
          

        //using the same index... 
        sleep(2);
        close(usb_ad);
        printf("Closed device\n");
        return 0;
}
