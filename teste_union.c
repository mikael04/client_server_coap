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

union valor
{
	uint32_t dword;
	struct
	{
		uint32_t valor;
	};
	struct
	{
		uint16_t word0;
		uint16_t word1;
	};
	struct
	{
		uint8_t byte0;
		uint8_t byte1;
		uint8_t byte2;
		uint8_t byte3;
	};
};

int main ()
{
	union valor var;
	var.valor = 700;

	printf("Valor = %d\n", var.valor);
	printf("Byte 0 = %d\n", var.byte0);
	printf("Byte 1 = %d\n", var.byte1);
	printf("Word 0 = %d\n", var.word0);
	return 0;
}
