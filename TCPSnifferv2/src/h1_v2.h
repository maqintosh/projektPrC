#ifndef H1_H_INCLUDED
#define H1_H_INCLUDED

// definicja i inicjalizacja naglowka
typedef struct tcp_h
{

    unsigned short sourcePort,destinationPort;
    unsigned int sequenceNumber,acknowledgment;
    unsigned short headerLength,windowSize,checksum,urgentPointer;

    struct tcp_h *next;
}tcp;



#endif // H1_H_INCLUDED
