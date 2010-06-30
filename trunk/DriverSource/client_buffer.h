#ifndef USB_AD_CLIENT_BUFFER
#define USB_AD_CLIENT_BUFFER


#include <linux/module.h>
#include <linux/kernel.h>       /* printk() */
#include <asm/uaccess.h>        /* copy_from/to_user */
#include "constants.h"

/* bufor nalezacy do klienta, acykliczny */
typedef struct Client_buffer Client_buffer;
struct Client_buffer {
        int row_size;                       //wielkosc pojedynczego wiersza bufora, w bajtach
        int row_count;                      //ilosc wierszy
        int size;                           //wielkosc bufora w bajtach = row_size * row_count
        unsigned char *buf;                 //bufor
        long curr_pos;                      //aktualna pozycja w buforze (numer wiersza)
        int curr_pos_times_added;           //do usredniania kilku wynikow
                                            //ile razy zostaly dosumowane dane do biezacego wiersza

        int avg_divisor;                    //z ilu porcji danych usredniamy
        unsigned char full;                 //bufor pelny, zerowane po odczycie (buffer_copy_to_user)
};

/* inicjalizacja nowo utworzonego bufora klienta */
int buffer_init(Client_buffer *buf, int row_size, int row_count, int avg_divisor);

/* zapis do bufora niecyklicznego
   zwraca 1, gdy zapis sie powiodl, a bufor zostal zapelniony
   zwraca 0, gdy zapis sie powiodl, a bufor nie zostal jeszcze zapelniony
*/
int buffer_write(Client_buffer *buf, unsigned char *data, int size);

/* kopiuje cala zawartosc bufora do tablicy */
int buffer_copy_to_user(Client_buffer *buf, unsigned char *data, int size);

/**** poczatek pliku .c ****/
/* inicjalizacja nowo utworzonego bufora klienta */
int buffer_init(Client_buffer *buf, int row_size, int row_count, int avg_divisor)
{
        int buffer_size;
        if(buf == NULL)
                return USB_AD_ERROR_NULL;
        if((row_size <= 0) || (row_count <= 0) || (avg_divisor <= 0))
                return USB_AD_ERROR_VALUE;
        printk("<1>USB_AD buffer_init row_size = %d, row_count = %d, avg_divisor = %d\n", row_size, row_count, avg_divisor);
        buffer_size = row_size * row_count;
        buf->buf = kmalloc(buffer_size, GFP_KERNEL);
        if(buf->buf == NULL)                //nie udalo sie zaalokowac pamieci dla bufora
                return USB_AD_ERROR_ALLOCATION;
        memset(buf->buf, 0, buffer_size);
        buf->row_size = row_size;
        buf->row_count = row_count;
        buf->size = buffer_size;
        buf->avg_divisor = avg_divisor;
        buf->curr_pos_times_added = 0;
        buf->curr_pos = 0L;
        buf->full = 0;
        return 0;
}

/* zapis do bufora niecyklicznego
   zwraca 1, gdy zapis sie powiodl, a bufor zostal zapelniony
   zwraca 0, gdy zapis sie powiodl, a bufor nie zostal jeszcze zapelniony
*/
int buffer_write(Client_buffer *buf, unsigned char *data, int size) {
        if((buf == NULL) || (buf->buf == NULL) || (data == NULL))
                return USB_AD_ERROR_NULL;
        if(buf->full)                       //bufor pelny, nie mozna pisac
                return USB_AD_ERROR_FULL;
        if(size != buf->row_size)           //rozmiar danych sie nie zgadza
                return USB_AD_ERROR_SIZE;
        if(buf->curr_pos >= buf->row_count) //nie powinno nigdy sie wydarzyc
                return USB_AD_ERROR_UNKNOWN;

        /* blok usredniania wartosci */
        if(buf->avg_divisor == 1) {
                memcpy(buf->buf + buf->curr_pos * buf->row_size, data, size);
                buf->curr_pos++;
        } else {
                //poki co wpisuje pierwsza wartosc i wcale nie usrednia
                if(buf->curr_pos_times_added == 0)
                        memcpy(buf->buf + buf->curr_pos * buf->row_size, data, size);
                buf->curr_pos_times_added++;
                if(buf->curr_pos_times_added == buf->avg_divisor) { //koniec usredniania
                        buf->curr_pos_times_added = 0;
                        buf->curr_pos++;
                }
        }
        printk("<1>USB_AD buffer_write buffer cpos: %d, size: %d\n",
                    buf->curr_pos,
                    (int)buf->row_count);
        if(buf->curr_pos == buf->row_count) {//zapelnilismy bufor
                printk("<1>USB_AD buffer_write buffer full cpos: %d, size: %d\n",
                    buf->curr_pos,
                    (int)buf->row_count);
                buf->full = 1;
                return 1;
        }
        return 0;
}

/* kopiuje cala zawartosc bufora do tablicy */
int buffer_copy_to_user(Client_buffer *buf, unsigned char *data, int size) {
        if((buf == NULL) || (buf->buf == NULL) || (data == NULL))
                return USB_AD_ERROR_NULL;
        if(size != buf->size)               //rozmiar danych sie nie zgadza
                return USB_AD_ERROR_SIZE;
        copy_to_user(data, buf->buf, size);
        buf->curr_pos = 0L;
        buf->full = 0;                      //oznaczenie bufora jako przeczytanego
        memset(buf->buf, 0, buf->size);     //wyzerowanie bufora
        return 0;
}

#endif
