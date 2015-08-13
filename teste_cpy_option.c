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

void printf_buffer(uint8_t *buffer)
	{
		int i;
		for (i=0; i<strlen((char *)buffer); i++)
		{
			//if ()
			printf("buffer [%d] = 0x%02x - %c\n", i, buffer[i], buffer[i]);
		}
	}

void monta_pkt (coap_packet_t pkt, uint8_t *buf, coap_conts *cont1)
{
	char *end;
	uint8_t *op = NULL;
	//int num_op = strtol((char*)pkt->opts[0].num, &end, 0);
	buf[0]= 'a';
	buf[1]= 'a';
	buf[2]= 'c';
	buf[3]= 'd';
	int i;
	op = (uint8_t *) buf+4;
	//*p++ = (0xFF & (*pkt->opts[i].num << 4 | pkt->opts[i].buf.len));
	/*
	*po++ = (0xFF & (*pkt.opts[0].num << 4 | pkt.opts[0].buf.len));
	memcpy(po, pkt.opts[0].buf.p, pkt.opts[0].buf.len);
	po = po+pkt.opts[0].buf.len;
	*po++ = (0xFF & (*pkt.opts[1].num << 4 | pkt.opts[1].buf.len));
	memcpy(po, pkt.opts[1].buf.p, pkt.opts[1].buf.len);*/
	for (i=0; i<pkt.numopts; i++)
	{
	   	uint8_t len = pkt.opts[i].buf.len;
	   	int num = (int)strtol((char *)pkt.opts[i].num,&end,0);
	    uint8_t opt;
	    opt = (uint8_t) num;
	    /*
		// *op++ = (0xFF & (*pkt.opts[i].num << 4 | pkt.opts[i].buf.len));
		printf("i = %d, opt = 0x%02x\n", i, *opt);
		printf("i = %d, opt = 0x%c\n", i, *opt);
		printf("i = %d, len = 0x%02x\n", i, len);
		printf("i = %d, opt & F = %0x02x\n", i, ((*opt << 4) & 0xFF));
		printf("i = %d, len & F = %0x02x\n", i, (len & 0x0F));
		*/
		printf("i = %d, opt = 0x%02x\n", i, opt);
		printf("i = %d, num opt = 0x%02x\n", i, (uint8_t) *pkt.opts[i].num);
		printf("i = %d, num opt & F = 0x%02x\n", i, ((*pkt.opts[i].num << 4) & 0xFF));
		printf("i = %d, ((opt << 4) | len ) & 0xFF = 0x%02x\n", i, (0xFF & (opt << 4 | len)));
		
		opt = (0xFF & (opt << 4 | len));
		printf("final opt = 0x%02x\n", opt);
		*op = opt;
		op = op+1;
		memcpy(op, pkt.opts[i].buf.p, pkt.opts[i].buf.len);
		op = op + pkt.opts[i].buf.len;
	}
	//char aux_len[5]; 
	//printf("buf = %s\n", buf);
	//printf("num = %d, buf.len = %d\n", num_op,pkt->opts[0].buf.len);
	//buffer[0] = ((pkt->opts[0].num) << 4) & 0xF0;
    //buffer[0] = (uint8_t ) (((num_op) << 4) & 0xF0);
    //buffer[0] |= (uint8_t )(pkt->opts[0].buf.len & 0x0F)
    /*
    *buffer = *buffer + *pkt->opts[0].num;
    *buffer += pkt->opts[0].buf.len;
    *buffer += *pkt->opts[0].buf.p;*/


    printf_buffer(buf);
    printf("Buffer = %s\n", buf);

    
	//strcat(buf,pkt->opts[0].num);

	//snprintf(end, 100,"%d",num_op);
	//snprintf(aux_len, 5, "%zu", pkt->opts[0].buf.len);
	//printf("aux_len = %s\n", aux_len);
	//strcat(buf,aux_len);
}

int main ()
{
	coap_packet_t pkt;
	uint8_t buf[256] = "";
	//uint8_t numopts = 0;
	coap_conts cont1;
	cont1.cont_op = 2;
	pkt.numopts = 3;
	pkt.opts[0].num =  (uint8_t *)"2";
	pkt.opts[0].buf.p = (uint8_t *)"temperature";
	pkt.opts[0].buf.len = strlen("temperature");
	pkt.opts[1].num =  (uint8_t *)"4";
	pkt.opts[1].buf.p = (uint8_t *)"var";
	pkt.opts[1].buf.len = strlen("var");
	pkt.opts[2].num =  (uint8_t *)"9";
	pkt.opts[2].buf.p = (uint8_t *)"asd";
	pkt.opts[2].buf.len = strlen("asd");
	monta_pkt(pkt,buf,&cont1);
	return 0;
}