#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "coap.h"

#include <stdlib.h>

#define DEBUG_END 1
#define TEMP_1_CHAR 0
#define TEMP_STRING 1
#define DEBUG_STRUCT_TIME 1

static char light = '0';
static char temperature[4] = "0";
static char time_freq_string[4] = "15";
var_cli variable = {0};

const uint16_t rsplen = 1500;
static char rsp[1500] = "";
void build_rsp(void);

#ifdef ARDUINO
#include "Arduino.h"
static int led = 6;
void endpoint_setup(void)
{                
    pinMode(led, OUTPUT);     
    build_rsp();
}
#else
#include <stdio.h>
void endpoint_setup(void)
{
    build_rsp();
    variable = create_var_time ();
}
#endif


/* Variável de tempo (usar no main-posix)*/
void set_var_time (short int tempo)
{
	variable.tempo = tempo;
}
short int get_var_time ()
{
	return variable.tempo;
}

var_cli create_var_time ()
{
	var_cli variable;
	variable.tempo = 5; /*Segundos, para testes*/
	return variable;
}

static const coap_endpoint_path_t path_well_known_core = {2, {".well-known", "core"}};
static int handle_get_well_known_core(coap_rw_buffer_t *scratch, const coap_packet_t *inpkt, coap_packet_t *outpkt, uint8_t id_hi, uint8_t id_lo)
{
    return coap_make_response(scratch, outpkt, (const uint8_t *)rsp, strlen(rsp), id_hi, id_lo, &inpkt->tok, COAP_RSPCODE_CONTENT, COAP_CONTENTTYPE_APPLICATION_LINKFORMAT);
}

static const coap_endpoint_path_t path_light = {1, {"light"}};
static int handle_get_light(coap_rw_buffer_t *scratch, const coap_packet_t *inpkt, coap_packet_t *outpkt, uint8_t id_hi, uint8_t id_lo)
{
    return coap_make_response(scratch, outpkt, (const uint8_t *)&light, 1, id_hi, id_lo, &inpkt->tok, COAP_RSPCODE_CONTENT, COAP_CONTENTTYPE_TEXT_PLAIN);
}

static int handle_post_light(coap_rw_buffer_t *scratch, const coap_packet_t *inpkt, coap_packet_t *outpkt, uint8_t id_hi, uint8_t id_lo)
{
    return coap_make_response(scratch, outpkt, (const uint8_t *)&light, 1, id_hi, id_lo, &inpkt->tok, COAP_RSPCODE_CONTENT, COAP_CONTENTTYPE_TEXT_PLAIN);
}

/* Receber parâmetros, inicialmente serão:
Tempo = 15m -> Para testes -> 2s
*/

static const coap_endpoint_path_t path_time_freq = {2, {"var", "time"}};
static int handle_get_time_freq(coap_rw_buffer_t *scratch, const coap_packet_t *inpkt, coap_packet_t *outpkt, uint8_t id_hi, uint8_t id_lo)
{
#if DEBUG_END
	printf("\nWell get time new method\n");
	
		printf("Visualizando time_string");
		printf("\nTime = %s\n", time_freq_string);
#endif
		return coap_make_response(scratch, outpkt, (const uint8_t *)&(time_freq_string), strlen(time_freq_string), id_hi, id_lo, &inpkt->tok, COAP_RSPCODE_CONTENT, COAP_CONTENTTYPE_TEXT_PLAIN);
	
}

static int handle_post_time_freq(coap_rw_buffer_t *scratch, const coap_packet_t *inpkt, coap_packet_t *outpkt, uint8_t id_hi, uint8_t id_lo)
{
	char *end;
#if DEBUG_END
	printf("\nWell post temperature new method?\n");
#endif
	if (inpkt -> payload.len == 0)
	{
		return coap_make_response(scratch, outpkt, NULL, 0, id_hi, id_lo, &inpkt->tok, COAP_RSPCODE_BAD_REQUEST, COAP_CONTENTTYPE_TEXT_PLAIN);
	}
	else
	{
		strncpy (time_freq_string, "00000", strlen(time_freq_string));
		strncpy (time_freq_string, (char *)inpkt->payload.p, strlen(time_freq_string));
#if DEBUG_END
		printf(" payload.len = %d\n", inpkt -> payload.len);
		strncpy (time_freq_string, "00000", strlen(time_freq_string));
		strncpy (time_freq_string, (char *)inpkt->payload.p, strlen(time_freq_string));
		printf("Time_fre_string = %s", time_freq_string);
#endif
#if DEBUG_END && DEBUG_STRUCT_TIME
		printf("1)Time_freq = %d\n", get_var_time());
#endif		
		set_var_time(strtol ((char *)inpkt->payload.p, &end, 0));
#if DEBUG_END && DEBUG_STRUCT_TIME
		printf("2)Time_freq = %d\n", get_var_time());
#endif
	}
    return coap_make_response(scratch, outpkt, (const uint8_t *)&(time_freq_string), inpkt->payload.len, id_hi, id_lo, &inpkt->tok, COAP_RSPCODE_CHANGED, COAP_CONTENTTYPE_TEXT_PLAIN);
}


static const coap_endpoint_path_t path_temperature = {2, {"var", "temperature"}};
static int handle_get_temperature(coap_rw_buffer_t *scratch, const coap_packet_t *inpkt, coap_packet_t *outpkt, uint8_t id_hi, uint8_t id_lo)
{
#if DEBUG_END
	printf("\nWell get temperature new method?\n");
	
		printf("Visualizando temperature");
		printf("\nTemperature = %s\nLight = %c\n", temperature, light);
#endif
		return coap_make_response(scratch, outpkt, (const uint8_t *)&(temperature), strlen(temperature), id_hi, id_lo, &inpkt->tok, COAP_RSPCODE_CONTENT, COAP_CONTENTTYPE_TEXT_PLAIN);
	
}

static int handle_post_temperature(coap_rw_buffer_t *scratch, const coap_packet_t *inpkt, coap_packet_t *outpkt, uint8_t id_hi, uint8_t id_lo)
{
#if DEBUG_END
	printf("\nWell post temperature new method?\n");
#endif
	if (inpkt -> payload.len == 0)
	{
		return coap_make_response(scratch, outpkt, NULL, 0, id_hi, id_lo, &inpkt->tok, COAP_RSPCODE_BAD_REQUEST, COAP_CONTENTTYPE_TEXT_PLAIN);
	}
	else
	{
#if DEBUG_END
		/*necessário temperature para devolver para pkt*/
		printf(" payload.len = %d\n", inpkt -> payload.len);
		printf("Temperature = %s\n", temperature);
		strncpy (temperature, "00000", strlen(temperature));
		printf("1)Temperature = %s\n lenght = %d\n", temperature, strlen(temperature));
		strncpy (temperature, (char *)inpkt -> payload.p, inpkt -> payload.len);
		printf("2)Temperature = %s\n lenght = %d\n", temperature, strlen(temperature));
#endif
		FILE * pFile;
		time_t t = time(NULL);
		struct tm tm = *localtime(&t);
		char date[10];
		snprintf(date,10, "%d",tm.tm_mday);
		pFile = fopen (date,"a+");
#if TEMP_1_CHAR
		if (inpkt -> payload.len > 5 || inpkt -> payload.len < 1)
			return coap_make_response(scratch, outpkt, NULL, 0, id_hi, id_lo, &inpkt->tok, COAP_RSPCODE_BAD_REQUEST, COAP_CONTENTTYPE_TEXT_PLAIN);
		else if (pFile!=NULL)
		{
			fprintf (pFile, "Temperature = ");
			if (inpkt -> payload.len < 2)
			{
				fprintf (pFile, "%c", inpkt -> payload.p [0]);
			}
			else if (inpkt -> payload.len  < 3)
			{
				fprintf (pFile, "%c", inpkt -> payload.p [0]);
				fprintf (pFile, "%c", inpkt -> payload.p [1]);
			}
			else if (inpkt -> payload.len  < 4)
			{
				fprintf (pFile, "%c", inpkt -> payload.p [0]);
				fprintf (pFile, "%c", inpkt -> payload.p [1]);
				fprintf (pFile, "%c", inpkt -> payload.p [2]);
			}
			else if (inpkt -> payload.len  < 5)
			{
				fprintf (pFile, "%c", inpkt -> payload.p [0]);
				fprintf (pFile, "%c", inpkt -> payload.p [1]);
				fprintf (pFile, "%c", inpkt -> payload.p [2]);
				fprintf (pFile, "%c", inpkt -> payload.p [3]);
			}
				
			fprintf (pFile, " C  \t|   Log - Data: %d / %d Hora: %d :%d : %d\n", tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
			fclose (pFile);
		}
#endif
#if TEMP_STRING
		if(pFile!=NULL)
		{
#if DEBUG_END
			printf("payload = %s\n", inpkt->payload.p);
#endif
			fprintf(pFile, "\n%s", (char *)inpkt->payload.p);
			fclose(pFile);
		}
#endif
		else
		{
			return coap_make_response(scratch, outpkt, NULL, 0, id_hi, id_lo, &inpkt->tok, COAP_RSPCODE_BAD_REQUEST, COAP_CONTENTTYPE_TEXT_PLAIN);
		}
	}
    return coap_make_response(scratch, outpkt, (const uint8_t *)&temperature, inpkt->payload.len, id_hi, id_lo, &inpkt->tok, COAP_RSPCODE_CHANGED, COAP_CONTENTTYPE_TEXT_PLAIN);
}


static int handle_put_light(coap_rw_buffer_t *scratch, const coap_packet_t *inpkt, coap_packet_t *outpkt, uint8_t id_hi, uint8_t id_lo)
{
#if DEBUG_END	
	printf("\nWell put light\n");
#endif
    if (inpkt->payload.len == 0)
        return coap_make_response(scratch, outpkt, NULL, 0, id_hi, id_lo, &inpkt->tok, COAP_RSPCODE_BAD_REQUEST, COAP_CONTENTTYPE_TEXT_PLAIN);
    if (inpkt->payload.p[0] == '1')
    {
        light = '1';
#ifdef ARDUINO
        digitalWrite(led, HIGH);
#else
        printf("ON\n");
#endif
        return coap_make_response(scratch, outpkt, (const uint8_t *)&light, 1, id_hi, id_lo, &inpkt->tok, COAP_RSPCODE_CHANGED, COAP_CONTENTTYPE_TEXT_PLAIN);
    }
    else
    {
        light = '0';
#ifdef ARDUINO
        digitalWrite(led, LOW);
#else
        printf("OFF\n");
#endif
        return coap_make_response(scratch, outpkt, (const uint8_t *)&light, 1, id_hi, id_lo, &inpkt->tok, COAP_RSPCODE_CHANGED, COAP_CONTENTTYPE_TEXT_PLAIN);
    }
}

const coap_endpoint_t endpoints[] =
{
    {COAP_METHOD_GET, handle_get_well_known_core, &path_well_known_core, "ct=40"},
    {COAP_METHOD_GET, handle_get_light, &path_light, "ct=0"},
    {COAP_METHOD_GET, handle_get_temperature, &path_temperature, "ct=6"},    
    {COAP_METHOD_GET, handle_get_time_freq, &path_time_freq, "ct=7"},
    {COAP_METHOD_PUT, handle_put_light, &path_light, NULL},
    {COAP_METHOD_POST, handle_post_temperature, &path_temperature, "ct=8"},
    {COAP_METHOD_POST, handle_post_light, &path_light, "ct=4"}, 
    {COAP_METHOD_POST, handle_post_time_freq, &path_time_freq, "ct=9"},
    {(coap_method_t)0, NULL, NULL, NULL}
};

void build_rsp(void)
{
    uint16_t len = rsplen;
    const coap_endpoint_t *ep = endpoints;
    int i;

    len--; /* Null-terminated string*/

    while(NULL != ep->handler)
    {
        if (NULL == ep->core_attr) {
            ep++;
            continue;
        }

        if (0 < strlen(rsp)) {
            strncat(rsp, ",", len);
            len--;
        }

        strncat(rsp, "<", len);
        len--;

        for (i = 0; i < ep->path->count; i++) {
            strncat(rsp, "/", len);
            len--;

            strncat(rsp, ep->path->elems[i], len);
            len -= strlen(ep->path->elems[i]);
        }

        strncat(rsp, ">;", len);
        len -= 2;

        strncat(rsp, ep->core_attr, len);
        len -= strlen(ep->core_attr);

        ep++;
    }
}


