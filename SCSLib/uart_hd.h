/*
 * uart_hd.h
 *
 *  Created on: Mar 23, 2023
 *      Author: v
 */

#ifndef SCSLIB_UART_HD_H_
#define SCSLIB_UART_HD_H_

#include "stm32f1xx.h"

extern void USART_HD_PostInit(USART_TypeDef* USARTx);
extern void USART_HD_IRQHandler();

extern void USART_HD_BufferFlush(void);
extern int16_t USART_HD_BufferRead(void);
extern void USART_HD_Send(uint8_t *buf , uint8_t len);

#endif /* SCSLIB_UART_HD_H_ */
