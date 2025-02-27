/*
 * uart.c
 *
 *  Created on: Apr 4, 2024
 *      Author: marcr
 */
#include "usart.h"
#include <stdio.h>
#include <errno.h>
#include <sys/unistd.h>

// _write syscall implementation for printf
int _write(int file, char *ptr, int len)
{
  // if file is not stdout or stderr
  if ((file != STDOUT_FILENO ) && (file != STDERR_FILENO))
  {
      errno = EBADF; // bad file num
      return -1;
  }
  HAL_StatusTypeDef status = HAL_UART_Transmit(&huart2, (uint8_t *)ptr, len, HAL_MAX_DELAY);

  return (status == HAL_OK ? len : 0); 	// return number of char written aka len if successfull or 0 if not
}
