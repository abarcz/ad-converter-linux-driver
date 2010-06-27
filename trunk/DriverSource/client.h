#ifndef USB_AD_CLIENT
#define USB_AD_CLIENT

#include "constants.h"
#include "client_buffer.h"

typedef struct Client Client;
struct Client {
        int pid;
        int requested_fq;                           //zadana czestotliwosc
        int buffer_size;                            //wielkosc paczki - ilosc probek
        unsigned char channels[USB_AD_CHANNELS_NUM];    //ktore kanaly program chce probkowac
        int buffer_full_number;                     //ktory bufor jest gotowy do odczytu (1/2/NA)
        struct Client_buffer first_buf;            
        struct Client_buffer second_buf;
        //wait_queue_head_t queue;
        //to tez nie dziala DECLARE_WAIT_QUEUE_HEAD(queue);
};

/* inicjalizacja nowo utworzonej struktury klienta */
int init_client(Client *client, int pid, int requested_fq, int buffer_size, unsigned char channels[USB_AD_CHANNELS_NUM]);

/**** poczatek pliku .c ****/  
int init_client(Client *client, int pid, int requested_fq, int buffer_size, unsigned char channels[USB_AD_CHANNELS_NUM]) {
        int channels_count,i,ret_val;
        int byte_buffer_size;
        printk("<1>USB_AD : entered init_client\n");
        if(client == NULL)
                return USB_AD_ERROR_NULL;
        if(requested_fq <= 0)
                return USB_AD_ERROR_VALUE;
        channels_count = 0;             //ilosc kanalow, ktore chce czytac klient
        for(i = 0; i < USB_AD_CHANNELS_NUM; i++) {
                if(channels[i])
                        channels_count++;
        }
        byte_buffer_size = buffer_size * channels_count * 2;
        if((byte_buffer_size <= 0 )|| (byte_buffer_size >= USB_AD_CLIENT_BUFFER_MAX_SIZE))
                return USB_AD_ERROR_VALUE;
        //Stworzenie kolejki - cos robie xle chyba
        //client->queue = __WAIT_QUEUE_HEAD_INITIALIZER(queue); 
        //:TODO: obliczanie zaleznosci czestotliwosci klienta od czestotliwosci probkowania, zamiast avg_divisor == 1
        ret_val = buffer_init(&(client->first_buf), channels_count * 2, buffer_size, 1);
        if(ret_val != 0)
                return ret_val;
        ret_val = buffer_init(&(client->second_buf), channels_count * 2, buffer_size, 1);
        if(ret_val != 0)
                return ret_val;
        
        client->pid = pid;
        client->requested_fq = requested_fq;
        client->buffer_size = buffer_size;
        for(i = 0; i < USB_AD_CHANNELS_NUM; i++)
                client->channels[i] = channels[i];
        client->buffer_full_number = -1;
        printk("<1>USB_AD : init_client successfully completed\n");
        return 0;
}
#endif
    
