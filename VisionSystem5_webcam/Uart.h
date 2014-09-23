#ifndef __UART_H
#define __UART_H

unsigned char rx_buffer[256];

void UART_Init();
void UART_TX_Char(char a);
void UART_TX();
int UART_RX();
void UART_Close();
#endif