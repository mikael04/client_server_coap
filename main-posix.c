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

/*srand and rand*/
#include <stdlib.h>

/*libs */
#include "coap.h"
#include "cli_main.h"


//cli -op 11 var -op 11 temperature -p 1234ddAS

#define PORT_CLI 32012
#define PORT_SERV 7891
#define TIME 0
#define DEBUG_MAIN 1 //Debug Geral
#define DEBUG_SAIR 1
#define DEBUG_SEND_ANOTHER_SERV 0
#define DEBUG_FORK 0
#define DEBUG_TIME 1 //DEBUG com tempo padrão (não obtido do endpoint)
#define DEBUG_GET_TIME 0 //DEBUG com tempo padrão (não obtido do endpoint)

//DEBUG
#define TIME_CALL_CLI_SEC 0.0166667*60*6 //0.0166667*60 = 1 segundo
#define TIME_CALL_CLI_NSEC 0

#define DEBUG_TIME_SEC 1
#define DEBUG_TIME_NSEC 0

#define PORCENTAGEM 60

int main(int argc, char **argv)
{

    /*SLEEP - TEST*/
#if TIME
    struct timespec req = {0};
        req.tv_sec = DEBUG_TIME_SEC;
        req.tv_nsec = DEBUG_TIME_NSEC;
#endif

    //var_cli variables = create_var_time(); //Criando estrutura que armazena tempo de exec do cli

#if DEBUG_MAIN & DEBUG_GET_TIME
    printf("Get = %d\n", get_var_time());
#endif
    struct timespec call_cli = {0};
#if DEBUG_MAIN && DEBUG_TIME
        call_cli.tv_sec = TIME_CALL_CLI_SEC;
        call_cli.tv_nsec = TIME_CALL_CLI_NSEC;
#endif
    srand (time(NULL)); //RANDOM FUNCTION
    int fd_client;
    int fd_other_serv;

    //Creating Bufs
    uint8_t buf[4096];
    uint8_t scratch_raw[4096];
    coap_rw_buffer_t scratch_buf = {scratch_raw, sizeof(scratch_raw)};

#ifdef IPV6
    struct sockaddr_in6 servaddr, cliaddr_serv;
#else /* IPV6 */
    struct sockaddr_in servaddr, cliaddr_serv;
#endif /* IPV6 */

#ifdef IPV6
    fd_client = socket(AF_INET6,SOCK_DGRAM,0);
#else /* IPV6 */
    fd_client = socket(AF_INET,SOCK_DGRAM,0);
#endif /* IPV6 */

    bzero(&servaddr,sizeof(servaddr));
#ifdef IPV6
    servaddr.sin6_family = AF_INET6;
    //servaddr.sin6_addr = in6addr_any;
    servaddr.sin6_port = htons(PORT_CLI);
#else /* IPV6 */
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("192.168.252.128");
    servaddr.sin_port = htons(PORT_CLI);
#endif /* IPV6 */
    bind(fd_client,(struct sockaddr *)&servaddr, sizeof(servaddr));

#ifdef IPV6
    fd_other_serv = socket(AF_INET6,SOCK_STREAM,0);
#else /* IPV6 */
    fd_other_serv = socket(AF_INET,SOCK_STREAM,0);
#endif

    bzero(&cliaddr_serv,sizeof(cliaddr_serv));
#ifdef IPV6
    cliaddr_serv.sin6_family = AF_INET6;
    //cliaddr_serv.sin6_addr = in6addr_any;
    cliaddr_serv.sin6_port = htons(PORT_SERV);
#else /* IPV6 */
    cliaddr_serv.sin_family = AF_INET;
    /* Cliente/Servidor Linux (Simple)*/
    //cliaddr_serv.sin_addr.s_addr = inet_addr("192.168.252.128");
    /* Cliente/Servidor Windows*/
    cliaddr_serv.sin_addr.s_addr = inet_addr("192.168.1.13");
    cliaddr_serv.sin_port = htons(PORT_SERV);
#endif /* IPV6 */
    bind(fd_other_serv,(struct sockaddr *)&cliaddr_serv, sizeof(cliaddr_serv));

    endpoint_setup();

    pid_t pid, pid2;
    int status;

    pid = fork ();
    if (pid > 0)
    {
#if DEBUG_MAIN
            printf("Starting recv\n");
#endif
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
            pid2 = fork();

            if (pid2 == 0)
            {
                if(0 == (strcmp((char*)buf, "sair\0")))
                {
                    printf("Saindo do servidor\n");
                    kill(pid, SIGTERM); 
                    return 0;           
                }
    #if DEBUG_MAIN && DEBUG_FORK
                printf("Sending other serv, son1\n");
    #endif
                size_t rsplen = sizeof(buf);
                if(connect(fd_other_serv, (struct sockaddr *)&cliaddr_serv, sizeof(cliaddr_serv)) < 0)
                {
                   printf("\n Error : Connect Failed \n");
                   return 1;
                }
                send(fd_other_serv,buf, rsplen,0);
                printf("Son Dead?\n");                
            }
            else if (pid2 > 0)
            {
    #if DEBUG_MAIN && DEBUG_FORK
                printf("Sending client, father\n");
    #endif
                
    #if DEBUG_MAIN && DEBUG_SAIR
                if(0 == (strcmp((char*)buf, "sair\0")))
                {
                    printf("Saindo do servidor\n");
                    waitpid(pid, &status, 0);
                    kill(pid, SIGTERM);
                    close(fd_client);
                    close(fd_other_serv);
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
                        call_cli.tv_sec = get_var_time(variables);
                        call_cli.tv_nsec = 0;
#endif
                        if (rand()%100<PORCENTAGEM)
                        {
                            printf("\n\nSending\n\n");
                            sendto(fd_client, buf, rsplen, 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
                        }
                        waitpid(pid2, &status, 0);
                        kill(pid, SIGTERM);
    #if DEBUG_MAIN && DEBUG_FORK
                        printf("Exec father\n");
    #endif
                        
                    }
                }
            }
            else
            {
                // fork failed
                printf("fork() failed!\n");
                return 1;
            }
        }
        //waitpid(pid, &status2, 0);
    }
    else if (pid == 0)
    {
        int clin = 0;
        while (clin<5)
        {
#if DEBUG_MAIN
            printf("Child 1 pid, Chamando %d vez\n", clin);
            printf("Get = %d\n", get_var_time());
#endif
            nanosleep(&call_cli, (struct timespec *) NULL);
            //main_cli();
            clin++;
        }
        return 0;
    }
    else
    {
        // fork failed
        printf("fork() failed!\n");
        return 1;
    }

    close(fd_client);
    close(fd_other_serv);
    return 0;
}
