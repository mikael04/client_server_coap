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
	#define DEBUG_PRINT_OPTION_M 1
	#define DEBUG_PRINT_PAYLOAD 1

	#define num_de_opcoes 30
	#define MAX_OPTIONS 10

	#define erro_argumento_invalido 10
	#define erro_argumento_token_invalido 11
	#define erro_argumento_option_invalido 12
	#define erro_argumento_payload_invalido 13
	#define erro_comando 20
	#define erro_argumento_inv
		
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
	void monta_pkt (coap_packet_t *pkt, uint8_t *buf, coap_conts *cont1)
	{
		//uint8_t aux_header[4];
		uint8_t *p = NULL;
		//char *end;
		//char aux_options[5];


		int i=0;
		buf[0] = (pkt->hdr.ver & 0x03) << 6;
	    buf[0] |= (pkt->hdr.t & 0x03) << 4;
	    buf[0] |= (pkt->hdr.tkl & 0xFF);
	    buf[1] = pkt->hdr.code;
	    buf[2] = pkt->hdr.id[0];
	    buf[3] = pkt->hdr.id[1];
	    p = buf+4;
	    //aux_option = aux_header;
	    printf_token(&pkt->tok);
	    if (pkt->hdr.tkl == 0x00)
	    {
	    	printf("Não existe token\n");
	    	printf("pkt->hdr.tkl = %d\n",pkt->hdr.tkl);
	    }
	    else
	    {
	    	printf("pkt->hdr.tkl = %d\n",pkt->hdr.tkl);
	    	printf("Existe token\n");
	    	memcpy(p, pkt->tok.p, pkt->hdr.tkl);
	    }
	    //printf("p = %s\n", p);
	    //printf("numopts = %d\n", (int)pkt->numopts);
	    int aux = (int)pkt->numopts;
	    //printf("aux = %d\n", aux);
	    //strncat (aux_options, (char *)pkt->numopts, 5);
	    //printf("aux_options = %s\n", aux_options);
		//aux_options = pkt->numopts;
		//int num_opt = strtol((char *)pkt->numopts,&end,0);
	    //printf("numopt = %d\n", num_opt);
	    for (i=0; i<aux; i++)
	    {
#if DEBUG && DEBUG_MONTA_OP
			printf("Entrando no for\n");
#endif
#if DEBUG && DEBUG_PRINT_OPTION_M
			printf_option(&pkt->opts[i]);
#endif
			uint8_t len = pkt->opts[i].buf.len;
#if DEBUG && DEBUG_MONTA_OP			
			printf("antes do strtol\n");
			//int num = (int)strtol((char *)pkt->opts[i].num,&end,0);
			printf("depois do strtol\n");
#endif
			uint8_t opt;
#if DEBUG && DEBUG_MONTA_OP			
			printf("Conteudo de opts[%d]->num = %d\n", i, (int)*pkt->opts[i].num);
			printf("Conteudo de opts[%d]->num = 0x%02x\n", i, *pkt->opts[i].num);
			printf("Conteudo de opts[%d]->num = %s\n", i, pkt->opts[i].num);
#endif
			opt = (int)*pkt->opts[i].num;
#if DEBUG && DEBUG_MONTA_OP			
			printf("Conteudo de op = %d\n", opt);
#endif
			opt = (0xFF & (opt << 4 | len));
#if DEBUG && DEBUG_MONTA_OP			
			printf("antes do *p\n");
#endif
	    	//aux_option = aux_option + pkt->opts[i].num;
	    	//aux_option = aux_option + pkt->opts[i].buf.len;
	    	//strcat (aux_options, (char *)pkt->opts[i].buf.len);
	    	//uint8_t len = pkt->opts[i].buf.len;
	    	//uint8_t opt = *pkt->opts[i].num;
	    	*p = opt;
			p = p+1;
			memcpy(p, pkt->opts[i].buf.p, pkt->opts[i].buf.len);
			p = p + pkt->opts[i].buf.len;
#if DEBUG && DEBUG_MONTA_OP			
			printf("saindo do for \n");
#endif
	    }

	}

	void monta_header (coap_packet_t *pkt)
	{
		//0100 0000 0000 0011 0000 0010 0000 0001
		pkt->hdr.ver = 	 0x01; // versão 01;
		pkt->hdr.t = 	 0x00; // code 0 (confirmable);
		pkt->hdr.tkl = 	 0x00; //Tamanho do token -> para testes, 0
		pkt->hdr.code =  0x03; //request -> 0000 0011
		pkt->hdr.id[0] = 0x02; //0010 -> TESTE
		pkt->hdr.id[1] = 0x01; //0001 -> TESTE

	#if DEBUG && DEBUG_M_HEADER
		printf_header(&pkt->hdr);
	#endif
	}

	void cria_pkt (coap_packet_t *pkt, coap_conts *cont1)
	{
		monta_header (pkt);
		pkt->numopts = 0;
		cont1->cont_t = 0;
		cont1->cont_op = 0;
		cont1->cont_p = 0;
	}
	/*void add_token_null (coap_packet_t *pkt)
	{
		//pkt->tok = NULL;
	}*/
	void add_payload (coap_packet_t *pkt, char *payload)
	{
		pkt->payload.len = strlen(payload);
		pkt->payload.p = (uint8_t *)payload;

	#if DEBUG && DEBUG_PRINT_PAYLOAD
		printf_payload(&pkt->payload);
	#endif

	}
	//void add_option (coap_packet_t *pkt, char *op_num, char *op_conteudo, int numopt, int *option_running_delta)
	void add_option (coap_packet_t *pkt, int cont_aux, char *op_num, char *op_conteudo, int numopt, int *option_running_delta)
	{
		char *end;
		char aux[30] = "";
		strncat (aux, op_conteudo, strlen(op_conteudo));
		int num_op = strtol(op_num, &end, 0);
	#if DEBUG && DEBUG_ADD_OPTION
		printf("Option num, antes do delta = %d\n", num_op);
		printf("op_conteudo = %s\n",aux);
	#endif	
		num_op = num_op - *option_running_delta;
	#if DEBUG && DEBUG_ADD_OPTION
		printf("Option running delta = %d\n", *option_running_delta);
	#endif
		*option_running_delta = num_op + *option_running_delta;
		snprintf(end, 100,"%d",num_op);
	#if DEBUG && DEBUG_ADD_OPTION
		printf("Option num - option_running_delta = %d\n", num_op);
	#endif
		//opt.num = (uint8_t *) end;
		*pkt->opts[cont_aux].num = (uint8_t)num_op;
		pkt->opts[cont_aux].buf.len = strlen(aux);
		pkt->opts[cont_aux].buf.p = (uint8_t *) aux;
		printf("op->num = 0x%02x\n", *pkt->opts[cont_aux].num);
		/*pkt->opts[numopt-1]->num = aux;
		printf("pkt->opts[numopt-1].num = %d\n", *pkt->opts[numopt-1]->num);
		pkt->opts[numopt-1].buf.len = strlen(op_conteudo);
		pkt->opts[numopt-1].buf.p = (uint8_t *) op_conteudo;*/

	#if DEBUG && DEBUG_PRINT_OPTION	
		printf_option(&pkt->opts[cont_aux]);
	#endif

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
		pkt->hdr.tkl = tkl;
		printf("pkt->hdr.tkl = %d\n", pkt->hdr.tkl);
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



	void lida_erro(int erro, int argc, char **argv)
	{
		if(erro == 10)
		{
			printf("Recebendo argumento(s) inválido(s) GERAL\n");
		}
		else if(erro == 11)
		{
			printf("Recebendo argumento no token inválido\n");
		}
		else if(erro == 12)
		{
			printf("Recebendo argumento na option inválido\n");
		}
		else if(erro == 13)
		{
			printf("Recebendo argumento no payload inválido\n");
		}
		else if(erro == 20)
		{
			printf("Recebendo comando invalido\n");
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


	void identifica_arg (coap_packet_t *pkt, int argc, char **argv, coap_conts *cont1)
	{
		int j;
		/*int cont_p = 0; // Mesma coisa do Token abaixo
		int cont_op = 0; // Mesma coisa do Token abaixo
		int cont_t = 0; // Apenas um token, caso cont_t > 1 erro de argumento;*/
		int option_delta = 0; //Option running delta
	#if DEBUG_ID && DEBUG_ID_ARGS
		for (j=1; j<argc; j++)
		{
			printf("argc = %d, j = %d\n", argc, j);
			printf("args = %s\n", argv[j]);
		}	
	#endif

		j = 1;
		while (j<argc)
		{
			//printf("argv[%d] = %s, argc = %d, j = %d\n", j, argv[j], argc, j);
			//printf("j = %d\n", j);
			//printf("OUT  = %d\n", j);
			if(strcmp(argv[j], "-t") == 0)
			{
				cont1->cont_t++;
				//TODO posso especificar esse erro também
				if (j+2 > argc)
				{
					printf("erro token 1");
					//printf("Entrei aqui no j+2 -t\n");
					lida_erro(erro_argumento_token_invalido, argc, argv);
				}
				//TODO posso dividir esse if para ter mais detalhes do meu erro;
				else if(veri_token(argv[j+1]))
				{
					printf("erro token 2");
					lida_erro(erro_argumento_token_invalido, argc, argv);
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
				cont1->cont_op++;
				pkt->numopts++;
				int cont_aux = cont1->cont_op - 1;
				if(cont1->cont_t == 0)
				{
					//add_token_null (&pkt);
				}
				//TODO posso especificar esse erro também
				if (j+3 > argc)
				{
					lida_erro(erro_argumento_option_invalido, argc, argv);
				}
				else if(veri_option (argv[j+1], argv[j+2]))
				{
					lida_erro(erro_argumento_option_invalido, argc, argv);
				}
				else
				{
					//add_option (&pkt, argv[j+1], argv[j+2]);

	#if DEBUG_ID_OP && DEBUG
					printf("1op)argv[j] = -op\n");
					printf("2op)argv[j] = %s, ", argv[j]);
					printf("3op)argv[j+1] = %s\n", argv[j+1]);
					printf("4op) j = %d\n", j);
	#endif			
					add_option(pkt, cont_aux, argv[j+1], argv[j+2], cont1->cont_op, &option_delta);
					//add_option(&pkt, argv[j+1], argv[j+2], cont_op, &option_delta);
					j+=3;
				}
			}
			else if(strcmp(argv[j], "-p") == 0)
			{
				cont1->cont_p++;
				//printf("cont_p++\n");
				//TODO posso especificar esse erro também
				if (j+2 > argc)
				{
					printf("Nao existe arg\n");
					lida_erro(erro_argumento_payload_invalido, argc, argv);
				}
				//TODO mesmo do TOKEN, dividir para especificar
				else if(cont1->cont_p > 1)
				{
					printf("Mais de um payload\n");
					lida_erro(erro_argumento_payload_invalido, argc, argv);
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
			else if(cont1->cont_p == 0 && cont1->cont_op == 0)
			{
				lida_erro (erro_comando, argc, argv);
			}
		}
		//monta_pkt ()
	}

	int main (int argc, char *argv[])
	{
		//Pacote

		//printf("1)OI");
		coap_packet_t pkt;
		coap_conts contadores;
		//memset(pkt, 0, sizeof(pkt));
		//Conexão
		struct sockaddr_in serv_addr;
		socklen_t addrlen = sizeof(serv_addr);
		uint8_t buffer[256];
		int buf_len = sizeof(buffer);
		memset(buffer, 0, buf_len);
		cria_pkt (&pkt, &contadores);

		int clienteSockfd = socket(AF_INET, SOCK_DGRAM, 0);
		if (clienteSockfd < 0)
		{
			printf("Erro Socket\n");
			exit(1);
		}
		 bzero ((char *) &serv_addr, addrlen);
		 serv_addr.sin_family = AF_INET;
		 serv_addr.sin_addr.s_addr = inet_addr ("127.0.0.1");
		 serv_addr.sin_port = htons(5683);
		 if(connect(clienteSockfd, (struct sockaddr *) &serv_addr, addrlen) < 0)
		 {
			 printf("Erro no socket\n");
			 exit(1);
		 }
		 //Fim de Conexão
		 //Argumentos
		 //Nº de Argumentos - ./cliente
		 int cont_arg = argc -1;
		 /*for (i=0; i<argc; i++)
		 {
		 	args[i] = argv[i];
		 }*/
		 //printf("1)OI");
		 for (cont_arg = 0; cont_arg <argc; cont_arg++)
		 {
		 	//printf("Entrando no for argumento\n");
	#if DEBUG
		 	//printf("Cont_arg = %d, argc = %d\n", cont_arg, argc);
		 	//printf("Arg = %s\n", argv[cont_arg]);
	#endif

		 }
		 identifica_arg (&pkt, argc, argv, &contadores);
		 printf("Imprimindo packet -> options\n\n");
		 printf_option(&pkt.opts[0]);
		 printf_option(&pkt.opts[1]);
		 printf_option(&pkt.opts[2]);
		 //strncpy(buffer,monta_pkt (&pkt), strlen(buffer));
		 monta_pkt(&pkt, buffer, &contadores);
		 printf("Entrando no ultimo printf\n");
		 printf_buffer (buffer);
		 /*printf("\nbuffer = 0x%02X\n", buffer[0]);
		 printf("\nbuffer = 0x%02X\n", buffer[1]);
		 printf("\nbuffer = 0x%02X\n", buffer[2]);
		 printf("\nbuffer = 0x%02X\n", buffer[3]);
		 printf("\nbuffer = 0x%02X\n", buffer[4]);
		 printf("\nbuffer = 0x%02X\n", buffer[5]);
		 printf("\nbuffer = 0x%02X\n", buffer[6]);
		 printf("\nbuffer = 0x%02X\n", buffer[7]);
		 printf("\nbuffer = 0x%02X\n", buffer[8]);
		 printf("\nbuffer = 0x%02X\n", buffer[9]);
		 printf("\nbuffer = 0x%02X\n", buffer[10]);
		 printf("\nbuffer = 0x%02X\n", buffer[11]);
		 printf("\nbuffer = 0x%02X\n", buffer[12]);
		 printf("\nbuffer = 0x%02X\n", buffer[13]);
		 printf("\nbuffer = 0x%02X\n", buffer[14]);
		 printf("\nbuffer = 0x%02X\n", buffer[15]);*/
		 return 0;

		 //Fim de argumentos
		 //Mensagem
		 /*while (1==1)
		 {
			 printf("Digite a mensagem:\n");
			 scanf("%s",buffer);
			 if (strcmp(buffer, "exit") == 0)
			 {
				 close(clienteSockfd);
				 exit(2);
			 }
			 //__fpurge(stdin);
			 printf("Buffer = %s\n", buffer);
			 write(clienteSockfd, buffer, addrlen);

		 }*/
		 //Fim de envio da mensagem
	}