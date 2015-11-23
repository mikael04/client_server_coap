#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdbool.h>
#include <strings.h>
#include <inttypes.h>
/*INET*/
#include <arpa/inet.h>
/*SLEEP - TEST*/
#include <time.h>
/*Palavras (sair) */
#include <string.h>
/*Close and fork*/
#include <unistd.h>

/*SIGNAL, KILL FORK*/
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#include <pthread.h>
/*srand and rand*/
#include <stdlib.h>

/*libs */
#include "coap.h"
#include "cli_main.h"


#define PORT_CLI 32015
/*#define PORT_SERV 7891 /*EMBARCADO NÃO REENVIA, CLIENTE ENVIARÀ -> não usada*/
#define TIME 0
#define DEBUG_MAIN 1 /*Debug Geral/
#define DEBUG_SAIR 1
#define DEBUG_SEND_ANOTHER_SERV 0
#define DEBUG_FORK 0
#define DEBUG_TIME 0 /*DEBUG com tempo padrão (não obtido do endpoint)*/
#define DEBUG_GET_TIME 1 /*DEBUG com tempo padrão (não obtido do endpoint)*/
#define DEBUG_SERV 1
#define CASA 1
#define MEGA 0

/*DEBUG
#define TIME_CALL_CLI_SEC 0.0166667*60*6 /*0.0166667*60 = 1 segundo*/
#define TIME_CALL_CLI_NSEC 0
#define NUM_THREADS 1

#define DEBUG_TIME_SEC 1
#define DEBUG_TIME_NSEC 0

#define PORCENTAGEM 100


void *thr_func_cli (void *arg)
{
    struct timespec *call_cli = (struct timespec *) arg;
    int clin = 0;
    while(clin<1)
    {
        nanosleep(call_cli, (struct timespec *) NULL);
#if DEBUG_MAIN
        printf("Child 1 pid, Chamando %d vez\n", clin);
        printf("Get = %d\n", get_var_time());
        printf("call_cli.tempo = %d\n", (int) call_cli->tv_sec);
#endif
        main_cli();
        clin++;
    }
    pthread_exit(NULL);
}

void *thr_func_serv_recv (void *arg)
{
    struct timespec *call_cli = (struct timespec *) arg;
        /*SLEEP - TEST*/
#if TIME
    struct timespec req = {0};
        req.tv_sec = DEBUG_TIME_SEC;
        req.tv_nsec = DEBUG_TIME_NSEC;
#endif
#if DEBUG_MAIN && DEBUG_TIME
        *call_cli->tv_sec = TIME_CALL_CLI_SEC;
        *call_cli->tv_nsec = TIME_CALL_CLI_NSEC;
#endif
    srand (time(NULL)); /*RANDOM FUNCTION*/
    int fd_client;

    /*Creating Bufs*/
    uint8_t buf[4096];
    uint8_t scratch_raw[4096];
    coap_rw_buffer_t scratch_buf = {scratch_raw, sizeof(scratch_raw)};

#ifdef IPV6
    struct sockaddr_in6 servaddr;
#else /* IPV6 */
    struct sockaddr_in servaddr;
#endif /* IPV6 */

#ifdef IPV6
    fd_client = socket(AF_INET6,SOCK_DGRAM,0);
#else /* IPV6 */
    fd_client = socket(AF_INET,SOCK_DGRAM,0);
#endif /* IPV6 */

    bzero(&servaddr,sizeof(servaddr));
#ifdef IPV6
    servaddr.sin6_family = AF_INET6;
    servaddr.sin6_addr = in6addr_any;
    servaddr.sin6_port = htons(PORT_CLI);
#else /* IPV6 */
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("192.168.1.16");
    servaddr.sin_port = htons(32015);
#endif /* IPV6 */
    bind(fd_client,(struct sockaddr *)&servaddr, sizeof(servaddr));

    while(1)
    {
        int n, rc;
        socklen_t len = sizeof(servaddr);
        coap_packet_t pkt;
        n = recvfrom(fd_client, buf, sizeof(buf), 0, (struct sockaddr *)&servaddr, &len);
#if DEBUG_MAIN
        printf("Received this: ");
        coap_dump(buf, n, true);
        printf("\n");
#endif

#if DEBUG_MAIN && DEBUG_SAIR
        if(0 == (strcmp((char*)buf, "sair\0")))
        {
            printf("Saindo do servidor\n");
            close(fd_client);
            return 0;            
        }
        else if (0 != (rc = coap_parse(&pkt, buf, n)))
            printf("Bad packet rc=%d\n", rc);
#else

        if (0 != (rc = coap_parse(&pkt, buf, n)))
            printf("Bad packet rc=%d\n", rc);
#endif
        else
        {
            
            coap_packet_t rsppkt;
#if DEBUG_MAIN
            coap_dumpPacket(&pkt);
#endif
            coap_handle_req(&scratch_buf, &pkt, &rsppkt);
            size_t rsplen = sizeof(buf);

            if (0 != (rc = coap_build(buf, &rsplen, &rsppkt)))
                printf("coap_build failed rc=%d\n", rc);
            else
            {
#if DEBUG_MAIN
                printf("Sending: ");
                coap_dump(buf, rsplen, true);
                printf("\n");
#endif
#if DEBUG_MAIN
                coap_dumpPacket(&rsppkt);
#endif
#if TIME
                nanosleep(&req, (struct timespec *) NULL);
#endif
#if DEBUG_TIME == 0
                call_cli->tv_sec = get_var_time();
                call_cli->tv_nsec = 0;
#endif
                if (rand()%100<PORCENTAGEM)
                {
                    printf("\n\nSending\n\n");
                    sendto(fd_client, buf, rsplen, 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
                }
                else
                {
                    printf("\n\nNot sending, simulando erro de comunicação\n\n");
                }                    
            }
        }
    }
    close(fd_client);    
    pthread_exit(NULL);
}

int main(int argc, char **argv)
{

    int i;
    pthread_t thr[NUM_THREADS];
    struct timespec call_cli = {0};

    endpoint_setup();

    call_cli.tv_sec = get_var_time();
    call_cli.tv_nsec = 0;

    int rc;

    printf("thread iniciando cliente \n");
    if ((rc = pthread_create(&thr[0], NULL, thr_func_cli, &call_cli)))
    {
        fprintf(stderr, "error: pthread_create, rc: %d\n", rc);
        return EXIT_FAILURE;
    }
    printf("thread iniciando servidor \n");
    if ((rc = pthread_create(&thr[1], NULL, thr_func_serv_recv, &call_cli)))
    {
        fprintf(stderr, "error: pthread_create, rc: %d\n", rc);
        return EXIT_FAILURE;
    }

    for (i = 0; i < NUM_THREADS; ++i)
    {
        pthread_join(thr[i], NULL);
    }

    return 0;
}
