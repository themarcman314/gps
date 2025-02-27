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

uint8_t gps_rx_data[200] = "";
HAL_StatusTypeDef err = HAL_ERROR;

struct GPGGA fields;
void parse(uint8_t *str, struct GPGGA *fields);
void print_fields(struct GPGGA *fields);


uint16_t rx_bytes_num = 0;
uint8_t buf[30];

void gps_routine()
{
	err = HAL_UARTEx_ReceiveToIdle(&huart1, gps_rx_data, sizeof(gps_rx_data), &rx_bytes_num, 1100);

	if(err == HAL_OK)
	{
	  printf("Received\r\n");

	  // Parse data
	  memcpy(buf, gps_rx_data, 30);
	  parse(gps_rx_data, &fields);
	}
	else
	  printf("Did not receive\r\n");
}
void parse(uint8_t *str, struct GPGGA *fields)
{
	uint32_t field_count = 0;
    // Returns first token
    char* token = strtok(str, ",");

    // Keep printing tokens while one of the
    // delimiters present in str[].
    while (token != NULL) {
        printf(" %s\r\n", token);
        switch (field_count) {
			case 1:strncpy(fields->time, token, sizeof(fields->time)); break;
			case 2:strncpy(fields->latitude, token, sizeof(fields->latitude)); break;
			case 7:strncpy(&fields->num_satellites_used, token, sizeof(fields->num_satellites_used)); break;

			default: printf("Normal\r\n");
				break;
		}
        field_count++;

        token = strtok(NULL, ",");
    }
}

void print_fields(struct GPGGA *fields)
{
    printf("time : %s\r\n", fields->time);
    printf("latitude : %s\r\n", fields->latitude);
    printf("num sat : %c\r\n", fields->num_satellites_used);
}
