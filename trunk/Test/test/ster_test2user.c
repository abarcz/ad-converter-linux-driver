//Autor: Aleksy Barcz
#include <stdio.h>
#include <unistd.h>     //read, write, close..
#include <fcntl.h>      //open, O_RDWR
#include <stdlib.h>     //exit
#include <sys/ioctl.h>
#include "ster_test2.h"

int main() { 
  unsigned char byte;
  int ret_val;
  char buffer[BUFFER_SIZE];
  
  //Opening the device ster
  printf("Ready to open\n");
  fflush(stdout);
     
  int STER=open("/dev/ster_test20", O_RDWR);
  if(STER==-1) {
     printf("Cannot open device...\n");
     fflush(stdout);
     exit(1);
   }
   printf("Opened device\n");
     fflush(stdout);
  
  byte=1;
  
  read(STER,&byte,1);
  printf("Read character: %d\n", byte);
  
  byte=4;
  write(STER,&byte,1);
  printf("Wrote character: %d\n", byte);
  byte=1;
  read(STER,&byte,1);
  printf("Read character: %d\n", byte);

  printf("Started ioctl\n");
  ret_val = ioctl(STER, IOCTL_ZERO_BYTE, buffer);
  printf("Read using ioctl character: %d\n", buffer[0]);
  
  ret_val = ioctl(STER, IOCTL_FIRST_BYTE, buffer);
  printf("Read using ioctl character: %d\n", buffer[0]);
  
  buffer[0] = 0;     //index
  ret_val = ioctl(STER, IOCTL_READ_NTH_BYTE, buffer);
  printf("Read using ioctl character: %d\n", buffer[0]);
  
  buffer[0] = 1;
  ret_val = ioctl(STER, IOCTL_READ_NTH_BYTE, buffer);
  printf("Read using ioctl character: %d\n", buffer[0]);
  
  buffer[0] = 2;
  ret_val = ioctl(STER, IOCTL_READ_NTH_BYTE, buffer);
  printf("Read using ioctl character: %d\n", buffer[0]);
  
  buffer[0] = 2;     //index
  buffer[1] = 5;     //value to be written in driver's buffer
  ret_val = ioctl(STER, IOCTL_WRITE_NTH_BYTE, buffer);
 
  //using the same index... 
  ret_val = ioctl(STER, IOCTL_READ_NTH_BYTE, buffer);
  printf("Read using ioctl character: %d\n", buffer[0]);
  
  close(STER);
  printf("Closed device\n");
  return 0;
}
