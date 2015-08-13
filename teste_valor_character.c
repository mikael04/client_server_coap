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

int main ()
{
	char c;
	int b;
	printf("Digite uma letra\n");
	scanf("%c", &c);
	printf("letra = %c, letra = %d\n", c, c);
	if (c < 48 || (c > 57 && c < 65) || (c > 90 && c < 97) || c > 122) 
	{
		printf("nao e letra ou numero\n");
	}
}