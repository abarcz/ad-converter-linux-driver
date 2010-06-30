#ifndef USB_AD_CLIENT
#define USB_AD_CLIENT

#include "constants.h"
#include "client_buffer.h"

typedef struct Client Client;
struct Client {
        int pid;
        int requested_fq;                           //zadana czestotliwosc
        int buffer_size;                            //wielkosc bufora (jednorazowej paczki) - ilosc probek
        unsigned char channels[USB_AD_CHANNELS_NUM];    //ktore kanaly program chce probkowac
        int channels_count;                         //ile kanalow chce probkowac
        int buffer_full_number;                     //ktory bufor jest gotowy do odczytu (1/2/NA)
        struct Client_buffer first_buf;            
        struct Client_buffer second_buf;
        wait_queue_head_t queue;
        bool warning;                               //ostrzezenie - klient nie nadaza z odbieraniem
        int loops_after_warn_count;                 //ile petli minelo od ostrzezenia klienta
};

/* inicjalizacja nowo utworzonej struktury klienta */
int init_client(Client *client, int pid, int requested_fq, int buffer_size, unsigned char channels[USB_AD_CHANNELS_NUM]);

/* usuwa klienta wraz z zawartoscia */
void remove_client(Client *client);

/* z pelnego ciagu bajtow (source) wybiera wartosci odpowiadajace zadanym kanalom, wpisuje do dest
 * size - rozmiar bufora source
 */
int choose_bytes(Client *client, unsigned char *source, unsigned char *dest, int size);

/**** poczatek pliku .c ****/  
int init_client(Client *client, int pid, int requested_fq, int buffer_size, unsigned char channels[USB_AD_CHANNELS_NUM]) 
{
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
        printk("<1>USB_AD : init_client pid = %d requested_fq = %d buffer_size = %d channels_count = %d\n", 
            pid,
            requested_fq,
            buffer_size,
            channels_count);
        client->channels_count = channels_count;
        byte_buffer_size = buffer_size * channels_count * 2;
        if((byte_buffer_size <= 0 )|| (byte_buffer_size >= USB_AD_CLIENT_BUFFER_MAX_SIZE))
                return USB_AD_ERROR_VALUE;
        //Stworzenie kolejki 
        init_waitqueue_head(&client->queue);
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
        client->warning = 0;
        client->loops_after_warn_count = 0;
        printk("<1>USB_AD : init_client successfully completed\n");
        return 0;
        
}

void remove_client(Client *client)
{
    if(client)
        kfree(client);
}

int choose_bytes(Client *client, unsigned char *source, unsigned char *dest, int size)
{
    int i;
    int pos_source = 2;
    int pos_dest = 0;
    if ((!client) || (!source) || (!dest)) {
            printk("<1>USB_AD choose_bytes error(1)\n");
            return USB_AD_ERROR_NULL;
    }
    
    for (i = 0; i < USB_AD_CHANNELS_NUM; i++) {
            if (pos_source >= size) {
                    break;
            }
            if (client->channels[i] == 1) {
                    dest[pos_dest] = source[pos_source];
                    dest[pos_dest  + 1] = source[pos_source + 1];
                    pos_dest += 2;
            }
            pos_source += 2;
    }
    if (pos_dest != (client->channels_count * 2)) {
                    printk("<1>USB_AD choose_bytes error %d != %d\n",
                        pos_dest,
                        client->channels_count * 2);
                    return USB_AD_ERROR_VALUE;
    }
    return 0;
}
    
#endif
    
