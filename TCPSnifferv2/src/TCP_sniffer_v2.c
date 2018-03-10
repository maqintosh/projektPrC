
#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <string.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <linux/if_ether.h>

//Dolaczam plik naglowkowy
#include "h1_v2.h"




//definiuje funkcje sluzace do zmiany kolejnosci wartosci w liscie
void reverse2B(unsigned short * bytes);
void reverse4B(unsigned int * bytes);

int main(void)
{
    printf("Uruchamiam odbieranie ramek Ethernet.\n");

    //Utworzenie bufora dla odbieranych ramek Ethernet
    char* buffer = (void*) malloc(ETH_FRAME_LEN);

    //Tworze wskaznik ktory wskazuje na pierwszy bajt naglowka TCP
    tcp* frame = (tcp *)(buffer + 34);

    //tcp* frame1 = (tcp *)(buffer + 50); teeest

    // deklaracja current-> aktualna wartosc z listy wiazanej, prev->poprzednia,head-> czolowy-glowny
    tcp *current,*prev, *head = NULL;

    // zmienna typu char aby pozniej umowliwilo zatrzymanie programu po wcisnieciu przycisku
    char znak='k';
    int flaga=1;

    // uzycie flagi do zatrzymania wykonywania programu(wyjscia z petli do return EXIT_SUCCESS;
    while(flaga)
    {
        // flaga pozwalajÄ…ca ustaliÄ‡ ile ramek odbieramy
        int liczbaRamek = 20;


        // Otwarcie gniazda pozwalajÄ…cego na odbiĂłr wszystkich ramek Ethernet
        int iEthSockHandl = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL)); // Htons zamienia kolejnosc bajtow pomiedzy hostem a siecia.
        //Kontrola czy gniazdo zostaĹ‚o otwarte poprawnie, w przypadku bledu wyĹ›wietlenie komunikatu.
        if (iEthSockHandl<0)
            printf("Problem z otwarciem gniazda : %s!\n", strerror(errno));

        //Zmienna do przechowywania rozmiaru odebranych danych
        int iDataLen = 0;

        //PÄ™tla nieskoĹ„czona do odbierania ramek Ethernet

        //Definicja zmiennej ktora umozliwi wyjscie z petli kiedy odbierzemy wystarczajaca liczbe ramek
        int i = 1;
        while (i)
        {

            //Odebranie ramki z utworzonego wczeĹ›niej gniazda i zapisanie jej do bufora
            iDataLen = recvfrom(iEthSockHandl, buffer, ETH_FRAME_LEN, 0, NULL, NULL);

            //kontrola bĹ‚edu ramki przy odebraniu
            if (iDataLen == -1)
                printf("Nie moge odebrac ramki: %s! \n", strerror(errno));
            else   //jeĹ›li ramka odebrana poprawnie wyĹ›wietlenie jej zawartoĹ›ci
            {
                // Wyswietlanie rozmiaru ramki ethernet i jej typ.
                printf("\nOdebrano ramke Ethernet o rozmiarze: %d [B] Typ: 0x%02x%02x\n", iDataLen,*(buffer + 12), *(buffer + 13));

                if((*(buffer + 12) == 0x08 && *(buffer + 13) == 0x00)) // Jesli *(buffer +12), *(buffer + 13) maja wartosc 0x0800 to jest to protokol ipv4
                    if(*(buffer + 23) == 0x06) // Jesli *(buffer + 23) bedzie mial wartosc 0x06 to jest to TCP
                    {
                        //Alokowanie pamieci do wskaznika current.
                        current = malloc(sizeof(struct tcp_h));

                        // wywolanie funkcji zamiany miejscami gdyĹĽ w "kablu" jest odwrotnie niĹĽ w hoscie
                       // reverse2B(&frame->urg_flag);
                        reverse2B(&frame->sourcePort);
                        reverse2B(&frame->destinationPort);
                        reverse4B(&frame->sequenceNumber);
                        reverse4B(&frame->acknowledgment);
                        reverse2B(&frame->headerLength);
                        reverse2B(&frame->windowSize);
                        reverse2B(&frame->checksum);
                        reverse2B(&frame->urgentPointer);

                        printf("Odebrano ramke TCP \n");

                        //jak koniec listy wiazanej to przypisanie current i od nowa przepisanie
                        if(head == NULL)
                            head = current;
                        else //dzialanie majace na celu zachowanie ciaglosci listy wiazanej
                            prev->next = current;

                        // przepisanie odpowiednio ustawionego naglowka TCP do listy wiazanej
                         //current->urg_flag = frame->urg_flag;
                        current->sourcePort = frame->sourcePort;
                        current->destinationPort = frame->destinationPort;
                        current->sequenceNumber = frame->sequenceNumber;
                        current->acknowledgment = frame->acknowledgment;
                        current->headerLength = frame->headerLength;
                        current->windowSize = frame->windowSize;
                        current->checksum = frame->checksum;
                        current->urgentPointer = frame->urgentPointer;


                        //konczymy liste wiazana az do przypisania nowej ramki
                        current->next = NULL;
                        prev = current;

                        // Zmniejszamy liczbe ramek
                        liczbaRamek--;

                        // Jesli liczba ramek dojdzie do zera to wychodzimy z petli.
                        if(liczbaRamek==0)
                            i=0;
                    }
            }
        }

        // Wskaznikowi current przypisujemy pierwszy element w liscie
        current = head;
        int n_ramki=1;
        // Teraz wyswietlamy liste od poczatku do momentu az wzkaznik current nie dojdzie do NULL (koniec listy)
        while(current != NULL)

        {


        	//printf("\n**************************************************************************    \n");
        	//printf("->  IP type: %02x%02x    \n",*(buffer+12),*(buffer+13));
        	//printf("->  HLEN type: %03x %03x   \n",*(buffer+47),*(buffer+46));


            printf("\n************************************* TCP ramka nr: %i *************************************    \n",n_ramki++);
            printf("->  Source Port :%04x   \n",current->sourcePort);
            printf("->  Destination Port: %04x   \n",current->destinationPort);
           // printf("->  TEST SOURCE %04x   \n",current->urg_flag);


            printf("->  Sequence Number: %08x    \n",current->sequenceNumber);
            printf("->  Acknowledgement: %08x  \n",current->acknowledgment);

            printf("->  Data offset and flags: %04x   \n",current->headerLength);



            /*Flagi â€“ 9-bitowa informacja/polecenie dotyczÄ…ce bieĹĽÄ…cego pakietu. PoszczegĂłlne flagi oznaczajÄ…:
    		NS â€“ (ang. Nonce Sum) jednobitowa suma wartoĹ›ci flag ECN (ECN Echo, Congestion Window Reduced, Nonce Sum) weryfikujÄ…ca ich integralnoĹ›Ä‡
    		CWR â€“ (ang. Congestion Window Reduced) flaga potwierdzajÄ…ca odebranie powiadomienia przez nadawcÄ™, umoĹĽliwia odbiorcy zaprzestanie wysyĹ‚ania echa.
    		ECE â€“ (ang. ECN-Echo) flaga ustawiana przez odbiorcÄ™ w momencie otrzymania pakietu z ustawionÄ… flagÄ… CE
    		URG â€“ informuje o istotnoĹ›ci pola "Priorytet"
    		ACK â€“ informuje o istotnoĹ›ci pola "Numer potwierdzenia"
    		PSH â€“ wymusza przesĹ‚anie pakietu
    		RST â€“ resetuje poĹ‚Ä…czenie (wymagane ponowne uzgodnienie sekwencji)
    		SYN â€“ synchronizuje kolejne numery sekwencyjne
   			FIN â€“ oznacza zakoĹ„czenie przekazu danych
              */

            unsigned short FIN,SYN,RST,PUSH,ACK,URGENT,ECN_ECHO,CWR;
            FIN=((current->headerLength))&00000001;
            SYN=((current->headerLength)>>1)&00000001;
            RST=((current->headerLength)>>2)&00000001;
            PUSH=((current->headerLength)>>3)&00000001;
            ACK=((current->headerLength)>>4)&00000001;
            URGENT=((current->headerLength)>>5)&00000001;
            ECN_ECHO=((current->headerLength)>>6)&00000001;
            CWR=((current->headerLength)>>7)&00000001;



            printf("->  FIN_FLAG %d   \n",FIN);
            printf("->  SYN_FLAG %d   \n",SYN);
            printf("->  RST_FLAG %d   \n",RST);
            printf("->  PSH_FLAG %d   \n",PUSH);
            printf("->  ACK_FLAG %d   \n",ACK);
            printf("->  URG_FLAG %d   \n",URGENT);
            printf("->  ECE_FLAG %d   \n",ECN_ECHO);
            printf("->  CWR_FLAG %d   \n",CWR);

            //printf("->  Data offset and flags: %x   \n",(current->headerLength)>>2);
            printf("->  Window Size: %04x \n",current->windowSize);
            printf("->  Checksum: %04x   \n",current->checksum);
            printf("->  Urgent Pointer: %04x   \n",current->urgentPointer);






            //Przechodzimy do nastepnego elementu.
            current = current->next;
            //n_ramki++;
        }





        // Zwalnianie pamieci listy
        while(head != NULL)
        {
            current = head;
            head = head->next;
            free(current);
        }

        printf("\n***************UWAGA***************\n");
        printf("ABY ZAKOĹ�CZYÄ† ODBIERANIE RAMEK WCIĹšNIJ KLAWISZ K \n");
       //Jesli chcemy zakonczyc dzialanie programu kliknij 'k'
        if((znak=getchar())=='k' || ( (znak=getchar())=='K') ){flaga=0;}


    }
    // uwalniamy pamiec bufora
    free(buffer);


    return EXIT_SUCCESS;
}


void reverse2B(unsigned short * byte)
{
    unsigned char * org = (unsigned char *) byte;
    unsigned char copy[2];
    memcpy(copy, byte, 2);
    org[0] = copy[1];
    org[1] = copy[0];
}

void reverse4B(unsigned int * byte)
{
    unsigned char * org = (unsigned char *) byte;
    unsigned char copy[4];
    memcpy(copy, byte, 4);
    org[0] = copy[3];
    org[1] = copy[2];
    org[2] = copy[1];
    org[3] = copy[0];
}




