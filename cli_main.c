#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include "cli_main.h"

#define PORT 32012

#define PORCENTAGEM 100 /*DEFINE PORCENTAGEM DE MENSAGENS ENVIADAS*/

#define STRING_SERV 1
#define METHOD_PUT 1
#define METHOD_POST 0
#define METHOD_POST_PUT 0 /*Setado para testes de PUT (precisa ser setado junto com POST)*/

#define BILLION  1E9
#define SEC 3 /*SLEEP_TIME*/
#define NSEC 5E8 /*SLEEP_TIME*/

#define DEBUG 1
#define DEBUG_ID 0
#define DEBUG_ID_ARGS 0
#define DEBUG_ID_T 0
#define DEBUG_ID_OP 0
#define DEBUG_ID_P 0
#define DEBUG_ARG_H 0
#define DEBUG_CONT_MINUS 0
#define DEBUG_M_HEADER 0
#define DEBUG_ADD_HDR_TKL 0
#define DEBUG_ADD_OPTION 0
#define DEBUG_ADD_PAYLOAD 0
#define DEBUG_PARAMETROS 0
#define DEBUG_PRINT_HEADER 0
#define DEBUG_PRINT_TOKEN 0
#define DEBUG_PRINT_OPTION 0
#define DEBUG_PRINT_OPTION_M 0
#define DEBUG_PRINT_PAYLOAD 0
#define DEBUG_MONTA_OP 0
#define DEBUG_MONTA_H_T 0
#define DEBUG_SEPARA_STRING 0
#define DEBUG_SEND_MSG 0
#define DEBUG_SEND_MSG_BUFFER 0
#define DEBUG_MSG_RECEIVED 0
#define DEBUG_ARGS 0
#define DEBUG_PKT_ARG 0
#define DEBUG_PUT 0
#define DEBUG_VER_BUF 0
#define DEBUF_IMPRIMIR_BUF 0
#define DEBUG_TIMEOUT 0
#define DEBUG_TOKEN_SETADO 0
#define DEBUG_TIME_ALL 0
#define DEBUG_WHILE 0
#define DEBUG_PRINT_CLI_SERV 0
#define DEBUG_WHILE_ORDER 0

 /*OPÇÕES*/
#define SAIR 1
#define TIME_CONTROL 1/*Se for sempre necessário calcular tempo*/
#define TOKEN_ALEATORIO 1
#define NUM_TIMEOUT 5   /*SE SELECIONAR 0, SE TORNA PUT (NAO TENTA REENVIAR)*/
#define NUM_DE_OPCOES 30
#define MAX_OPTIONS 10
#define MAX_TAMANHO_TOKEN 10 /*char*/
#define MAX_TAMANHO_OPTION 24 /*char*/
#define MAX_TAMANHO_PAYLOAD 64 /*char*/

#define ACK_WAIT_TIMEOUT_SEC 1
#define ACK_WAIT_TIMEOUT_USEC 0

#define MAX_LEN_OPTION 32
#define MAX_VALUE_OPTION 64
#define FUNC_ARG 0 /*Recebendo via argumentos no cmd*/
#define FUNC_STRINGS 1 /*Recebendo via strings digitadas*/


/*Definição de erros*/
#define erro_argumento_invalido 10
#define erro_argumento_token_invalido 11
#define erro_argumento_num_option_invalido 12
#define erro_argumento_payload_invalido 13
#define erro_argumento_mais_de_um_token 14
#define erro_argumento_mais_de_um_payload 15
#define erro_len_15 16
#define erro_len_invalida 17
#define erro_delta_15 18
#define erro_delta_invalida 19
#define erro_comando 20
#define erro_prim_arg_invalido 21
#define erro_seg_arg_invalido 22 
#define erro_payload_mais_ags 23
#define erro_payload_excede_tamanho 24
#define erro_option_excede_tamanho 25
#define erro_token_excede_tamanho 26
#define erro_buffer_cheio 27
#define erro_argumento_inv
#define erro_falta_argumento 30


	void lida_erro_monta (short int erro);
	void lida_erro_add (short int erro);
	void lida_erro_send_msg (short int erro);
	
	void printf_header (coap_header_t *hdr)
	{
		printf("Header:\n");
	    printf("  ver  0x%02X\n", hdr->ver);
	    printf("  t    0x%02X\n", hdr->t);
	    printf("  tkl  0x%02X\n", hdr->tkl);
	    printf("  code 0x%02X\n", hdr->code);
	    printf("  id   0x%02X%02X\n", hdr->id[0], hdr->id[1]);
	}

	void printf_token (coap_buffer_t *token)
	{
		printf("Token:\n");
	    printf("  size  0x%d\n",(int) token->len);
	    printf("  token %s\n", token->p);
	}
	void printf_payload (coap_buffer_t *payload)
	{
		printf("Payload:\n");
	    printf("  size  0x%d\n",(int) payload->len);
	    printf("  payload %s\n", payload->p);
	}

	void printf_option (coap_option_t *opt)
	{
		printf("Option:\n");
	    printf("  num  %d\n", (int)opt->num);
	    printf("  num  0x%02x\n", opt->num);
	    printf("  option buf len %d\n",(int) opt->buf.len);
	    printf("  option buf p %s\n", opt->buf.p);
	}

	void printf_buffer(uint8_t *buffer)
	{
		short int i;
		for (i=0; i<strlen((char *)buffer); i++)
		{
			printf("buffer [%d] = 0x%02x = %c\n", i, buffer[i], buffer[i]);
		}
	}
	void printf_buffer_m(uint8_t *buffer, short int size)
	{
		short int i;
		for (i=0; i<size; i++)
		{
			printf("%02x ", buffer[i]);
		}
	}
	void printf_buffer_str(char buf[][512], int num_linhas)
	{
		int i,j;
		for (i=0; i<num_linhas; i++)
		{		
			printf("\n");
			for (j=0; j<strlen(buf[i]); j++)
			{
				printf("0x%02X ", buf[i][j]);
			}
			printf("\n");
			printf("buffer [%d] = %s \n", i, (char *)buf[i]);
		}
	}

	void tempo_agora (char *tempo);
	void get_time (struct timespec *time_now);
	float calc_time_sub (struct timespec *start, struct timespec *stop);

void buffer_msg (char *buf_out, char *buf_out_p, short int *cont_msg, short int *pos, char buf_str[][512])
{
	int k, m, ult_esc = -1;
#if DEBUG && DEBUG_SEND_MSG_BUFFER
	printf("buf_out send_msg = %s\n", buf_out);
#endif
		/*DECIDIR O QUE FAZER SE BUFFER ESTIVER CHEIO*/
		if(*cont_msg > 7)
		{
			lida_erro_send_msg (erro_buffer_cheio);
		}
		else
		{
#if DEBUG && DEBUG_SEND_MSG
			printf("123\n");
			printf("-2 0X%02X ", *(buf_out_p-2));
			printf("-1 0X%02X ", *(buf_out_p-1));
			printf(" 0X%02X ", *buf_out_p);
			printf("+1 0X%02X", *(buf_out_p+1));
			printf("\n");
			printf("strlen %d\n", strlen(buf_out));
			printf("printf ultima pos -2 = 0x%02X\n", buf_out[strlen(buf_out)-2]);
			printf("printf ultima pos -1 = 0x%02X\n", buf_out[strlen(buf_out)-1]);
			printf("printf ultima pos = 0x%02X\n", buf_out[strlen(buf_out)]);
			printf("printf ultima pos +1 = 0x%02X\n", buf_out[strlen(buf_out)]+1);
#endif

			buf_out_p = buf_out_p + strlen(buf_out);
			*buf_out_p++ = 0x20;
#if DEBUG && DEBUG_SEND_MSG
			printf("213\n");
			printf("-2 0X%02X ", *(buf_out_p-2));
			printf("-1 0X%02X ", *(buf_out_p-1));
			printf(" 0X%02X ", *buf_out_p);
			printf("+1 0X%02X", *(buf_out_p+1));
			printf("\n");
			printf("strlen %d\n", strlen(buf_out));
			printf("printf ultima pos -2 = 0x%02X\n", *(buf_out_p-2));
			printf("printf ultima pos -1 = 0x%02X\n", *(buf_out_p-1));
			printf("printf ultima pos = 0x%02X\n", *(buf_out_p));
			printf("printf ultima pos +1 = 0x%02X\n", *(buf_out_p+1));
#endif

#if DEBUG && DEBUG_SEND_MSG
			printf("321\n");
			printf("-2 0X%02X ", *(buf_out_p-2));
			printf("-1 0X%02X ", *(buf_out_p-1));
			printf(" 0X%02X ", *buf_out_p);
			printf("+1 0X%02X", *(buf_out_p+1));
			printf("\n");
			printf("strlen %d\n", strlen(buf_out));
			printf("printf ultima pos -2 = 0x%02X\n", buf_out[strlen(buf_out)-2]);
			printf("printf ultima pos -1 = 0x%02X\n", buf_out[strlen(buf_out)-1]);
			printf("printf ultima pos = 0x%02X\n", buf_out[strlen(buf_out)]);
			printf("printf ultima pos +1 = 0x%02X\n", buf_out[strlen(buf_out)]+1);
#endif
#if DEBUG && DEBUG_SEND_MSG
			printf("\n");
			printf("buf_out = %s, buf_out len = %d, buf_out size = %d\n", buf_out, strlen((char*)buf_out), sizeof(buf_out));
			for (m = 0; m<(int)strlen((char*)buf_out); m++)
			{
				printf("buf[%d]= 0x%02X\n", m, buf_out[m]);
			}
			printf("\n");
			for (m = 0; m<(int)strlen((char*)buf_out); m++)
			{
				printf("%02X ", buf_out[m]);
			}
			printf("\n");
#endif

#if DEBUG && DEBUG_SEND_MSG
			printf("rsplen = %d\n", (int) rsplen);
#endif
			
			k=1;
			while(k==1)
			{
#if DEBUG && DEBUG_SEND_MSG
				printf("ult_esc = %d", ult_esc);
#endif
				for (m = 0; m<8; m++)
				{
#if DEBUG && DEBUG_VER_BUF
					printf("pos [%d] = %d\n", m, pos[m]);
#endif
				}
				if(ult_esc == 7)
				{
					ult_esc = -1;
				}
				if(pos[ult_esc+k]==0)
				{
#if DEBUG && DEBUG_SEND_MSG
					printf("Ult_esc = %d, cont_msg = %d, pos[ult_esc] = %d\n", ult_esc, *cont_msg, pos[ult_esc+k]);
#endif
					ult_esc++;
					*cont_msg = *cont_msg + 1;;
					pos[ult_esc] = 1;
					memcpy(buf_str[ult_esc], buf_out, 512);
					k++;
#if DEBUG && DEBUG_SEND_MSG
					printf("buf = %s \n", (char*)buf_str[ult_esc]);
					printf("cont_msg = %d\n", *cont_msg);
#endif
					break;
				}
				ult_esc++;
				
			}
#if DEBUG && DEBUG_SEND_MSG
				printf("Saiu do while\n");
#endif
			
		}

int lida_msg_recebida (char *buf_in, char buf_str[][512], short int *cont_msg, short int *pos, struct timespec *time_post, struct timespec *time_start, FILE *pFile)
{
	int i, j;
	float time_calc = 0;
#if DEBUG && DEBUG_MSG_RECEIVED
	printf("\nbuf_in = %s\n", buf_in);
	for (i=0; i<strlen(buf_in); i++)
	{
		printf("0x%02X ", (uint8_t) buf_in[i]);
	}
	printf("\n");
	for (i=0; i<strlen(buf_in); i++)
	{
		printf("%c ",buf_in[i]);
	}
	for (i=0; i<8; i++)
	{

		printf("\npos [%d] = %d,", i, pos[i]);
		printf("buf_str [%d] = ", i);
		printf_buffer_m((uint8_t *)buf_str[i], 9);

	printf("\ncont_msg = %d\n", *cont_msg);
#endif
	for (i=0; i<8; i++)
	{
		if (pos[i] == 1)
		{
	#if DEBUG && DEBUG_MSG_RECEIVED
			printf("buf_in[%d] = 0x%02X\n", i, buf_in[0]);
			printf("buf_str[%d] = 0x%02X\n", i, buf_str[0][0]);
			printf("buf_in[%d] = 0x%02X\n", i+1, buf_in[1]);
			if ((buf_in[1] & 0xFF)== 0x84)
			{
				printf("É igual\n");
			}
			else
			{
				printf("Não é igual\n");
			}
			printf("buf_str[%d] = 0x%02X\n", i+1, buf_str[0][1]);
	#endif

	#if METHOD_POST									/*Resposta(6)*/			
			if (((buf_in[0] & (0xF0)) == 0x60) && ((buf_in[0] & 0X0F) == (buf_str[i][0] & 0x0F)) && ((buf_in[1] & 0xFF )== 0x44))
	#endif
	#if METHOD_PUT
			if (((buf_in[0] & (0xF0)) == 0x60) && ((buf_in[0] & 0X0F) == (buf_str[i][0] & 0x0F)) && ((buf_in[1] & 0xFF )== 0x44))
	#endif
			{
				int cont = 0;
				for (j=0; j<(buf_in[0] & 0x0F); j++)
				{
	#if DEBUG && DEBUG_MSG_RECEIVED
					printf("entrando no for buf_in\n");
					printf("j = %d\n", j);
	#endif
					if(buf_in[2+j] == buf_str[i][2+j])
					{
	#if DEBUG && DEBUG_MSG_RECEIVED
						printf("buf_in = 0x%02X, buf_str = 0x%02X\n", buf_in[2+j], buf_str[i][2+j]);
	#endif
						cont++;
					}
				}
				if (cont == (buf_in[0] & 0x0F))
				{
					printf("Mensagem ACK recebida, buffer limpo\n");
	#if TIME_CONTROL
					get_time(time_post);

		#if DEBUG_TIME_ALL
					printf("time_start.sec = %d, time_start.nsec = %d\ntime_post_ACK.sec = %d, time_post_ACK.nsec = %d", (int)time_start->tv_sec, (int)time_start->tv_nsec, (int)time_post->tv_sec, (int)time_post->tv_nsec);
		#endif
					time_calc = calc_time_sub (time_start, time_post);	    		
		    		printf( "\nHow long to receive ACK in a POST request = %lf\n", time_calc);
		    		if(pFile!=NULL)
					{
						fprintf(pFile, "%lf,", time_calc);
						fprintf(pFile, "Y;\n");
					}
	#endif
				   		
					pos[i] = 0;
					*cont_msg = *cont_msg - 1;
					memset(buf_str[i], 0x00, 512);
					return 1;
				}
			}
			else
			{
				printf("Mensagem não é ACK, mensagem incorreta\n");
				return 0;
			}
		}
	}
	return 0;
}

void tempo_agora (char *tempo)
{
	time_t  t= time(NULL);
	struct tm tm = *localtime(&t);
	snprintf(tempo,42," Hora: ""%d"":""%d"":""%d" " ""Data:""%d""/""%d", tm.tm_hour, tm.tm_min, tm.tm_sec, tm.tm_mday, tm.tm_mon);
	printf("Tempo agora = %s\n", tempo);
}

void get_time (struct timespec *time_now)
{
	if( clock_gettime( CLOCK_REALTIME, time_now) == -1 )
	{
      perror( "clock gettime" );
      exit( EXIT_FAILURE );
    }
}
float calc_time_sub (struct timespec *start, struct timespec *stop)
{
	float tempo_decorrido;
	tempo_decorrido = ( stop->tv_sec - start->tv_sec )
          + ( stop->tv_nsec - start->tv_nsec )
            / BILLION;
	return tempo_decorrido;
}


void n_sleep (struct timespec *sleep_time)
{
	if(nanosleep(sleep_time, NULL) < 0)   
   	{
      	printf("Nano sleep system call failed \n");
      	exit (0);
   	}
}

/*Recebendo mensagem com 00 00, cliente interpreta restante como 0, porém vem payload após 00 00*/
short int corrige_len (uint8_t *buffer, short int size)
{
		short int i = 0;
		short int cont_ff = 0;
		while (i<size && (cont_ff<1 || buffer[i] != 0x00))
		{
			if(buffer[i] == 0xff)
			{
				cont_ff++;
			}
			printf("buffer[%i] = %c\n", i, buffer[i]);
			i++;
		}
		return i;
}
void monta_pkt (coap_packet_t *pkt, uint8_t *buf)
{
	uint8_t *p = NULL;
	uint16_t running_delta = 0;
	
	/*max len = 16*/
	/*max delta = 64*/

	/*Adicionar tempo ao final do pacote*/
	char tempo[100];
	tempo_agora(tempo);
	/*Precisei diminuir o tamanho do restante da mensagem, erro "stack smashing detected"*/
	


	short int i=0;
	buf[0] = (pkt->hdr.ver & 0x03) << 6;
    buf[0] |= (pkt->hdr.t & 0x03) << 4;
    buf[0] |= (pkt->hdr.tkl & 0xFF);
    buf[1] = pkt->hdr.code;
    buf[2] = pkt->hdr.id[0];
    buf[3] = pkt->hdr.id[1];
    p = buf+4;
   	memcpy(p, pkt->tok.p, pkt->hdr.tkl);
    p = p+pkt->hdr.tkl;
    short int aux = (short int)pkt->numopts;
    for (i=0; i<aux; i++)
    {
#if DEBUG && DEBUG_MONTA_OP
		printf("Entrando no for\n");
#endif
#if DEBUG && DEBUG_PRINT_OPTION_M
		printf_option(&pkt->opts[i]);
#endif
		uint8_t len = pkt->opts[i].buf.len;
		uint8_t delta = (int)pkt->opts[i].num;
		uint8_t op_num_len = 0;
		delta = delta - running_delta;
		running_delta = 2*running_delta + delta;
		uint8_t len_aux = 0, delta_aux = 0;
		/*Lida com opções, RFC7252, Page 18*/
		if(delta>13 && delta<65)
		{
			delta_aux = delta-13;
			delta = 13;
		}
		else if (delta == 15)
		{
			lida_erro_monta(erro_delta_15);
		}
		else if (delta < 0 || delta >= MAX_VALUE_OPTION) /*-> MAX_VALUE_OPTION = 64*/
		{
			lida_erro_monta (erro_delta_invalida);
		}
		
		/*Lida com opções, RFC7252, Page 19*/
		if(len>13 && len<17)
		{
			len_aux = len-13;
			len = 13;
		}
		else if (len == 15)
		{
			lida_erro_monta(erro_len_15);
		}
		else if (len < 0 || len >= MAX_LEN_OPTION) /*-> MAX_LEN_OPTION = 16*/
		{
			lida_erro_monta (erro_len_invalida);
		}
		
		
#if DEBUG && DEBUG_MONTA_OP
		printf("Conteudo de opts[%d]->num = %d\n", i, (int)pkt->opts[i].num);
		printf("Conteudo de opts[%d]->num = 0x%02x\n", i, pkt->opts[i].num);
		printf("Conteudo de delta = %d\n", (int)delta);
		printf("Conteudo de delta = 0x%02x\n", delta);
		printf("Conteudo de delta_aux = %d\n", (int)delta_aux);
		printf("Conteudo de delta_aux = 0x%02x\n", delta_aux);
		printf("Conteudo de running_delta = %d\n", (int)running_delta);
		printf("Conteudo de running_delta = 0x%02x\n", running_delta);
		printf("Conteudo de opts[%d]->buf.len = %d\n", i, pkt->opts[i].buf.len);
		printf("Conteudo de opts[%d]->buf.p = %s\n", i, (char *) pkt->opts[i].buf.p);
#endif
		
#if DEBUG && DEBUG_MONTA_OP
		printf("Conteudo de op = %d\n", delta);
#endif
		op_num_len = (0xFF & (delta << 4 | len));
#if DEBUG && DEBUG_MONTA_OP
		printf("antes do *p\n");
#endif
    	*p++ = op_num_len;
		if(delta==13)
		{
#if DEBUG && DEBUG_MONTA_OP
			printf("Entrando no delta == 13\n");
#endif
			*p++=delta_aux;
#if DEBUG && DEBUG_MONTA_OP
			printf("p = %c, 0x%02X, p[-1] = %c, 0x%02X\n", *p, *p, *(p-1),*(p-1));
#endif
		}
		if(len == 13)
		{
			*p++=len_aux;
		}
		memcpy(p, pkt->opts[i].buf.p, pkt->opts[i].buf.len);
		p = p + pkt->opts[i].buf.len;
    }
    /*Payload*/

	*p++ = 0xFF;
	memcpy(p, pkt->payload.p, pkt->payload.len);
	p = p+pkt->payload.len;

	/*Adicionando tempo*/
	memcpy(p, tempo, (int)strlen(tempo));
	p=p+(int)strlen(tempo);

}

void monta_header_token (coap_packet_t *pkt, uint8_t *token)
{
	/*0100 0000 0000 0011 0000 0010 0000 0001*/
	pkt->hdr.ver = 	 0x01; /* versão 01;*/
	pkt->hdr.t = 	 0x00; /* code 0 (confirmable);*/
	/*pkt->hdr.tkl = 	 0x00; /*Tamanho do token -> para testes, 0*/
#if METHOD_POST
	pkt->hdr.code =  0x02; /*request -> 0000 0010 -> POST*/
#endif
#if METHOD_PUT
	pkt->hdr.code =  0x03; /*request -> 0000 0011 -> PUT*/
#endif
#if METHOD_POST_PUT
	pkt->hdr.code =  0x03; /*request -> 0000 0011 -> PUT*/
#endif
	/* GERANDO id aleatoriamente*/
	srand(time(NULL));
	short int var_aux = rand()%254;
	pkt->hdr.id[0] = (uint8_t) var_aux; /*0010 -> TESTE*/
	var_aux = rand()%254;
	pkt->hdr.id[1] = (uint8_t) var_aux; /*0001 -> TESTE*/
	/* GERANDO TOKEN aleatoriamente*/
#if TOKEN_ALEATORIO
	var_aux = rand()%254;
	token[0] = var_aux;
#endif
#if DEBUG && DEBUG_MONTA_H_T
	printf("token = %d, var_aux = %d\n", token[0], var_aux);
#endif
#if TOKEN_ALEATORIO
	var_aux = rand()%254;
	token[1] = var_aux;
#endif
#if DEBUG && DEBUG_MONTA_H_T
	printf("token = %d, var_aux = %d\n", token[1], var_aux);
#endif
#if TOKEN_ALEATORIO
	var_aux = rand()%254;
	token[2] = var_aux;
#endif
#if DEBUG && DEBUG_MONTA_H_T
	printf("token = %d, var_aux = %d\n", token[2], var_aux);
#endif
#if TOKEN_ALEATORIO
	var_aux = rand()%254;
	token[3] = var_aux;
#endif
#if DEBUG && DEBUG_MONTA_H_T
	printf("token = %d, var_aux = %d\n", token[3], var_aux);
#endif
#if TOKEN_ALEATORIO
	var_aux = rand()%254;
	token[4] = var_aux;
#endif
#if DEBUG && DEBUG_TOKEN_SETADO
	token[0] = 0x30;
	token[1] = 0x31;
	token[2] = 0x32;
	token[3] = 0x33;
	token[4] = 0x34;
#endif
#if DEBUG && DEBUG_MONTA_H_T
	printf("token = %d, var_aux = %d\n", token[4], var_aux);
#endif
	pkt->tok.p = token;
	pkt->tok.len = 5;
	pkt->hdr.tkl = pkt->tok.len;

#if DEBUG && DEBUG_M_HEADER
	printf_header(&pkt->hdr);
#endif
}

void cria_pkt (coap_packet_t *pkt, uint8_t *token)
{
	monta_header_token (pkt, token);
	pkt->numopts = 0;
}
void add_payload (coap_packet_t *pkt, char *payload)
{
	if (strlen(payload) > 64)
	{
		lida_erro_add(erro_payload_excede_tamanho);
	}
	else
	{
		pkt->payload.len = strlen(payload);
		pkt->payload.p = (uint8_t *)payload;
#if DEBUG && DEBUG_PRINT_PAYLOAD
		printf_payload(&pkt->payload);
#endif
	}

}
void add_option (coap_packet_t *pkt, short int cont_aux, short int *buf_aux_opt_n, char *op_conteudo, short int numopt, short int *option_running_delta)
{
	short int num_op = *buf_aux_opt_n;
	if (strlen(op_conteudo) > 24)
	{
		lida_erro_add(erro_option_excede_tamanho);
	}
	else
	{
#if DEBUG && DEBUG_ADD_OPTION
		printf("Option num, antes do delta = %d\n", num_op);
#endif
#if DEBUG && DEBUG_ADD_OPTION
		printf("Option running delta = %d\n", *option_running_delta);
#endif
#if DEBUG && DEBUG_ADD_OPTION
		printf("Option num - option_running_delta = %d\n", num_op);
#endif
		*buf_aux_opt_n = num_op;
		pkt->opts[cont_aux].num = *buf_aux_opt_n;
		pkt->opts[cont_aux].buf.len = strlen(op_conteudo);
		pkt->opts[cont_aux].buf.p = (uint8_t *) op_conteudo;
#if DEBUG && DEBUG_PRINT_OPTION
		printf_option(&pkt->opts[cont_aux]);
#endif
	}
}
void add_token_hdr_tkl (coap_packet_t *pkt, char *token)
{
	/*HDR_TKL*/
	short int tkl;
	tkl = strlen(token);
	if (tkl > MAX_TAMANHO_TOKEN)
	{
		lida_erro_add(erro_token_excede_tamanho);
	}
	else
	{
		pkt->hdr.tkl = tkl;
#if DEBUG && DEBUG_PRINT_HEADER
	printf_header(&pkt->hdr);
#endif
		/*HDR_TKL*/
		pkt->tok.len = strlen (token);
		pkt->tok.p = (uint8_t *)token;
#if DEBUG && DEBUG_PRINT_TOKEN
		printf_token(&pkt->tok);
#endif
	}
}
void lida_erro_send_msg (short int erro)
{
	if(erro == erro_buffer_cheio)
	{
		printf("Buffer cheio, erro %d\n", erro);
	}
	exit (0);
}
void lida_erro_add (short int erro)
{
	if (erro == erro_token_excede_tamanho)
	{
		printf("Token excede tamanho máximo %d\n", erro);
	}
	else if (erro == erro_option_excede_tamanho)
	{
		printf("Option excede tamanho máximo %d\n", erro);
	}
	else if (erro == erro_payload_excede_tamanho)
	{
		printf("Payload excede tamanho máximo %d\n", erro);
	}
	exit (0);
}

void lida_erro_monta (short int erro)
{
	if (erro == erro_len_15)
	{
		printf("Len = 15, numero reservado, erro %d\n", erro);
	}
	else if (erro == erro_len_invalida)
	{
		printf("Len invalido, numero reservado, erro %d\n", erro);
	}
	else if (erro == erro_delta_15)
	{
		printf("Delta = 15, numero reservado, erro %d\n", erro);
	}
	else if (erro == erro_delta_invalida)
	{
		printf("Delta inválido, numero reservado, erro %d\n", erro);
	}
	exit (0);
}
void lida_erro_id(short int erro, short int argc, char **argv)
{
	if(erro == erro_argumento_invalido)
	{
		printf("Recebendo argumento(s) inválido(s) GERAL, erro %d\n", erro);
	}
	else if(erro == erro_prim_arg_invalido)
	{
		printf("Recebendo primeiro argumento inválido, erro %d\n", erro);
	}
	else if(erro == erro_seg_arg_invalido)
	{
		printf("Recebendo segundo argumento inválido, erro %d\n", erro);
	}
	else if(erro == erro_argumento_token_invalido)
	{
		printf("Recebendo argumento no token inválido, erro %d\n", erro);
	}
	else if(erro == erro_argumento_num_option_invalido)
	{
		printf("Recebendo argumento num option inválido, erro %d\n", erro);
	}
	else if(erro == erro_argumento_payload_invalido)
	{
		printf("Recebendo argumento no payload inválido, erro %d\n", erro);
	}
	else if(erro == erro_argumento_mais_de_um_token)
	{
		printf("Recebendo mais de um token, erro %d\n", erro);
	}
	else if(erro == erro_argumento_mais_de_um_payload)
	{
		printf("Recebendo mais de um payload, erro %d\n", erro);
	}
	else if(erro == erro_payload_mais_ags)
	{
		printf("Recebendo payload mais de um argumento, erro %d\n", erro);
	}
	else if(erro == erro_falta_argumento)
	{
		printf("Recebendo poucos argumentos, erro %d\n", erro);
	}
	else if(erro == erro_comando)
	{
		printf("Recebendo comando invalido, erro %d\n", erro);
	}
	short int i;
	printf("Mensagem enviada: \n");
	for (i=1; i<argc; i++)
	{
		printf("%s ", argv[i]);
	}
	printf("\n");
	exit (0);
}

short int veri_token (char *argv1)
{
	short int i;
	for (i=0; i<strlen(argv1); i++)
	{
		/*ASCII 48-57*/
		if (argv1[i] < 48 || (argv1[i] > 57 && argv1[i] < 65) || (argv1[i] > 90 && argv1[i] < 97) || argv1[i] > 122)
		{
			return 1;
		}
	}
	return 0;
}

short int veri_option (char *argv1, char *argv2)
{
	short int i;
	short int cont = 0;
	for (i=0; i<strlen(argv1); i++)
	{
		/*ASCII 48-57*/
		if (argv1[i] < 48 || argv1[i] > 57)
		{
			return 1;
		}
	}
	if(cont > 0)
		return 1;
	else
		return 0;
}

short int veri_payload (char *argv1)
{
	short int i;
	if (argv1[0] == 43 || argv1[0] == 45)
		return 1;
	for (i=1; i<strlen(argv1); i++)
	{
		/*ASCII 48-57*/
		if (argv1[i] < 48 || (argv1[i] > 57 && argv1[i] < 65) || (argv1[i] > 90 && argv1[i] < 97) || argv1[i] > 122)
		{
			return 1;
		}
	}
	return 0;
}

short int find_minus_plus (char *argv)
{
	if (argv[0] == '-' || argv[0] == '+')
	{
		return 1;
	}
	return 0;
}


void identifica_arg (coap_packet_t *pkt, int argc, char **argv, char *buf_aux_opt_c, short int *buf_aux_opt_n)
{
	short int j;
	char *end;
	short int cont_p = 0; /* Mesma coisa do Token abaixo*/
	short int cont_op = 0; /* Mesma coisa do Token abaixo*/
	short int cont_t = 0; /* Apenas um token, caso cont_t > 1 erro de argumento;*/
	short int option_delta = 0; /*Option running delta*/
#if DEBUG_ID && DEBUG_ID_ARGS
	for (j=1; j<argc; j++)
	{
		printf("argc = %d, j = %d\n", argc, j);
		printf("args = %s\n", argv[j]);
	}
#endif

	j = 1;
	if (argc <3)
#endif
	{
		lida_erro_id(erro_falta_argumento,argc, argv);
	}
	else if (0 != strcmp(argv[0],"cliente") && 0 != strcmp(argv[0],"cli"))
	{
		lida_erro_id(erro_prim_arg_invalido,argc, argv);
	}
	else if (0 != strcmp(argv[1], "-t") && 0 != strcmp(argv[1], "-op"))
	{
		lida_erro_id(erro_seg_arg_invalido, argc, argv);
	}
	while (j<argc)
	{
#if DEBUG && DEBUG_ID
		printf("argv[%d] = %s, argc = %d, j = %d\n", j, argv[j], argc, j);
		printf("j = %d\n", j);
#endif
		if(strcmp(argv[j], "-t") == 0)
		{
#if DEBUG_ID_T && DEBUG
			printf("entrando no -t \n");
#endif
			cont_t++;
			if (cont_t > 1)
			{
				lida_erro_id(erro_argumento_mais_de_um_token, argc, argv);
			}
			/*TODO posso especificar esse erro também*/
			else if (j+2 > argc)
			{
				lida_erro_id(erro_argumento_token_invalido, argc, argv);
			}
			/*TODO posso dividir esse if para ter mais detalhes do meu erro;*/
			else if(veri_token(argv[j+1]))
			{
				lida_erro_id(erro_argumento_token_invalido, argc, argv);
			}
			else
			{

#if DEBUG_ID_T && DEBUG
				printf("1t)argv[j] = -t\n");
				printf("2t)argv[j] = %s, ", argv[j]);
				printf("3t)argv[j+1] = %s\n", argv[j+1]);
				printf("4t) j = %d\n", j);
#endif
				add_token_hdr_tkl(pkt, argv[j+1]);
				j+=2;
			}
		}
		else if(strcmp(argv[j], "-op") == 0)
		{
			cont_op++;
			pkt->numopts++;
			short int cont_aux = cont_op - 1;
			/*TODO posso especificar esse erro também
			if (j+3 > argc)
			{
				lida_erro_id(erro_argumento_num_option_invalido, argc, argv);
			}
			else if(veri_option (argv[j+1], argv[j+2]))
			{
				lida_erro_id(erro_argumento_num_option_invalido, argc, argv);
			}
			else
			{
				/*Converte string option para inteiro;*/
				buf_aux_opt_n[cont_op-1] = strtol(argv[j+1], &end, 0);
#if DEBUG_ID_OP && DEBUG
				printf("1op)argv[j] = -op\n");
				printf("2op)argv[j] = %s, ", argv[j]);
				printf("3op)argv[j+1] = %s\n", argv[j+1]);
				printf("4op) j = %d\n", j);
#endif
				add_option(pkt, cont_aux, &buf_aux_opt_n[cont_op-1], argv[j+2], cont_op, &option_delta);
				j+=3;
			}
		}
		else if(strcmp(argv[j], "-p") == 0)
		{
			cont_p++;
			/*TODO posso especificar esse erro também*/
			if (j+2 > argc)
			{
				printf("Nao existe arg\n");
				lida_erro_id(erro_argumento_payload_invalido, argc, argv);
			}
			/*TODO mesmo do TOKEN, dividir para especificar*/
			else if(cont_p > 1)
			{
				printf("Mais de um payload\n");
				lida_erro_id(erro_argumento_payload_invalido, argc, argv);
			}
			else
			{

#if DEBUG_ID_P && DEBUG
				printf("1p)argv[j] = -p\n");
				printf("2p)argv[j] = %s, ", argv[j]);
				printf("3p)argv[j+1] = %s\n", argv[j+1]);
				printf("4p) j = %d\n", j);
#endif
				add_payload(pkt, argv[j+1]);
				j+=2;
			}
		}
		else if (cont_p > 0)
		{
			lida_erro_id(erro_payload_mais_ags, argc, argv);
		}
		else if(cont_p == 0 && cont_op == 0)
		{
			lida_erro_id (erro_comando, argc, argv);
		}
	}
	/*monta_pkt ()*/
}

void separa_string (char **string_sep, char *buf, short int n_str, short int len)
{
	short int i;
	short int count_w = 0, j = 0;
	for (i=0; i<len; i++)
	{
#if DEBUG && DEBUG_SEPARA_STRING
		printf("buf[%d] = %c\n", i, buf[i]);
		printf ("Count_w = %d, j = %d, i = %d\n", count_w, j, i);
#endif
		if (buf[i] == ' ')
		{

			string_sep[count_w][j] = '\0';
			count_w++;
			j=0;
		}
		else
		{
			string_sep[count_w][j] = buf[i];
			j++;
		}
	}
#if DEBUG && DEBUG_SEPARA_STRING
	printf("String 1 = %s\n", string_sep[0]);
	printf("String 2 = %s\n", string_sep[1]);
	for (i=0; i<n_str; i++)
	{
		printf("1)String %d = %s\n", i, string_sep[i]);
	}
#endif
}
short int  conta_espc (char *buf)
{
	short int i, cont=0;
	if (buf[0] == ' ')
	{
		printf("Erro\n");

	}
	for (i=0; i<strlen(buf); i++)
	{
		if((buf[i] == ' ' && buf[i-1] != ' ')|| buf[i] == 0x0a)
		{
			cont++;
		}
	}
	return cont;
}

#if DEBUG && DEBUG_PKT_ARG
int main (int argc, char *argv[])
#endif
#if FUNC_STRINGS
int main_cli ()
#endif
{
#if DEBUG && DEBUG_PKT_ARG
#endif
	/*Tempo*/
	struct timespec time_start; /*Só para não precisar alterar a função lida_msg*/
	
#if METHOD_POST
	#if METHOD_POST_PUT
		struct timespec time_put;
	#else
		struct timespec time_post, time_resend, time_post_send;	
	#endif
#else
#if METHOD_PUT
	struct timespec time_put;
#endif
#endif
#if TIME_CONTROL
	get_time(&time_start);
#endif

	/*memset(pkt, 0, sizeof(pkt));*/
#if DEBUG && DEBUG_PKT_ARG && FUNC_ARG
	coap_packet_t pkt1, pkt2;
	uint8_t buffer[512];
	int buf_siz = sizeof(buffer);
	memset(buffer, 0, buf_siz);
	char buf_aux_opt_c[60] = "";
	short int buf_aux_opt_n[10];
	cria_pkt (&pkt1);
	short int cont_arg;
	cria_pkt (&pkt2);
#endif
	/*Salvar logs em txt*/
	FILE * pFile;
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	char date[50];
#if METHOD_PUT
	snprintf(date,50, "Dia=%d_Metodo=PUT_Porcentagem=%d",tm.tm_mday, PORCENTAGEM);
#else
	#if METHOD_POST
		#if METHOD_POST_PUT
	snprintf(date,50, "Dia=%d_Metodo=PUT_Porcentagem=%d",tm.tm_mday, PORCENTAGEM);
		#else
			snprintf(date,50, "Dia=%d_Metodo=POST_Porcentagem=%d",tm.tm_mday, PORCENTAGEM);
		#endif
	
	#endif
#endif
	pFile = fopen (date,"a+");
	/*VARIAVEL ALEATORIA DE ENVIO
	srand (time(NULL)); /*RANDOM FUNCTION*/

	/*CONEXAO*/
	int fd;

	struct sockaddr_in cliaddr;
	fd = socket(AF_INET,SOCK_DGRAM,0);
	bzero(&cliaddr,sizeof(cliaddr));
#if METHOD_POST
	socklen_t szcliaddr = sizeof(cliaddr);
#endif
	cliaddr.sin_family = AF_INET;
	cliaddr.sin_addr.s_addr = inet_addr("192.168.0.103");
	cliaddr.sin_port = htons(32011);
	bind(fd,(struct sockaddr *)&cliaddr, sizeof(cliaddr));
	 	
#if DEBUG && DEBUG_ARGS && FUNC_ARG
	for (cont_arg = 0; cont_arg <argc; cont_arg++)
	{
	 	printf("Cont_arg = %d, argc = %d\n", cont_arg, argc);
	 	printf("Arg = %s\n", argv[cont_arg]);

	}
#endif
#if DEBUG && DEBUG_PKT_ARG && FUNC_ARG
	identifica_arg (&pkt1, argc, argv, buf_aux_opt_c, buf_aux_opt_n);
	monta_pkt(&pkt1, buffer);
	printf_buffer (buffer);
	printf_buffer_m (buffer, buf_siz));
#endif
	
	/*TIMEOUT*/
#if METHOD_POST

	char buf_str[8][512];
	struct timeval tv;
	tv.tv_sec = ACK_WAIT_TIMEOUT_SEC;
   	tv.tv_usec = ACK_WAIT_TIMEOUT_USEC;
   	#if METHOD_POST_PUT
	#else
   		short int num_timeouts = 0;
   	#endif
#endif
	printf("\n");
	char buf_in[512], buf_out[512];
	char string_aux[50];
	strncat(string_aux, "cli -op 11 var -op 11 temperature -p 0810D",50);
	int str_len = strlen(string_aux);
	string_aux[str_len] = '\n';
	string_aux[str_len+1] = '\0';
#if DEBUG && DEBUG_VER_BUF
	char op[3];
#endif
	/*Variável utilizada para salvar tempo de envio;*/
	float time_calc = 0;
	/*Fim de argumentos*/

	
#if METHOD_POST
	short int cont_msg = 0;
	short int pos[8] = {0, 0, 0, 0, 0, 0, 0, 0};
#endif
	/*Mensagem*/
 	/*buffer*/
	coap_packet_t pkt3;
	uint8_t token[5];
	cria_pkt(&pkt3, token);
	memset(buf_out, 0x00, 512);
	memset(buf_in, 0x00, 512);
#if METHOD_POST
	#if METHOD_POST_PUT
	#else
		num_timeouts = 0;
	#endif
#endif

#if STRING_SERV
	strncat(buf_out, string_aux, 511);				
#if DEBUG && DEBUG_PRINT_CLI_SERV 
	printf_buffer((uint8_t *)buf_out);
#endif
#else 
	fflush(stdin);
	printf("Digite a mensagem:\n");
	fgets(buf_out, 512, stdin);
	get_time(&time_start); /*Tempo -> Após digitar mensagem*/
#if DEBUG && DEBUG_PRINT_CLI_SERV 
	printf_buffer((uint8_t *)buf_out);
#endif
#endif
	
#if METHOD_POST
	char *buf_out_p = buf_out;
#endif

	short int i;
	char buf_aux_opt_c2[60] = "";
	short int buf_aux_opt_n2[10];

	short int len = strlen(buf_out)-1;
	short int n_str = conta_espc (buf_out);
	char **string_sep;
	string_sep = malloc(n_str * sizeof(char*));
	for (i = 0; i<n_str; i++)
    	string_sep[i] = malloc((20) * sizeof(char));

#if DEBUG && DEBUG_SEPARA_STRING
	printf("Buffer_in = %s\nLen = %d\nNumero de Strings = %d\n", buf_out, len, n_str);
#endif
	separa_string(string_sep, buf_out, n_str, len);

	identifica_arg (&pkt3, n_str, string_sep, buf_aux_opt_c2, buf_aux_opt_n2);
#if SAIR	
	if(0 == strcmp(buf_out, "sair\n\0"))
	{
		printf("Saindo 2\n");
		sendto(fd, "sair", (size_t)6, 0, (struct sockaddr *) &cliaddr, sizeof(cliaddr));
		close(fd);
		return 0;
	}
#endif
	memset(buf_out, 0x00, 512);
	monta_pkt(&pkt3, (uint8_t *)buf_out);

	printf("Mensagem enviada:\n");
 	printf_buffer_m ((uint8_t *)buf_out, strlen(buf_out));
	size_t rsplen = strlen(buf_out);
#if DEBUG && DEBUG_SEND_MSG
	printf("buf_out [0] = %c, 0x%02X", *buf_out_p, *buf_out_p);
#endif
	printf("\n");
	printf("Buffer_out = %s\n", buf_out);
#if DEBUG && DEBUG_SEND_MSG
	printf("cont_msg = %d\n",cont_msg);
#endif

#if METHOD_POST
	if (rand()%100<PORCENTAGEM)
    {
        printf("\n\nSending\n\n");
        sendto(fd, buf_out, rsplen, 0, (struct sockaddr *) &cliaddr, sizeof(cliaddr));
    }
    else
    {
        printf("\n\nNot sending, simulando erro de comunicação\n\n");
    }
    #if METHOD_POST_PUT
    #else
		buffer_msg (buf_out, buf_out_p, &cont_msg,  pos, buf_str);
	#endif
#else
	#if METHOD_PUT
	if (rand()%100<PORCENTAGEM)
    {
        printf("\n\nSending\n\n");
        sendto(fd, buf_out, rsplen, 0, (struct sockaddr *) &cliaddr, sizeof(cliaddr));
    }
    else
    {
        printf("\n\nNot sending, simulando erro de comunicação\n\n");
    }
		
	#endif
#endif
#if TIME_CONTROL
	#if METHOD_PUT
		time_put.tv_sec = 0;
   		time_put.tv_nsec = 0;				   		
   		get_time (&time_put);
		#if DEBUG && DEBUG_TIME_ALL
				printf("time_start.sec = %d, time_start.nsec = %d\ntime_put.sec = %d, time_put.nsec = %d", (int)time_start.tv_sec, (int)time_start.tv_nsec, (int)time_put.tv_sec, (int)time_put.tv_nsec);
		#endif
		time_calc = calc_time_sub (&time_start, &time_put);
		printf( "\nHow long to send a PUT request = %lf\n", time_calc);
		if(pFile!=NULL)
		{
			fprintf(pFile, "%lf\n", time_calc);
		}
	#endif
	#if METHOD_POST
		#if METHOD_POST_PUT /*Implementado para testes de perda de pacote*/
			time_put.tv_sec = 0;
	   		time_put.tv_nsec = 0;				   		
	   		get_time (&time_put);
			#if DEBUG && DEBUG_TIME_ALL
					printf("time_start.sec = %d, time_start.nsec = %d\ntime_put.sec = %d, time_put.nsec = %d", (int)time_start.tv_sec, (int)time_start.tv_nsec, (int)time_put.tv_sec, (int)time_put.tv_nsec);
			#endif
			time_calc = calc_time_sub (&time_start, &time_put);
			printf( "\nHow long to send a PUT request = %lf\n", time_calc);
			if(pFile!=NULL)
			{
				fprintf(pFile, "%lf", time_calc);
			}
		#else /* METHOD POST -> para testes, caso não seja teste remover IF*/
			time_post_send.tv_sec = 0;
	   		time_post_send.tv_nsec = 0;				   		
	   		get_time (&time_post_send);
			#if DEBUG && DEBUG_TIME_ALL
					printf("time_start.sec = %d, time_start.nsec = %d\ntime_post.sec = %d, time_post.nsec = %d", (int)time_start.tv_sec, (int)time_start.tv_nsec, (int)time_post_send.tv_sec, (int)time_post_send.tv_nsec);
			#endif
			time_calc = calc_time_sub (&time_start, &time_post_send);
			printf( "\nHow long to send a POST request = %lf\n", time_calc);
			if(pFile!=NULL)
			{
				fprintf(pFile, "%lf,", time_calc);
			}
		#endif	
	#endif
#endif

#if DEBUG && DEBUG_VER_BUF && METHOD_POST
	printf("1)");
	printf_buffer_str (buf_str, 8);
#endif
#if METHOD_POST
	#if METHOD_POST_PUT
		if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) 
	   	{
	   		perror("Error");
	   	}
	   	else if (recvfrom(fd, buf_in, rsplen, 0, (struct sockaddr *)&cliaddr, &szcliaddr) >= 0)
		{
			printf( "\nACK PUT request received\n");
   			if(pFile!=NULL)
			{
				fprintf(pFile, ",Y;\n");
			}
	   		
		}
		else
   		{
   			printf( "\nACK PUT request NOT received\n");
   			if(pFile!=NULL)
			{
				fprintf(pFile, ",N;\n");
			}
   		}	
	#else /*METHOD POST*/
		while (num_timeouts < NUM_TIMEOUT)
	   	{
		   	if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) 
		   	{
		   		perror("Error");
		   	}		
		   	else if (recvfrom(fd, buf_in, rsplen, 0, (struct sockaddr *)&cliaddr, &szcliaddr) >= 0)
		   	{

	#if DEBUG && DEBUG_WHILE_ORDER
		   		printf("Entrando no ELSE \n");
	#endif
		   		if (1 == lida_msg_recebida (buf_in, buf_str, &cont_msg, pos, &time_post, &time_start, pFile))
		   		{
		   			num_timeouts = NUM_TIMEOUT;
		   		}
	#if DEBUG && DEBUG_VER_BUF
		   		printf("2)");
		   		printf_buffer_str (buf_str, 8);
	#endif
	#if DEBUG && DEBUG_TIMEOUT
		   		printf("msg received = ");
		   		printf("buf in:  %s, len = %d\n", buf_in, strlen(buf_in));
		   		int len_buf_in = corrige_len((uint8_t *)buf_in, sizeof(buf_in));
		   		printf("msg received = ");
		   		printf("buf in:  %s, len = %d\n", buf_in, len_buf_in);
		   		printf_buffer_m((uint8_t *)buf_in, len_buf_in);
		   		printf("\n");
	#endif			   		
		   }
		   
		   else
		   {


		   		printf("Entrando no não recebida msg, resending\n");
	#if DEBUG && DEBUG_TIMEOUT
		   		printf("buf in = %s\n", buf_in);
	#endif
		   		if (rand()%100<PORCENTAGEM)
	            {
	                printf("\n\nSending\n\n");
	                sendto(fd, buf_out, rsplen, 0, (struct sockaddr *) &cliaddr, sizeof(cliaddr));
	            }
	            else
			    {
			        printf("\n\nNot sending, simulando erro de comunicação\n\n");
			    }
	#if TIME_CONTROL
				time_resend.tv_sec = 0;
		   		time_resend.tv_nsec = 0;				   		
		   		get_time (&time_resend);
	#if DEBUG && DEBUG_TIME_ALL
				printf("time_start.sec = %d, time_start.nsec = %d\ntime_resend.sec = %d, time_resend.nsec = %d", (int)time_start.tv_sec, (int)time_start.tv_nsec, (int)time_resend.tv_sec, (int)time_resend.tv_nsec);
	#endif
				time_calc = calc_time_sub (&time_start, &time_resend);
				printf( "\nHow long to resend a POST request = %lf\n", time_calc);
				if(pFile!=NULL)
				{
					fprintf(pFile, "%lf,", time_calc);
				}
	#endif
		   		printf("Timeout reached. Resending segment %d\n", num_timeouts);
		   		
				num_timeouts++;
		   		if(num_timeouts == NUM_TIMEOUT)
		   		{
		   			printf("Programa fechando, não obteve resposta do servidor\n");
		   			if(pFile!=NULL)
					{
						fprintf(pFile, "N;");
						fprintf(pFile, "\n");
					}
		   		}
		   }
		}
	#endif
#endif

	for (i=0; i<n_str; i++)
	{
		free(string_sep[i]);
	}
	free(string_sep);

	close(fd);
	fclose(pFile);
	/*Fim de envio da mensagem*/
	return 0;
}
