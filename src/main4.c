#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "gen.h"
#include "pkt.h"
#include "part.h"
#include "dlist.h"
#include "netlib.h"

sock_t this;
dlist_t pool;

int done = 0;

dlist_t bests;

char last = 0;
void *nb_getch(void *arg)
{
    while (1)
    {
        last = getch();
    }
}

void runc(char *ipaddr, char *port)
{
    nlib_mksock(&this, ipaddr, port, NLIB_SERVER, NLIB_TCP);
    nlib_noblock(&this);
    printf("LISTENING FOR CONNECTIONS...\n");

    // thread for non-blocking input
    pthread_t thread;
    pthread_create(&thread, NULL, nb_getch, NULL);

    while (1)
    {
        sleep(10);
        pkt_t pkt;
        sock_t worker;

        if (nlib_accept(&this, &worker))
        {
            printf("%s CONNECTED TO THE NETWORK\n", worker.ipaddr);
            dl_add(&pool, &worker);

            // send set
            int len = sizeof(int) + sizeof(int) * size;
            void *pld = malloc(len);
            *(int *)(pld) = size;
            memcpy(pld + sizeof(int), set, size);
            pkt.id = PKT_SET;
            pkt.len = len;
            pkt.pld = pld;
            send_pkt(&worker, &pkt);
            free(pld);

            // send gen info
            len = sizeof(int) * 2;
            pld = malloc(len);
            *(int *)(pld) = MAX_GEN;
            *(int *)(pld + sizeof(int)) = PER_GEN;
            pkt.id = PKT_GEN;
            pkt.len = len;
            pkt.pld = pld;
            send_pkt(&worker, &pkt);
            free(pld);
        }

        switch (last)
        {
        case 'R':
            pkt.id = PKT_RUN;
            pkt.len = 0;
            for (int i = 0; i < pool.used; i++)
            {
                worker = *(sock_t *)pool.get[i];
                send_pkt(&worker, &pkt);
            }
            printf("RUN COMMAND SENT\n");
            last = 0;
            break;
        case 'Q':
            printf("EXIT RECEIVED... GOODBYE\n");
            exit(0);
        }

        for (int i = 0; i < pool.used; i++)
        {
            worker = *(sock_t *)pool.get[i];

            int res = recv_pkt(&worker, &pkt);

            if (!res)
            {
                continue;
            }

            if (res < 0)
            {
                printf("CONNECTION TO WORKER %s LOST\n", worker.ipaddr);
                dl_rem(&pool, i);
                break;
            }

            switch (pkt.id)
            {
            case PKT_SOL:
                part_t best;
                pinit(&best);

                best.sum1 = *(size_t *)(pkt.pld);
                best.sum2 = *(size_t *)(pkt.pld + sizeof(size_t));
                best.diff = *(size_t *)(pkt.pld + 2 * sizeof(size_t));
                memcpy(best.buf, pkt.pld + 3 * sizeof(size_t), size);

                pkt.id = PKT_END;
                pkt.len = 0;
                send_pkt(&worker, &pkt);
                printf("SOLUTION FOUND\n");
                printp_brief(&best);
                exit(0);
            case PKT_FIN:
                printf("%s FINNISHED\n", worker.ipaddr);
                part_t part;
                pinit(&part);

                part.sum1 = *(size_t *)(pkt.pld);
                part.sum2 = *(size_t *)(pkt.pld + sizeof(size_t));
                part.diff = *(size_t *)(pkt.pld + 2 * sizeof(size_t));
                memcpy(part.buf, pkt.pld + 3 * sizeof(size_t), size);

                dl_add(&bests, &part);

                done++;
                if (done == pool.used)
                {
                    pkt.id = PKT_END;
                    pkt.len = 0;
                    send_pkt(&worker, &pkt);
                    printf("ALL WORKERS HAVE FINNISHED\n");

                    pinit(&best);
                    for (int j = 0; j < bests.used; j++)
                    {
                        part_t *best_ = bests.get[j];
                        if (best_->diff < best.diff)
                        {
                            copy(&best, best_);
                        }
                    }

                    printp_brief(&best);
                    exit(0);
                }
                continue;
            }
        }
    }
    exit(0);
}

void runw(char *ipaddr, char *port)
{
    nlib_mksock(&this, NULL, NULL, NLIB_CLIENT, NLIB_TCP);
    if (!nlib_connect(&this, ipaddr, port))
    {
        printf("UNABLE TO CONNECT TO %s:%s\n", ipaddr, port);
        exit(1);
    }
    printf("CONNECTION SUCCESS\n");
    nlib_noblock(&this);

    while (1)
    {
        sleep(10);
        pkt_t pkt;

        int res = recv_pkt(&this, &pkt);

        if (!res)
        {
            continue;
        }

        if (res < 0)
        {
            printf("CONNECTION TO COMMANDER LOST\n");
            exit(0);
        }

        switch (pkt.id)
        {
        case PKT_RUN:
            printf("RUN COMMAND RECEIVED\n");

            gen() ? (pkt.id = PKT_SOL) : (pkt.id = PKT_FIN);

            pkt.len = 3 * sizeof(size_t) + sizeof(int8_t) * size;
            pkt.pld = malloc(pkt.len);
            *(size_t *)(pkt.pld) = best.sum1;
            *(size_t *)(pkt.pld + sizeof(size_t)) = best.sum2;
            *(size_t *)(pkt.pld + 2 * sizeof(size_t)) = best.diff;
            memcpy(pkt.pld + 3 * sizeof(size_t), best.buf, sizeof(int8_t) * size);
            send_pkt(&this, &pkt);
            free(pkt.pld);

            printf("DONE\n");
            break;
        case PKT_END:
            printf("EXIT RECEIVED... GOODBYE\n");
            exit(0);
        case PKT_SET:
            size = *(int *)(pkt.pld);
            set = (int *)(pkt.pld + sizeof(int));
            printf("RECEIVED SET OF SIZE: %d\n", size);
            continue;
        case PKT_GEN:
            MAX_GEN = *(int *)(pkt.pld);
            PER_GEN = *(int *)(pkt.pld + sizeof(int));
            printf("RECEIVED MAX_GEN AND PER_GEN: %d %d\n", MAX_GEN, PER_GEN);
            continue;
        }

        free(pkt.pld);
    }
    exit(0);
}

int main(int argc, char *argv[])
{
    char *usage = "USAGE: %s <COMMANDER|WORKER> <IP ADDRESS> <PORT> [COMMANDER: N ITEMS] [COMMANDER: MAX GEN] [COMMANDER: PER GEN]\n";
    if (argc < 4)
    {
        printf(usage, argv[0]);
        return 0;
    }

    nlib_init();
    dl_init(&pool, sizeof(sock_t));
    dl_init(&bests, sizeof(part_t));

    if (!strcmp(argv[1], "COMMANDER"))
    {
        if (argc != 7)
        {
            printf(usage, argv[0]);
            return 0;
        }
        init(atoi(argv[4]));
        MAX_GEN = atoi(argv[5]);
        PER_GEN = atoi(argv[6]);
        runc(argv[2], argv[3]);
    }

    if (!strcmp(argv[1], "WORKER"))
    {
        runw(argv[2], argv[3]);
    }

    printf(usage, argv[0]);
    return 0;
}
