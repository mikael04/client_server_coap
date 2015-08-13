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
	/*
	int i,j;
	int cont = 0;
	for (i=0; i<argc; i++)
	{
		for(j=0; j<strlen(argv[i]); j++)
		{
			if (argv[i][j] == '-')
			{
				printf("posição i = %d, j = %d\n", i, j);
				printf("Achamos um -\n");
				cont++;
			}
		}
	}

	if ((argc)/(cont) != 2)
	{
		printf("Erro diferença de menos\n");
	}
	return cont;
	*/
		if (argv[2][0] == '-')
	{
		printf("Achamos um -\n");
		return 1;
	}
	return 0;
}