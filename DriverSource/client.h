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
        struct Client_buffer first_buf;
        struct Client_buffer second_buf;
        wait_queue_head_t queue;
        bool warning;                               //ostrzezenie - klient nie nadaza z odbieraniem
        int loops_after_warn_count;                 //ile petli minelo od ostrzezenia klienta
};

int double_round(long a, long b);
/* inicjalizacja nowo utworzonej struktury klienta */
int init_client(Client *client, int pid, unsigned int requested_fq, int buffer_size, unsigned char channels[USB_AD_CHANNELS_NUM]);

/* usuwa klienta wraz z zawartoscia */
void remove_client(Client *client);

/* z pelnego ciagu bajtow (source) wybiera wartosci odpowiadajace zadanym kanalom, wpisuje do dest
 * size - rozmiar bufora source
 */
int choose_bytes(Client *client, unsigned char *source, unsigned char *dest, int size);

/* wypisuje na ekran zawartosc buforow klienta, rzedami */
int print_client_buffers(Client *client);

/**** poczatek pliku .c ****/  
int double_round(long a, long b) {
        int ret_val = 0;
        if (a/b - (int)a/b >= 0.5)
                ret_val = (int)a/b + 1;
        else
                ret_val = (int)a/b;
        return ret_val;
}

int init_client(Client *client, int pid, unsigned int requested_fq, int buffer_size, unsigned char channels[USB_AD_CHANNELS_NUM]) 
{
        int channels_count,i,ret_val;
        int byte_buffer_size;
        int avg_divisor = 1;
        printk("<1>USB_AD : entered init_client\n");
        if (client == NULL)
                return USB_AD_ERROR_NULL;
        if ((requested_fq <= 0) || (requested_fq > usb_ad_fq()))
                return USB_AD_ERROR_VALUE;
        channels_count = 0;             //ilosc kanalow, ktore chce czytac klient
        for (i = 0; i < USB_AD_CHANNELS_NUM; i++) {
                if (channels[i])
                        channels_count++;
        }
        printk("<1>USB_AD : init_client pid = %d requested_fq = %d buffer_size = %d channels_count = %d\n", 
            pid,
            requested_fq,
            buffer_size,
            channels_count);
        client->channels_count = channels_count;
        byte_buffer_size = buffer_size * channels_count * 2;
        if ((byte_buffer_size <= 0 )|| (byte_buffer_size >= USB_AD_CLIENT_BUFFER_MAX_SIZE))
                return USB_AD_ERROR_VALUE;
        //Stworzenie kolejki 
        init_waitqueue_head(&client->queue);
        avg_divisor =  (int)(usb_ad_fq() / requested_fq);
        ret_val = buffer_init(&(client->first_buf), channels_count * 2, buffer_size, avg_divisor);
        if (ret_val != 0)
                return ret_val;
        ret_val = buffer_init(&(client->second_buf), channels_count * 2, buffer_size, avg_divisor);
        if (ret_val != 0)
                return ret_val;
        
        client->pid = pid;
        client->requested_fq = requested_fq;
        client->buffer_size = buffer_size;
        for (i = 0; i < USB_AD_CHANNELS_NUM; i++)
                client->channels[i] = channels[i];
        client->warning = 0;
        client->loops_after_warn_count = 0;
        printk("<1>USB_AD : init_client successfully completed\n");
        return 0;
        
}

void remove_client(Client *client)
{
    if (client)
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

int print_client_buffers(Client *client) {
        int i, j;
        int row_size;
        unsigned char *buf;
        if ((!client) || (!client->first_buf.buf) || (!client->second_buf.buf))
                return USB_AD_ERROR_NULL;
        printk("<1>USB_AD : printing buffers for client with PID = %d\n", client->pid);
        row_size = client->first_buf.row_size;
        buf = client->first_buf.buf;
        for (i = 0; i < client->buffer_size; i++) {
                for (j = 0; j < row_size; j++)
                        printk("%d ", buf[i * row_size + j]);
                printk("\n");
        }
        printk("\n");
        buf = client->second_buf.buf;
        for (i = 0; i < client->buffer_size; i++) {
                for (j = 0; j < row_size; j++)
                        printk("%d ", buf[i * row_size + j]);
                printk("\n");
        }
        return 0;
}
#endif
    
