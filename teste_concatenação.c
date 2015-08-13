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
    uint8_t ver;                /* CoAP version number */
    uint8_t t;                  /* CoAP Message Type */
    uint8_t tkl;                /* Token length: indicates length of the Token field */
    uint8_t code;               /* CoAP status code. Can be request (0.xx), success reponse (2.xx),
                                 * client error response (4.xx), or rever error response (5.xx)
                                 * For possible values, see http://tools.ietf.org/html/rfc7252#section-12.1 */
    uint8_t id[2];
} coap_header_t;

typedef struct
{
	uint8_t *p;
	size_t len;
} coap_buffer_t;

typedef struct
{
	uint8_t *num; //PQ PONTEIRO?
	coap_buffer_t buf;
} coap_option_t;

typedef struct
{
	short int cont_t;
	short int cont_op;
	short int cont_p;
} coap_conts;

typedef struct
{
    coap_header_t hdr;          /* Header of the packet */
    coap_buffer_t tok;          /* Token value, size as specified by hdr.tkl */
    uint8_t numopts;            /* Number of options */
    coap_option_t opts[15]; 	/* Options of the packet. For possible entries see
                                 * http://tools.ietf.org/html/rfc7252#section-5.10 */
    coap_buffer_t payload;      /* Payload carried by the packet */
} coap_packet_t;

void monta_pkt (coap_packet_t *pkt, char *buf, coap_conts *cont1)
{
	char *end;
	uint8_t buffer[256];
	//int num_op = strtol((char*)pkt->opts[0].num, &end, 0);
	//buf[0]= 'a';
	//buf[1]= 'a';
	//buf[2]= 'c';
	//buf[3]= 'd';
	//char aux_len[5];
	//printf("buf = %s\n", buf);
	//printf("num = %d, buf.len = %d\n", num_op,pkt->opts[0].buf.len);
	buffer[0] = ((pkt->opts[0].num) << 4) & 0xF0;
    //buffer[0] = (uint8_t ) (((num_op) << 4) & 0xF0);
    //buffer[0] |= (uint8_t )(pkt->opts[0].buf.len & 0x0F);
    /*
    *buffer = *buffer + *pkt->opts[0].num;
    *buffer += pkt->opts[0].buf.len;
    *buffer += *pkt->opts[0].buf.p;*/
    printf("Buffer = 0x%X\n", buffer);

    
	//strcat(buf,pkt->opts[0].num);

	//snprintf(end, 100,"%d",num_op);
	//snprintf(aux_len, 5, "%zu", pkt->opts[0].buf.len);
	//printf("aux_len = %s\n", aux_len);
	//strcat(buf,aux_len);
}

int main ()
{
	coap_packet_t pkt;
	char buf[256];
	//uint8_t numopts = 0;
	coap_conts cont1;
	cont1.cont_op = 2;
	pkt.numopts = 2;
	pkt.opts[0].num =  (uint8_t *)"12";
	pkt.opts[0].buf.p = (uint8_t *)"temperature";
	pkt.opts[0].buf.len = strlen("temperature");
	pkt.opts[1].num =  (uint8_t *)"15";
	pkt.opts[1].buf.p = (uint8_t *)"var";
	pkt.opts[1].buf.len = strlen("var");
	monta_pkt(&pkt,buf,&cont1);
	return 0;
}