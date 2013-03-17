#include "uart.h"

static char uart_rx_buf[UART_BUF_LEN];
static volatile uint8_t uart_rx_read = 0;
static volatile uint8_t uart_rx_write = 0;
static volatile uint8_t uart_rx_ready = 0;

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
	  uart_rx_ready = 1;
	} else {
	  uart_rx_buf[uart_rx_write] = data;
	  uart_rx_ready = 0;
	}

	uart_rx_write = (uart_rx_write+1) & (UART_BUF_LEN-1);
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
	while(uart_rx_read == uart_rx_write);
	char tmp = uart_rx_buf[uart_rx_read];
	uart_rx_read = (uart_rx_read+1) & (UART_BUF_LEN-1);
	return tmp;
}

/**
 * Receive string from UART.
 * String has to be ended by '\n'
 * and cannot be longer than
 * UART_BUF_LEN.
 *
 * Returns number of read chars.
 */
uint8_t uart_gets(char *str) {
	if(uart_rx_ready == 0) {
		str[0] = '\0';
		return 0;
	}

	uint8_t i = 0, j = 0;
	for(i = uart_rx_read; i != uart_rx_write; i=(i+1)&(UART_BUF_LEN-1)) {
		str[j++] = uart_rx_buf[i];
	}

	uart_rx_read = uart_rx_write;//(uart_rx_read+j) & (UART_BUF_LEN-1);
	str[j] = '\0';
	return j;
}
