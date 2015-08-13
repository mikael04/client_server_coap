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

		int main()
		{
				char buffer_para_servidor [256];
				int i, k=0;
				printf("Digite a string por favor\n");
				scanf("%s",buffer_para_servidor);
				printf("Length of string = %d\n", strlen(buffer_para_servidor));
				char buffer_temp [strlen(buffer_para_servidor)];
				int buf;		
				unsigned char buffer_hex [strlen(buffer_para_servidor)];
				strncpy (buffer_temp, buffer_para_servidor, strlen(buffer_para_servidor));
		
				printf("String = %s\n", buffer_temp);
				/*for (i=0; i<strlen(buffer_temp); i=i+2)
				{		
					buffer_hex [k] = "0x"+buffer_temp[i]+buffer_temp[i+1];
					k=k+1;
					printf("K = %d\n", k);
				}*/
				buffer_hex = {0x1E, 0x28, 0x05};
				buf = 0x1E2805;
				printf("%s\n\n", buffer_hex);
				printf("%d\n\n", buf);
				printf("%02X%s", buffer_hex, (sizeof(buffer_hex) > 0) ? " " : "");
		}