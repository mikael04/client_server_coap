/*
 ============================================================================
 Name        : cli_main.c
 Author      : Mikael
 Version     :
 Copyright   : Your copyright notice
 Description : 
 ============================================================================
 */

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

#define FUNC_ARG 0
#define FUNC_STRINGS 1

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
#define DEBUG_PRINT_HEADER 1
#define DEBUG_PRINT_TOKEN 1
#define DEBUG_PRINT_OPTION 1
#define DEBUG_PRINT_OPTION_M 0
#define DEBUG_PRINT_PAYLOAD 1
#define DEBUG_MONTA_OP 0
#define DEBUG_SEPARA_STRING 0
#define DEBUG_ARGS 0
#define DEBUG_PKT_ARG 0


#define num_de_opcoes 30
#define MAX_OPTIONS 10
#define MAX_TAMANHO_TOKEN 16 //char
#define MAX_TAMANHO_OPTION 24 //char
#define MAX_TAMANHO_PAYLOAD 64 //char

#define MAX_LEN_OPTION 32
#define MAX_VALUE_OPTION 64

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
#define erro_argumento_inv
#define erro_falta_argumento 30


	void lida_erro_monta (int erro);
	void lida_erro_add (int erro);
	
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
	    printf("  size  0x%d\n", token->len);
	    printf("  token %s\n", token->p);
	}
	void printf_payload (coap_buffer_t *payload)
	{
		printf("Payload:\n");
	    printf("  size  0x%d\n", payload->len);
	    printf("  payload %s\n", payload->p);
	}

	void printf_option (coap_option_t *opt)
	{
		printf("Option:\n");
	    printf("  num  %d\n", (int)*opt->num);
	    printf("  num  0x%02x\n", *opt->num);
	    printf("  option buf len %d\n", opt->buf.len);
	    printf("  option buf p %s\n", opt->buf.p);
	}

	void printf_buffer(uint8_t *buffer)
	{
		int i;
		for (i=0; i<strlen((char *)buffer); i++)
		{
			//if ()
			printf("buffer [%d] = 0x%02x\n", i, buffer[i]);
		}
	}
	void printf_buffer_m(uint8_t *buffer)
	{
		int i;
		for (i=0; i<strlen((char *)buffer); i++)
		{
			//if ()
			printf("%02x ", buffer[i]);
		}
	}
	void monta_pkt (coap_packet_t *pkt, uint8_t *buf)
	{
		//uint8_t aux_header[4];
		uint8_t *p = NULL;
		uint16_t running_delta = 0;
		
		//max len = 16
		//max delta = 64


		int i=0;
		buf[0] = (pkt->hdr.ver & 0x03) << 6;
	    buf[0] |= (pkt->hdr.t & 0x03) << 4;
	    buf[0] |= (pkt->hdr.tkl & 0xFF);
	    buf[1] = pkt->hdr.code;
	    buf[2] = pkt->hdr.id[0];
	    buf[3] = pkt->hdr.id[1];
	    p = buf+4;
	    if (pkt->hdr.tkl == 0x00)
	    {
	    	//printf("Não existe token\n");
	    	//printf("pkt->hdr.tkl = %d\n",pkt->hdr.tkl);
	    }
	    else
	    {
	    	//printf("pkt->hdr.tkl = %d\n",pkt->hdr.tkl);
	    	//printf("Existe token\n");
	    	memcpy(p, pkt->tok.p, pkt->hdr.tkl);
	    	p = p+pkt->hdr.tkl;
	    }
	    int aux = (int)pkt->numopts;
	    for (i=0; i<aux; i++)
	    {
#if DEBUG && DEBUG_MONTA_OP
			printf("Entrando no for\n");
#endif
#if DEBUG && DEBUG_PRINT_OPTION_M
			printf_option(&pkt->opts[i]);
#endif
			uint8_t len = pkt->opts[i].buf.len;
			uint8_t delta = (int)*pkt->opts[i].num;
			uint8_t op_num_len = 0;
			delta = delta - running_delta;
			running_delta = 2*running_delta + delta;
			uint8_t len_aux = 0, delta_aux = 0;
			//Lida com opções, RFC7252, Page 18
			if(delta>13 && delta<65)
			{
				delta_aux = delta-13;
				delta = 13;
			}
			else if (delta == 15)
			{
				lida_erro_monta(erro_delta_15);
			}
			else if (delta < 0 || delta >= MAX_VALUE_OPTION) //-> MAX_VALUE_OPTION = 64
			{
				lida_erro_monta (erro_delta_invalida);
			}
			
			//Lida com opções, RFC7252, Page 19
			if(len>13 && len<17)
			{
				len_aux = len-13;
				len = 13;
			}
			else if (len == 15)
			{
				lida_erro_monta(erro_len_15);
			}
			else if (len < 0 || len >= MAX_LEN_OPTION) //-> MAX_LEN_OPTION = 16
			{
				lida_erro_monta (erro_len_invalida);
			}
			
			
#if DEBUG && DEBUG_MONTA_OP
			printf("Conteudo de opts[%d]->num = %d\n", i, (int)*pkt->opts[i].num);
			printf("Conteudo de opts[%d]->num = 0x%02x\n", i, *pkt->opts[i].num);
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
			//ACIMA p = p+1;
				printf("Antes no delta = 13\n");
			if(delta==13)
			{
				printf("Entrando no delta = 13\n");
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
				printf("Depois do delta = 13\n");
			memcpy(p, pkt->opts[i].buf.p, pkt->opts[i].buf.len);
			p = p + pkt->opts[i].buf.len;
#if DEBUG && DEBUG_MONTA_OP
			printf("Debug 1");
#endif
	    }
	    //Payload
#if DEBUG && DEBUG_MONTA_OP
			printf("Debug 2");
#endif
	    	*p++ = 0xFF;
#if DEBUG && DEBUG_MONTA_OP
			printf("Debug 3");
#endif
	    	memcpy(p, pkt->payload.p, pkt->payload.len);
#if DEBUG && DEBUG_MONTA_OP
			printf("Debug 4");
#endif
	    	p = p+pkt->payload.len;

	}

	void monta_header (coap_packet_t *pkt)
	{
		//0100 0000 0000 0011 0000 0010 0000 0001
		pkt->hdr.ver = 	 0x01; // versão 01;
		pkt->hdr.t = 	 0x00; // code 0 (confirmable);
		pkt->hdr.tkl = 	 0x00; //Tamanho do token -> para testes, 0
		pkt->hdr.code =  0x03; //request -> 0000 0011 -> PUT
		pkt->hdr.id[0] = 0x02; //0010 -> TESTE
		pkt->hdr.id[1] = 0x01; //0001 -> TESTE

	#if DEBUG && DEBUG_M_HEADER
		printf_header(&pkt->hdr);
	#endif
	}

	void cria_pkt (coap_packet_t *pkt)
	{
		monta_header (pkt);
		pkt->numopts = 0;
	}
	/*void add_token_null (coap_packet_t *pkt)
	{
		//pkt->tok = NULL;
	}*/
	void add_payload (coap_packet_t *pkt, char *payload)
	{
		printf("strlen  = %d\n", strlen(payload));
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
	void add_option (coap_packet_t *pkt, int cont_aux, int *buf_aux_opt_n, char *op_conteudo, int numopt, int *option_running_delta)
	{
		int num_op = *buf_aux_opt_n;
		if (strlen(op_conteudo) > 24)
		{
			lida_erro_add(erro_option_excede_tamanho);
		}
		else
		{
#if DEBUG && DEBUG_ADD_OPTION
			printf("Option num, antes do delta = %d\n", num_op);
#endif
			//num_op = num_op - *option_running_delta;
#if DEBUG && DEBUG_ADD_OPTION
			printf("Option running delta = %d\n", *option_running_delta);
#endif
			//*option_running_delta = num_op + *option_running_delta;
#if DEBUG && DEBUG_ADD_OPTION
			printf("Option num - option_running_delta = %d\n", num_op);
#endif
			*buf_aux_opt_n = num_op;
			pkt->opts[cont_aux].num = (uint8_t *)buf_aux_opt_n;
			pkt->opts[cont_aux].buf.len = strlen(op_conteudo);
			pkt->opts[cont_aux].buf.p = (uint8_t *) op_conteudo;
#if DEBUG && DEBUG_PRINT_OPTION
			printf_option(&pkt->opts[cont_aux]);
#endif
		}
	}
	void add_token_hdr_tkl (coap_packet_t *pkt, char *token)
	{
		//HDR_TKL
		short int tkl;
		tkl = strlen(token);
#if DEBUG && DEBUG_ADD_HDR_TKL
		//printf("tkl = %d\n", tkl);
		//printf("tkl & 0xff = %d\n", (tkl & 0xff));
		/*CONVERSÃO INT -> UINT8_T //não necessária
		uint8_t byte1 = (tkl & 0xff);
		uint8_t byte2 = ((tkl >> 8) & 0xff);
		printf("byte 1 = %d, byte 2 = %X\n", byte1, byte2);*/
#endif
		if (tkl > MAX_TAMANHO_TOKEN)
		{
			lida_erro_add(erro_token_excede_tamanho);
		}
		else
		{
			pkt->hdr.tkl = tkl;
			//printf("pkt->hdr.tkl = %d\n", pkt->hdr.tkl);
#if DEBUG && DEBUG_PRINT_HEADER
		printf_header(&pkt->hdr);
#endif
			//HDR_TKL
			pkt->tok.len = strlen (token);
			pkt->tok.p = (uint8_t *)token;
#if DEBUG && DEBUG_PRINT_TOKEN
			printf_token(&pkt->tok);
#endif
		}
	}

	void lida_erro_add (int erro)
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

	void lida_erro_monta (int erro)
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
	void lida_erro_id(int erro, int argc, char **argv)
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
		int i;
		printf("Mensagem enviada: \n");
		for (i=1; i<argc; i++)
		{
			printf("%s ", argv[i]);
		}
		printf("\n");
		exit (0);
	}

	int veri_token (char *argv1)
	{
		int i;
		for (i=0; i<strlen(argv1); i++)
		{
			//ASCII 48-57
			if (argv1[i] < 48 || (argv1[i] > 57 && argv1[i] < 65) || (argv1[i] > 90 && argv1[i] < 97) || argv1[i] > 122)
			{
				return 1;
			}
		}
		return 0;
	}

	int veri_option (char *argv1, char *argv2)
	{
		int i;
		int cont = 0;
		for (i=0; i<strlen(argv1); i++)
		{
			//ASCII 48-57
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

	int veri_payload (char *argv1)
	{
		int i;
		if (argv1[0] == 43 || argv1[0] == 45)
			return 1;
		for (i=1; i<strlen(argv1); i++)
		{
			//ASCII 48-57
			if (argv1[i] < 48 || (argv1[i] > 57 && argv1[i] < 65) || (argv1[i] > 90 && argv1[i] < 97) || argv1[i] > 122)
			{
				return 1;
			}
		}
		return 0;
	}

	int find_minus_plus (char *argv)
	{
		if (argv[0] == '-' || argv[0] == '+')
		{
			return 1;
		}
		return 0;
	}


	void identifica_arg (coap_packet_t *pkt, int argc, char **argv, char *buf_aux_opt_c, int *buf_aux_opt_n)
	{
		int j;
		char *end;
		int cont_p = 0; // Mesma coisa do Token abaixo
		int cont_op = 0; // Mesma coisa do Token abaixo
		int cont_t = 0; // Apenas um token, caso cont_t > 1 erro de argumento;
		int option_delta = 0; //Option running delta
	#if DEBUG_ID && DEBUG_ID_ARGS
		for (j=1; j<argc; j++)
		{
			printf("argc = %d, j = %d\n", argc, j);
			printf("args = %s\n", argv[j]);
		}
	#endif

		j = 1;
		if(argc < 3)
		{
			lida_erro_id(erro_falta_argumento,argc, argv);
		}
		//cli 
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
			//printf("OUT  = %d\n", j);
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
				//TODO posso especificar esse erro também
				else if (j+2 > argc)
				{
					//printf("erro token 1");
					//printf("Entrei aqui no j+2 -t\n");
					lida_erro_id(erro_argumento_token_invalido, argc, argv);
				}
				//TODO posso dividir esse if para ter mais detalhes do meu erro;
				else if(veri_token(argv[j+1]))
				{
					//printf("erro token 2");
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
				int cont_aux = cont_op - 1;
				if(cont_t == 0)
				{
					//add_token_null (&pkt);
				}
				//TODO posso especificar esse erro também
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
					//add_option (&pkt, argv[j+1], argv[j+2]);
					//Converte string option para inteiro;
					buf_aux_opt_n[cont_op-1] = strtol(argv[j+1], &end, 0);
	#if DEBUG_ID_OP && DEBUG
					//printf("buf_aux = %d\n", buf_aux_opt_n[cont_op-1]);
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
				//printf("cont_p++\n");
				//TODO posso especificar esse erro também
				if (j+2 > argc)
				{
					printf("Nao existe arg\n");
					lida_erro_id(erro_argumento_payload_invalido, argc, argv);
				}
				//TODO mesmo do TOKEN, dividir para especificar
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
		//monta_pkt ()
	}

	void separa_string (char **string_sep, char *buf, int n_str, int len)
{
	int i;
	int count_w = 0, j = 0;
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
int  conta_espc (char *buf)
{
	int i, cont=0;
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

#if DEBUG && DEBUG_PKT_ARG && FUNC_ARG
	int main (int argc, char *argv[])
#endif
#if FUNC_STRINGS
	int main ()
#endif
	{
#if DEBUG && DEBUG_PKT_ARG
#endif
		//Pacote

		//memset(pkt, 0, sizeof(pkt));
#if DEBUG && DEBUG_PKT_ARG && FUNC_ARG
		coap_packet_t pkt1, pkt2;
		uint8_t buffer[512];
		memset(buffer, 0, buf_len);
		int buf_len = sizeof(buffer);
		char buf_aux_opt_c[60] = "";
		int buf_aux_opt_n[10];
		cria_pkt (&pkt1);
		int cont_arg;
		cria_pkt (&pkt2);
#endif
		char **string_sep;

		//CONEXAO
		int fd;
#ifdef IPV6
    	struct sockaddr_in6 cliaddr;
#else /* IPV6 */
    	struct sockaddr_in cliaddr;
#endif /* IPV6 */

#ifdef IPV6
    	fd = socket(AF_INET6,SOCK_DGRAM,0);
#else /* IPV6 */
    	fd = socket(AF_INET,SOCK_DGRAM,0);
#endif /* IPV6 */

    	bzero(&cliaddr,sizeof(cliaddr));
#ifdef IPV6
    	cliaddr.sin6_family = AF_INET6;
    	cliaddr.sin6_addr = in6addr_any;
    	cliaddr.sin6_port = htons(5683);
#else /* IPV6 */
    	cliaddr.sin_family = AF_INET;
    	cliaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    	cliaddr.sin_port = htons(5683);
#endif /* IPV6 */
    	bind(fd,(struct sockaddr *)&cliaddr, sizeof(cliaddr));
		 //Fim de Conexão
		 //Argumentos
		 //Nº de Argumentos - ./cliente
		 /*for (i=0; i<argc; i++)
		 {
		 	args[i] = argv[i];
		 }*/
		 //printf("1)OI");

		 	
#if DEBUG && DEBUG_ARGS && FUNC_ARG
		for (cont_arg = 0; cont_arg <argc; cont_arg++)
		{
		 	printf("Cont_arg = %d, argc = %d\n", cont_arg, argc);
		 	printf("Arg = %s\n", argv[cont_arg]);
	
		}
#endif
#if DEBUG && DEBUG_PKT_ARG
		identifica_arg (&pkt1, argc, argv, buf_aux_opt_c, buf_aux_opt_n);
		monta_pkt(&pkt1, buffer);
		printf_buffer (buffer);
		printf_buffer_m (buffer);
#endif
		printf("\n");
		char buf_in[512], buf_out[512];
		int cont=0;
		//Fim de argumentos
		//Mensagem
		 while (1==1)
		 {
			coap_packet_t pkt3;
			cria_pkt(&pkt3);
			memset(buf_in, 0x00, 512);
			printf("Digite a mensagem:\n");
			int i;
			fgets(buf_in, 512, stdin);
			char buf_aux_opt_c2[60] = "";
			int buf_aux_opt_n2[10];

			int len = strlen(buf_in)-1;
			int n_str = conta_espc (buf_in);
			char **string_sep;
			string_sep = malloc(n_str * sizeof(char*));
			for (i = 0; i<n_str; i++)
		    	string_sep[i] = malloc((20) * sizeof(char));

#if DEBUG && DEBUG_SEPARA_STRING
			printf("Buffer_in = %s\nLen = %d\nNumero de Strings = %d\n", buf_in, len, n_str);
#endif
			separa_string(string_sep, buf_in, n_str, len);

			identifica_arg (&pkt3, n_str, string_sep, buf_aux_opt_c2, buf_aux_opt_n2);
			memset(buf_out, 0x00, 512);
			monta_pkt(&pkt3, (uint8_t *)buf_out);
			printf("Mensagem enviada:\n");
		 	printf_buffer_m ((uint8_t *)buf_out);

			cont++;
			if (cont>5)
			{
				close(fd);
				exit(2);
			}
			size_t rsplen = strlen((char *)buf_out);
			//__fpurge(stdin);
			printf("\n");
			printf("Buffer_out = %s\n", buf_out);
			sendto(fd, buf_out, rsplen, 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr));
			//write(clienteSockfd, buf_out, addrlen);
			for (i=0; i<n_str; i++)
			{
				free(string_sep[i]);
			}
			free(string_sep);

		 }
		 //Fim de envio da mensagem
		return 0;
	}
