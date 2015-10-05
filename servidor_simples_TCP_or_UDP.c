
/* Sample UDP server */
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#define DEBUG 0
#define ANY 0
#define TCP 0
#define TIME 1
#define SEND 1
#define PORT 32000

#define PORCENTAGEM 75


#define WAIT_SEC 1
#define WAIT_NSEC 0

void printf_hexa (char *buf_in)
{
   int i;
   for (i=0; i<strlen(buf_in); i++)
   {
      printf("Caracter %d = %c = 0x%02X\n", i, buf_in[i], buf_in[i]);
   }
}

//cli -op 11 var -op 11 temperature -p 1234C

int main(int argc, char**argv)
{
   srand (time(NULL));
   int sockfd,n;
   struct sockaddr_in servaddr,cliaddr;
   socklen_t len;
   char mesg[1000];
   //char msg [20] = "Servidor msg";
   //memcpy(mesg, 0x00, 1000);
   //SLEEP
   //int sleep_mili = 1000;
#if TIME
   struct timespec req = {0};
   req.tv_sec = WAIT_SEC;
   req.tv_nsec = WAIT_NSEC;
#endif

#if TCP
   printf("TCP\n");
   sockfd=socket(AF_INET,SOCK_STREAM,0);
#else //UDP
   printf("UDP\n");
    sockfd=socket(AF_INET,SOCK_DGRAM,0);
#endif

   bzero(&servaddr,sizeof(servaddr));
   servaddr.sin_family = AF_INET;
#if ANY
   servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
#else
   servaddr.sin_addr.s_addr=inet_addr("192.168.252.128");
#endif
   servaddr.sin_port=htons(PORT);
   bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));

   for (;;)
   {
      //memset(mesg, 0x00, 1000);
      strncpy(mesg, "", 1000);
      len = sizeof(cliaddr);
      n = recvfrom(sockfd,mesg,1000,0,(struct sockaddr *)&cliaddr,&len);
      //mesg[n] = '\0';
#if DEBUG
      printf_hexa(mesg);
#endif
      if(strcmp(mesg, "sair ") == 0)
      {
         printf("Fechando socket\n");
         close(sockfd);
         return 0;
      }
      else if (strcmp (mesg, "") != 0)
      {


         printf("-------------------------------------------------------\n");

         printf("Received the following:\n");
         printf("%s\n",mesg);
         //printf("p-1 = 0x%02X, p = 0x%02X, p+1 = 0x%02X", mesg[3],mesg[4],mesg[5]);
         printf("-------------------------------------------------------\n");
#if TIME
         nanosleep(&req, (struct timespec *) NULL);
#endif
#if SEND
         if (rand()%100<PORCENTAGEM)
         {
            printf("\n\nSending -> %s\n\n", mesg);
            sendto(sockfd,mesg,strlen(mesg),0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
         }
#endif
      }
   }
   close(sockfd);
   return 0;
}
