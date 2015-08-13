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

struct coap_header_t
{
	uint8_t ver;                
    uint8_t t;                  
    uint8_t tkl;                
    uint8_t code;
    uint8_t id[2];
};

int main ()
{
	char buf[256];
	struct coap_header_t hdr;

	//0100 1000 0000 0000 0000 0010 0000 0001
	hdr.ver 	= 	0x01; // versão 01;
	hdr.t 		= 	0x00; // code 0 (confirmable);
	hdr.tkl 	=	0x00; //Tamanho do token -> para testes, 0
	hdr.code 	=	0x03; //request
	hdr.id[0] 	= 	0x02; //0010 -> TESTE
	hdr.id[1]	= 	0x01; //0001 -> TESTE
	//0110 0000
	buf[0] = (hdr.ver & 0x03) << 6;
    buf[0] |= (hdr.t & 0x03) << 4;
    buf[0] |= (hdr.tkl & 0x0F);

	printf("buffer [0] = %s\n", buf[0]);
	return 0;
}