#include "uart.h"

static char uart_rx_buf[UART_BUF_LEN];
static volatile uint8_t uart_rx_read = 0;
static volatile uint8_t uart_rx_write = 0;

/* UART baudrate */
//#define BAUDRATE	115200
#define BAUDRATE	57600
//#define BAUDRATE	38400
//#define BAUDRATE	19200
//#define BAUDRATE	9600

/* Compute baudrate from MCU frequency */
#define UBAUD(b)	(F_CPU/16/(b)-1)

/**
 * UART initialization to 'ubrr' baudrate.
 */
void uart_init(uint16_t ubrr) {
	/* Set baudrate. */
	UBRR = UBAUD(ubrr);
	/* Disable double speed. */
	UCSRA &= ~(_BV(U2X));
	/* Enable receiver and transmitter. */
	UCSRB = _BV(RXCIE) | _BV(RXEN)|_BV(TXEN);
	/* Format: async, no parity, stop bit, 8 bits. */
	UCSRC = _BV(UCSZ01)|_BV(UCSZ00);
}

ISR(UART_RINT) {
	uint8_t data = UDR;
	if(data == '\n') {
	  uart_rx_buf[uart_rx_write] = '\0';
	} else {
	  uart_rx_buf[uart_rx_write] = data;
	}

	uart_rx_write = uart_rx_write++ & (UART_BUF_LEN-1);
}

/**
 * Send one char via UART.
 */
void uart_putc(char data) {
	/* Wait until buffer is empty. */
	while(!( UCSRA & _BV(UDRE)));
	/* Send data to buffer. */
	UDR = data;
}

/**
 * Send string via UART.
 */
void uart_puts(const char *str) {
	uint8_t i = 0;
	while(str[i] != '\0')
		uart_putc(str[i++]);
}

/**
 * Is there new character in UART buffer?
 */
char uart_available(void) {
	return uart_rx_write>=uart_rx_read;
}

/**
 * Get one char from UART.
 */
char uart_getc(void) {
	while(uart_rx_read >= uart_rx_write);
	char tmp = uart_rx_buf[uart_rx_read];
	uart_rx_read = uart_rx_read++ & (UART_BUF_LEN-1);
	return tmp;
}

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
uint8_t uart_gets(char *str) {
	uint8_t i = 0, j = 0;
	for(i = uart_rx_read; i == uart_rx_write; (i++)&(UART_BUF_LEM-1)) {
		str[j++] = uart_rx_buf[i];
	}

	uart_rx_read = (uart_rx_read+i) & (UART_BUF_LEN-1);
	return j;
}
