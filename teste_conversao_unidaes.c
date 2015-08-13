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

typedef struct
{
	uint8_t *p;
	size_t len;
} coap_buffer_t;

typedef struct
{
	uint8_t *num;
	coap_buffer_t buf;
} coap_option_t;

void passa_arg (coap_option_t *op)
{
	int op_num = 23;
	*op->num = op_num;
	printf("option = 0x%02X\n", *op->num);
}

int main ()
{
	coap_option_t opt;
	uint8_t *option;
	int op;
	op = 23;

	passa_arg (&opt);
	//*option =  op;
	//printf("option = 0x%02X\n", *option);
}