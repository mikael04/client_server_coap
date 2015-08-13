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
	printf("option = %d\n", *op->num);
	char *palavra = NULL;
	palavra = "bccc21s";
	uint8_t *pa = NULL;
	pa = (uint8_t *)palavra;
	printf("palavra = %s\n", palavra);
	printf("pa = 0x%02X\n", pa[0]);
	printf("pa = 0x%02X\n", pa[01]);
	printf("pa = 0x%02X\n", pa[2]);
	printf("pa = 0x%02X\n", pa[3]);
	printf("pa = 0x%02X\n", pa[4]);
	printf("pa = 0x%02X\n", pa[5]);
	printf("pa = 0x%02X\n", pa[6]);
	printf("pa = 0x%02X\n", pa[7]);
	printf("pa = 0x%02X\n", pa[8]);
	printf("pa = 0x%02X\n", pa[9]);
}
//STRTOL
//char *end;
//int num_op = strtol(op_num, &end, 0);

//SNPRINTF
//snprintf(end, 100,"%d",num_op);
int main ()
{
	coap_option_t opt;
	//uint8_t *option;
	//int op;
	//op = 23;

	passa_arg (&opt);
	return 0;
	//*option =  op;
	//printf("option = 0x%02X\n", *option);
}