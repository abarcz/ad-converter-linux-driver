#ifndef USB_AD_CONSTANTS
#define USB_AD_CONSTANTS

/**** podstawowe define'y urzadzenia usb ****/
/* Define these values to match your devices */
#define USB_AD_VENDOR_ID      0xa5a5        //hex(42405)
#define USB_AD_PRODUCT_ID     0x01          //hex(1)
/* Get a minor range for your devices from the usb maintainer */
#define USB_AD_MINOR_BASE       192         //:TODO:
#define USB_AD_MAX_TRANSFER     (PAGE_SIZE - 512)
/* USB_AD_MAX_TRANSFER  is chosen so that the VM is not stressed by
   allocations > PAGE_SIZE and the number of packets in a page
   is an integer 512 is the largest possible packet on EHCI */
#define USB_AD_WRITES_IN_FLIGHT 8           /* arbitrarily chosen */

/**** mozliwe bledy ****/
#define USB_AD_ERROR_NULL -1                //np niezainicjalizowany wskaznik
#define USB_AD_ERROR_FULL -2                //np pelny bufor
#define USB_AD_ERROR_SIZE -3                //np niewlasciwy rozmiar bufora
#define USB_AD_ERROR_ALLOCATION -4          //blUSB_AD alokacji
#define USB_AD_ERROR_VALUE -5               //nieprawidlowa wartosc
#define USB_AD_ERROR_UNKNOWN -6             //?? ;)

/**** inne stale ****/
#define USB_AD_CHANNELS_NUM 7               //ilosc kanalow probkowanych przez A/C
#define USB_AD_CLIENT_BUFFER_MAX_SIZE 1024  //w bajtach
#define USB_AD_MAX_CLIENTS_NUM 100          //ile programow moze naraz podlaczyc
                                            //sie do sterownika
#define USB_AD_BOOT_CONF 1                  //id podstawowej, nieuzywanej konfiguracji
#define USB_AD_ACTIVE_CONF 2                //id wlasciwej konfiguracji urzadzenia
#define USB_AD_MAX_MSG_WAIT 500             //w jiffies
#define USB_AD_WAIT_LOOPS_AFTER_WARNING 7   /* ilosc petli, jaka odczeka
                                             * sterownik po zaznaczeniu flagi
                                             * warning przed odlaczeniem
                                             * klienta 
                                             */
#define USB_AD_FQ_H 127                     //starszy bajt czestotliwosci
#define USB_AD_FQ_L 127                     //mlodszy bajt czestotliwosci
#define USB_AD_FQ_DIV 0                     //podzial czestotliwosci (mozliwe wartosci: 00,01,10,11)

/* zwraca czestotliwosc z jaka przetwornik w rzeczywistosci probkuje (na wszystkich kanalach) */
int usb_ad_fq(void) {
        int res = USB_AD_FQ_H * 128 + USB_AD_FQ_L;

        /* podzial czestotliwosci */
        if (USB_AD_FQ_DIV & 0)
            return res;
        if (USB_AD_FQ_DIV & 1)
            return res / 2;
        if (USB_AD_FQ_DIV & 2)
            return res / 4;
        //if (USB_AD_FQ_DIV & 3)
        return res / 8;
}

#endif
