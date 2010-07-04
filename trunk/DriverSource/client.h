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
        int ready_for_input;                        //czy jest gotowy na przyjmowanie danych
        bool use_first_buf;                         /* czy zapis ma sie odbywac do pierwszego bufora
                                                     * jesli 0 => zapis do drugiego bufora 
                                                     */ 
};

int round_div(const unsigned int a, const unsigned int b);
/* inicjalizacja nowo utworzonej struktury klienta */
int init_client(Client *client, int pid, const unsigned int requested_fq, const int buffer_size, const unsigned char channels[USB_AD_CHANNELS_NUM]);

/* usuwa klienta wraz z zawartoscia */
void remove_client(Client *client);

/* z pelnego ciagu bajtow (source) wybiera wartosci odpowiadajace zadanym kanalom, wpisuje do dest
 * size - rozmiar bufora source
 */
int choose_bytes(Client *client, unsigned char *source, unsigned char *dest, const int size);

/* wypisuje na ekran zawartosc buforow klienta, rzedami */
int print_client_buffers(Client *client);

/**** poczatek pliku .c ****/  
int round_div(const unsigned int a, const unsigned int b) {
        long amult, div; 
        unsigned int lsnum, retval;

        amult = a * 10;
        div = amult / b;
        lsnum = div % 10;
        div = div / 10;
        if (lsnum >= 5)
            retval = (unsigned int) div + 1;
        else
            retval = (unsigned int) div;
        return retval;
} 

int init_client(Client *client, int pid, const unsigned int requested_fq, const int buffer_size, const unsigned char channels[USB_AD_CHANNELS_NUM])
{
        int channels_count,i,ret_val;
        int byte_buffer_size;
        int avg_divisor = 1;
        //printk("<1>USB_AD : entered init_client\n");
        if (client == NULL)
                return -EINVAL;
        if ((requested_fq <= 0) || (requested_fq > usb_ad_fq()))
                return -EINVAL;
        client->ready_for_input = 0;
        channels_count = 0;             //ilosc kanalow, ktore chce czytac klient
        for (i = 0; i < USB_AD_CHANNELS_NUM; i++) {
                if (channels[i])
                        channels_count++;
        }
        /*printk("<1>USB_AD : init_client pid = %d requested_fq = %d buffer_size = %d channels_count = %d\n", 
            pid,
            requested_fq,
            buffer_size,
            channels_count);*/
        client->channels_count = channels_count;
        byte_buffer_size = buffer_size * channels_count * 2;
        if ((byte_buffer_size <= 0 )|| (byte_buffer_size >= USB_AD_CLIENT_BUFFER_MAX_SIZE))
                return -EINVAL;
        //Stworzenie kolejki 
        init_waitqueue_head(&client->queue);
        avg_divisor =  round_div(usb_ad_fq(), requested_fq);
        printk("<1>USB_AD : init_client usb_fq : %d req_fq: %d avg_divisor : %d\n", usb_ad_fq(), requested_fq, avg_divisor);
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
        client->use_first_buf = 1;
        //printk("<1>USB_AD : init_client successfully completed\n");
        return 0;
        
}

void remove_client(Client *client)
{
    if (!client)
        return;
    remove_buffer(&client->first_buf);
    remove_buffer(&client->second_buf);
    kfree(client);
}

int choose_bytes(Client *client, unsigned char *source, unsigned char *dest, const int size)
{
    int i;
    int pos_source = 2;
    int pos_dest = 0;
    if ((!client) || (!source) || (!dest)) {
            printk("<1>USB_AD choose_bytes error(1)\n");
            return -EINVAL;
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
                    return -EINVAL;
    }
    return 0;
}

int print_client_buffers(Client *client) {
        int i, j;
        int row_size;
        unsigned char *buf;
        if ((!client) || (!client->first_buf.buf) || (!client->second_buf.buf))
                return -EINVAL;
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
    
