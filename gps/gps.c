/*
 * gps.c
 *
 *  Created on: Feb 27, 2025
 *      Author: marcman
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "usart.h"

#define MAX_SIZE_SENTENCE 80
#define NMEA_HEADER_SIZE 5

struct GPGGA
{
    char time[10];
    char latitude[9];
    char north_south_indicator;
    char longitude[10];
    char east_west_indicator;
    char position_fix_indicator;
    char num_satellites_used;
};

struct NMEA_GPS
{
	struct GPGGA gpgga;
};

HAL_StatusTypeDef err = HAL_ERROR;

void parseGPGGA(uint8_t *, struct GPGGA *fields);
void print_fields(struct GPGGA *fields);
void seperate_sentences(uint8_t *, struct NMEA_GPS *fields);


uint16_t rx_bytes_num = 0;

void gps_routine()
{
	uint8_t gps_rx_data[200] = "";

	struct NMEA_GPS fields = {};
	fields.gpgga.num_satellites_used = '0';

	err = HAL_UARTEx_ReceiveToIdle(&huart1, gps_rx_data, sizeof(gps_rx_data), &rx_bytes_num, 1100);

	if(err == HAL_OK)
	{
	  printf("Received\r\n");

//	  parse(gps_rx_data, &fields);
	  seperate_sentences(gps_rx_data, &fields);
	  print_fields(&fields.gpgga);
	}
	else
	  printf("Did not receive\r\n");
}

void seperate_sentences(uint8_t *gps_rx_str, struct NMEA_GPS *fields)
{
    char *token = strtok((char *)gps_rx_str, "$");
    while (token != NULL)
    {
//    	printf("size of token : %u\r\n", strlen(token));
//    	printf("%s\r\n",token);
    	if(memcmp("GPGGA", token, NMEA_HEADER_SIZE) == 0)
    		parseGPGGA((uint8_t *)token+NMEA_HEADER_SIZE+1, &fields->gpgga);
    	token = strtok(NULL, "$");
    }

}
void parseGPGGA(uint8_t *gpgga_data_str, struct GPGGA *fields)
{
	uint32_t field_count = 0;
    // Returns first token
    char *token;

    // Keep printing tokens while one of the
    // delimiters present in str[].
    while( (token = strsep((char **)&gpgga_data_str, ",")) != NULL) {
        switch (field_count) {
			case 0:memcpy(fields->time, token, sizeof(fields->time)); break;
			case 1:memcpy(fields->latitude, token, sizeof(fields->latitude)); break;
			case 3:memcpy(fields->longitude, token, sizeof(fields->longitude)); break;
			case 6:memcpy(&fields->num_satellites_used, token, sizeof(fields->num_satellites_used)); break;

			default:
				break;
		}
        field_count++;

        token = strtok(NULL, ",");
    }
}

void print_fields(struct GPGGA *fields)
{
    printf("time : %.2sh %.2sm %.2ss\r\n", fields->time, fields->time+2, fields->time+4);
    printf("latitude : %s\r\n", fields->latitude);
    printf("longitude : %s\r\n", fields->longitude);
    printf("number of connected sat : %c\r\n", fields->num_satellites_used);
}
