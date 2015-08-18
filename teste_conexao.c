#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <string.h>

int main (int argc, char *argv[])
{
		//Pacote
		int i=0;
		//Conexão
		char buffer[256];
		uint8_t buf[256];
		int j;
		int buf_len = sizeof(buf);
		memset(buf, 0x00, buf_len);
		memset(buffer, 0x00, buf_len);

		int fd;
#ifdef IPV6
    	struct sockaddr_in6 servaddr, cliaddr;
#else /* IPV6 */
    	struct sockaddr_in servaddr, cliaddr;
#endif /* IPV6 */

#ifdef IPV6
    	fd = socket(AF_INET6,SOCK_DGRAM,0);
#else /* IPV6 */
    	fd = socket(AF_INET,SOCK_DGRAM,0);
#endif /* IPV6 */

    	bzero(&cliaddr,sizeof(cliaddr));
#ifdef IPV6
    	cliaddr.sin6_family = AF_INET6;
    	cliaddr.sin6_addr = in6addr_any;
    	cliaddr.sin6_port = htons(5683);
#else /* IPV6 */
    	cliaddr.sin_family = AF_INET;
    	cliaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    	cliaddr.sin_port = htons(5683);
#endif /* IPV6 */
    	bind(fd,(struct sockaddr *)&cliaddr, sizeof(cliaddr));


		while (i==0)
		{
			printf("Digite a mensagem a ser enviada:");
			fgets(buffer, 256, stdin);
			int len = strlen(buffer);
			printf("len = %d\n", len);
			for(j=0; j<len; j++)
			{
				buf[j] = buffer[j];
				printf("buf[%d] = 0x%02x\n", j, buf[j]);
			}
			for (j=0; j<len; j++)
			{
				printf("buf[%d] = 0x%02x\n", j, buf[j]);
			}
			//scanf("%x", buf);
			if(!strcmp(buffer, "sair\0"))
			{
				i=1;
			}
			size_t rsplen = sizeof(buf);
			sendto(fd, buf, rsplen, 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr));
			//write(clienteSockfd, buffer, addrlen);
			//write(clienteSockfd, buf, addrlen);
		}
		return 0;
}
