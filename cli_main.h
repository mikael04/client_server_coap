#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <netinet/in.h> //struct sockaddr_in
#include <stdbool.h>
#include <stddef.h>
#include "coap.h"

void printf_header (coap_header_t *hdr);
void printf_token (coap_buffer_t *token);
void printf_payload (coap_buffer_t *payload);
void printf_option (coap_option_t *opt);
void printf_buffer(uint8_t *buffer);
void printf_buffer_str(char buf[][512], int num_linhas);
void lida_erro_send_msg (short int erro);
void lida_erro_add (short int erro);
void lida_erro_monta (short int erro);
void lida_erro_id(short int erro, short int argc, char **argv);
void tempo_agora (char *tempo);
void get_time (struct timespec *time_now);
float calc_time_sub (struct timespec *start, struct timespec *stop);
void buffer_msg (char *buf_out, char *buf_out_p, short int *cont_msg, short int *pos, char buf_str[][512]);
int lida_msg_recebida (char *buf_in, char buf_str[][512], short int *cont_msg, short int *pos, struct timespec *time_post, struct timespec *time_start, FILE *pFile);
float calc_time_sub (struct timespec *start, struct timespec *stop);
void n_sleep (struct timespec *sleep_time);
short int corrige_len (uint8_t *buffer, short int size);
void monta_pkt (coap_packet_t *pkt, uint8_t *buf);
void monta_header_token (coap_packet_t *pkt, uint8_t *token);
void cria_pkt (coap_packet_t *pkt, uint8_t *token);
void add_payload (coap_packet_t *pkt, char *payload);
void add_option (coap_packet_t *pkt, short int cont_aux, short int *buf_aux_opt_n, char *op_conteudo, short int numopt, short int *option_running_delta);
void add_token_hdr_tkl (coap_packet_t *pkt, char *token);
short int veri_token (char *argv1);
short int veri_option (char *argv1, char *argv2);
short int veri_payload (char *argv1);
short int find_minus_plus (char *argv);
void identifica_arg (coap_packet_t *pkt, int argc, char **argv, char *buf_aux_opt_c, short int *buf_aux_opt_n);
void separa_string (char **string_sep, char *buf, short int n_str, short int len);
short int  conta_espc (char *buf);
int main_cli ();
