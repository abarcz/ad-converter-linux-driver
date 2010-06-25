#ifndef USB_AD_CONSTANTS
#define USB_AD_CONSTANTS

/* mozliwe bledy */
#define USB_AD_ERROR_NULL -1                //np niezainicjalizowany wskaznik
#define USB_AD_ERROR_FULL -2                //np pelny bufor
#define USB_AD_ERROR_SIZE -3                //np niewlasciwy rozmiar bufora
#define USB_AD_ERROR_ALLOCATION -4          //blUSB_AD alokacji
#define USB_AD_ERROR_VALUE -5               //nieprawidlowa wartosc
#define USB_AD_ERROR_UNKNOWN -6             //?? ;)

/* inne stale */
#define USB_AD_CHANNELS_NUM 7               //ilosc kanalow probkowanych przez A/C
#define USB_AD_CLIENT_BUFFER_MAX_SIZE 1024  //w bajtach
#define USB_AD_MAX_CLIENTS_NUM 100          //ile programow moze naraz podlaczyc
                                            //sie do sterownika


#endif
