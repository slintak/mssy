#ifndef __UART_H__
#define __UART_H__

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#if defined(__AVR_ATmega1281__)
	/* ATMega1281 */
	#define UBRR	UBRR1
	#define UCSRA	UCSR1A
	#define UCSRB	UCSR1B
	#define UCSRC	UCSR1C
	#define RXEN	RXEN1
	#define TXEN	TXEN1
	#define RXC	RXC1
	#define TXC	TXC1
	#define UDRE	UDRE1
	#define UDR	UDR1
	#define U2X	U2X0
	#define RXCIE	RXCIE1
#elif defined(__AVR_ATmega328P__)
	/* ATMega328P */
	#define UBRR	UBRR0
	#define UCSRA	UCSR0A
	#define UCSRB	UCSR0B
	#define UCSRC	UCSR0C
	#define RXEN	RXEN0
	#define TXEN	TXEN0
	#define RXC	RXC0
	#define TXC	TXC0
	#define UDRE	UDRE0
	#define UDR	UDR0
	#define U2X	U2X0
	#define RXCIE	RXCIE0
	#define UART_RINT USART_RX_vect
#else
	#error "Unsupported MCU!"
#endif

#define UART_BUF_LEN	(16)

/* UART baudrate */
//#define BAUDRATE	115200
#define BAUDRATE	57600
//#define BAUDRATE	38400
//#define BAUDRATE	19200
//#define BAUDRATE	9600

/* Compute baudrate from MCU frequency */
#define UBAUD(b)	(F_CPU/16/(b)-1)

//static char uart_rx_buf[UART_BUF_LEN];
//static uint8_t uart_rx_count = 0;

/**
 * UART initialization to 'ubrr' baudrate.
 */
void uart_init(uint16_t ubrr);

/**
 * Send one char via UART.
 */
void uart_putc(char data);

/**
 * Send string via UART.
 */
void uart_puts(const char *str);

/**
 * Is there new character in UART buffer?
 */
char uart_available(void);

/**
 * Get one char from UART.
 */
char uart_getc(void);

/**
 * Receive string from UART.
 * String has to be ended by '\n'
 * and cannot be longer than
 * UART_BUF_LEN.
 *
 * If message is not received in
 * 'timeout' seconds, NULL is
 * returned.
 */
uint8_t uart_gets(char *str);

#endif
