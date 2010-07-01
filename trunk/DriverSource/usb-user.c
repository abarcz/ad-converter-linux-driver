#define BUFFER_SIZE 14336

#include <stdio.h>
#include <unistd.h>     //read, write, close..
#include <fcntl.h>      //open, O_RDWR
#include <stdlib.h>     //exit
#include <unistd.h>     //getpid
//#include <conio.h>      //kbhit
#include <sys/ioctl.h>
#include <sys/select.h>
//#include <termios.h>
#include "usb-ioctl.h"
//Kazdy IOCLT musi miec taki poczatek w buforze
int kbhit()
{
        struct timeval tv = { 0L, 0L };
        fd_set fds;
        FD_SET(0, &fds);
        return select(1, &fds, NULL, NULL, &tv);
}

int getch()
{
        int r;
        unsigned char c;
        if ((r = read(0, &c, sizeof(c))) < 0) {
                return r;
                } 
        else {
                return c;
                }
}




void set_beginning(char *bufor, int pid)
{
        ((int *)bufor)[0]=USB_AD_IOCTL_MAGIC_NUMBER;  
        ((int *)bufor)[1]=pid;
        ((int *)bufor)[2]=0;
        ((int *)bufor)[3]=0;
        ((int *)bufor)[4]=0;
        ((int *)bufor)[5]=0;
        ((int *)bufor)[6]=0;
        ((int *)bufor)[7]=0;
        ((int *)bufor)[8]=0;
        ((int *)bufor)[9]=0;
        ((int *)bufor)[10]=0;
}


void params(char *buffer,int pid){
        char str[32];
        int i;
        printf("Podaj parametry probkowania\nPodaj czestotliwosc:\n");
        set_beginning(buffer, pid);
        scanf("%d",&((unsigned int *)buffer)[2]);
        //((unsigned int*)buffer)[2] = 1638;
        printf("Podaj ilosc probek zwracanych przez sterownik:\n");
        scanf("%d",&((unsigned int *)buffer)[3]);
        //((int*)buffer)[3] = 10;
        printf("Podaj numery kanalow (1-7) z ktorych bedza wysylane probki:\n");
        scanf("%s",str);
        for (i = 0; i < 32; i++){
                if(str[i]=='1'){
                        ((int *)buffer)[4] = 1;
                        }
                if(str[i]=='2'){
                        ((int *)buffer)[5] = 1;
                        }
                if(str[i]=='3'){
                        ((int *)buffer)[6] = 1;
                        }
                if(str[i]=='4'){
                        ((int *)buffer)[7] = 1;
                        }
                if(str[i]=='5'){
                        ((int *)buffer)[8] = 1;
                        }
                if(str[i]=='6'){
                        ((int *)buffer)[9] = 1;
                        }
                if(str[i]=='7'){
                        ((int *)buffer)[10] = 1;
                        }
                }
}

int param_send(char *buffer, int usb_ad){
        int ret_val;
        printf("Staram sie wyslac IOCTLa z parametrami fq: %d size: %d channels: %d %d %d %d %d %d %d\n",
        ((int *)buffer)[2],
        ((int *)buffer)[3],
        ((int *)buffer)[4],   
        ((int *)buffer)[5],  
        ((int *)buffer)[6],
        ((int *)buffer)[7],  
        ((int *)buffer)[8],  
        ((int *)buffer)[9], 
        ((int *)buffer)[10] 
        );
        ret_val = ioctl(usb_ad, IOCTL_SET_PARAMS, buffer);
        printf("Odpowiedx: %d\n",ret_val);
        return ret_val;
}

int get_data(char *buffer, int pid, int usb_ad, int block_size, int channel_count){
        int i,j,ret_val;
        set_beginning(buffer, pid);
        ret_val = ioctl(usb_ad, IOCTL_GET_DATA, buffer);
        
        for (i = 0; i < block_size * channel_count * 2 - 1; i+=2) {
                printf("%d,%d  ",
                        buffer[i],
                        buffer[i+1]);
                j++;
                if(j == channel_count){
                        printf("\n");
                        j = 0;
                        }
                }
        return 0;
}

int main(int argc, char **argv) { 
        int pid = getpid();  
        int ret_val,i,j=0;
        int channel_count=0;
        int block_size=0;
        char buffer[BUFFER_SIZE];
        char str[32];
        char pom[7];
        int debcount = 0;
        //Opening the device
        //ROTFL for(i = 0;i < argc; i++)printf("%d\n",atoi(argv[i]));
        printf("Proba otwarcia urzadzenia...\n");
        int usb_ad = open("/dev/USB_AD0", O_RDWR);
        if (usb_ad == -1) {
                printf("Nie powiodla sie\n");
                exit(1);
                }
        printf("Powiodla sie\n");
        if (argc == 10){         
                set_beginning(buffer, pid);
                //!!!!!!nie sprawdzamy fq - pogadac z Aleksem
                ((int *)buffer)[2]=atoi(argv[1]);    
                ((int *)buffer)[3]=atoi(argv[2]);
                //kanaly
                for (i = 3; i < 10; i++)
                        if(atoi(argv[i]) == 1){
                                ((int *)buffer)[i+1] = 1;
                                }
                        else{
                                ((int *)buffer)[i+1] = 0;
                                }
                }
        else{
                params(buffer, pid);
                }
        ret_val = param_send(buffer, usb_ad);
        printf("IOCTL odpowiedzial: %d\n", ret_val);
        //Liczymy z ilu kanalow zbieramy informacje (zeby sensowanie wyswietlac)
        for (i = 0; i < 7; i++)
                if (((int *)buffer)[i+4] == 1)
                        channel_count++;
        block_size = ((int *)buffer)[3];
        
        if (ret_val > -1){
                printf("Wysyłam prosby o dane\n");
                while((!kbhit())){
                        ret_val = get_data(buffer,pid,usb_ad,block_size,channel_count);
                        }
                }
        //wyczyszczenie bufora zaznkow
        while(getch()!=10)
                /*DONOTHING*/;
        int loop = 1;
        int dialog = 1;
        while(loop == 1){
                while(dialog == 1){
                        printf("Co teraz?\n1-probkuj dalej\n2-podaj nowe parametry\n3-zakoncz prace programu\n");
                        scanf("%d",((int *)str));
                        if(str[0] == 1 || str[0] == 2 || str[0] == 3)
                                dialog = 0;
                        }
                if (str[0] == 1){
                        printf("Wysyłam prosby o dane\n");
                        while((!kbhit()))
                                ret_val = get_data(buffer,pid,usb_ad,block_size,channel_count);
                        while(getch()!=10)/*DONOTHING*/;
                        }
                if (str[0] == 2){
                        params(buffer,pid);
                        param_send(buffer,pid);
                        }
                dialog = 1;
                if (str[0] == 3){
                        loop = 0;
                        }
                }

        
        //sleep(2);
        close(usb_ad);
        printf("Urzadzenie zamkniete\n");
        return 0;
}
