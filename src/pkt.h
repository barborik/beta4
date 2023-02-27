#ifndef __PKT_
#define __PKT_

#include "netlib.h"

enum
{
    PKT_RUN = 0x0A, // COMMANDER -> WORKER
    PKT_END = 0x0B, // COMMANDER -> WORKER
    PKT_SET = 0x0C, // COMMANDER -> WORKER
    PKT_GEN = 0x0D, // COMMANDER -> WORKER
    PKT_SOL = 0x0E, // WORKER -> COMMANDER
    PKT_FIN = 0x0F, // WORKER -> COMMANDER
};

typedef struct
{
    char id;
    int len;
    void *pld;
} pkt_t;

int send_pkt(sock_t *sock, pkt_t *pkt)
{
    int len = sizeof(char) + sizeof(int) + pkt->len;
    void *data = malloc(len);

    *(char *)(data) = pkt->id;
    *(size_t *)(data + sizeof(char)) = pkt->len;
    memcpy(data + sizeof(char) + sizeof(int), pkt->pld, pkt->len);

    nlib_send(sock, data, len);
    return 1;
}

int recv_pkt(sock_t *sock, pkt_t *pkt)
{
    int res;

    res = nlib_recv(sock, &pkt->id, sizeof(char));
    res = nlib_recv(sock, &pkt->len, sizeof(int));

    if (pkt->len)
    {
        pkt->pld = malloc(pkt->len);
        res = nlib_recv(sock, pkt->pld, pkt->len);
    }

    return res;
}

int send_set(sock_t *sock)
{
}

#endif