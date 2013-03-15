#include "hardware.h"

#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

/* UART baudrate */
//#define BAUDRATE	115200
#define BAUDRATE	57600
//#define BAUDRATE	38400
//#define BAUDRATE	19200
//#define BAUDRATE	9600

/* Compute baudrate from MCU frequency */
#define UBAUD(b)	(F_CPU/16/(b)-1)

/* Timer's OCR constant */
#define TIMER_FREQ	(1)	// 1Hz
//#define TIMER_FREQ	(2)	// 2Hz
//#define TIMER_FREQ	(10)	// 10Hz
#define TIMER_OCR(s, p)	((F_CPU / ((s)*(p))-1))

#define LED_ON(l)	(LEDPORT |= (1 << (l)))
#define LED_OFF(l)	(LEDPORT &= ~(1 << (l)))
#define LED_TOGGLE(l)	(LEDPORT ^= _BV((l)))

#define TRUE	(1)
#define	FALSE	(0)

static volatile uint16_t adcvalue;
static volatile uint8_t timer_seconds = 0;

/**
 * Timer1 (16bit) initialization.
 * Timer generates 'compare' interrupt.
 */
void timer_init(uint16_t f) {
	/* Set 1024 prescaller. */
	TCCR1B |= _BV(WGM12) | _BV(CS12) | _BV(CS10);
	/* Enable compare interrupt. */
	TIMSK1 |= _BV(OCIE1A);
	/* Compare constant. */
	OCR1A = TIMER_OCR(f, 1024);
	/* Clear timer register. */
	TCNT1 = 0;
}

/**
 * Compare interrupt for Timer1.
 */
ISR(TIMER1_COMPA_vect) {
	timer_seconds++;
	//LED_TOGGLE(LEDR);
}

/**
 * UART initialization to 'ubrr' baudrate.
 */
void uart_init(uint16_t ubrr) {
	/* Set baudrate. */
	UBRR = UBAUD(ubrr);
	/* Disable double speed. */
	UCSRA &= ~(_BV(U2X));
	/* Enable receiver and transmitter. */
	UCSRB = _BV(RXEN)|_BV(TXEN);
	/* Format: async, no parity, stop bit, 8 bits. */
	UCSRC = _BV(UCSZ01)|_BV(UCSZ00);
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
	return (UCSRA & _BV(RXC));
}

/**
 * Get one char from UART.
 */
char uart_getc(void) {
	/* Wait for data. */
	while(!(UCSRA & _BV(RXC)));
	return UDR;
}

#define UART_BUF_LEN	(20)
static char uart_rx_buf[UART_BUF_LEN];
static uint8_t uart_rx_count = 0;

/**
 * Receive string from UART.
 * String has to be ended by '\n'
 * and cannot be longer than
 * UART_BUF_LEN.
 */
char *uart_gets(uint8_t timeout) {
	uart_rx_count = 0;
	timer_seconds = 0;

	char ch;
	while(1) {
	  if(uart_available()) {
	    ch = uart_getc();
	    if(ch == '\n') break;
	    uart_rx_buf[uart_rx_count++] = ch;

	  }

	  if(timer_seconds >= timeout)
	  	return NULL;
	}

	uart_rx_buf[uart_rx_count] = '\0';
	return uart_rx_buf;
}

/**
 * Initialization of ADC.
 */
void adc_init(void) {
	#ifdef ZIGBIT
	  /* For Aref=2.56V */
	  ADMUX |= _BV(REFS1) | _BV(REFS0);
	#elif ARDUINO
	  /* For Aref=VCC */
	  ADMUX |= _BV(REFS0);
	#endif
	/* Prescaler div factor */
	#if F_CPU == 8000000UL
	  /* 64 -> 8MHZ/64 = 125KHz */
	  ADCSRA |= _BV(ADPS2)|_BV(ADPS1);
	#elif F_CPU == 16000000UL
	  /* 64 -> 16MHZ/128 = 250KHz */
	  ADCSRA |= _BV(ADPS2)|_BV(ADPS1)|_BV(ADPS0);
	#else
	  #error "ADC: Unsupported MCU frequency!"
	#endif
	/* ADC Enable. */
	ADCSRA |= _BV(ADEN);
}

uint16_t adc_read(uint8_t channel) {
	/* Select channel. */
	ADMUX |= channel & 0x07;
	/* Start conversion. */
	ADCSRA |= _BV(ADSC);

	/* Wait until conversion ended. */
	while(!(ADCSRA & _BV(ADIF)));
	return ADC;
}

int main(void) {
	/* Initialization of peripherals. */
	uart_init(BAUDRATE);
	adc_init();
	timer_init(TIMER_FREQ);
	sei(); /* Enable interrupts. */
	
	/* Initialization of LEDs. */
	LEDDDR |= LED_ON(LEDY) | LED_ON(LEDR) | LED_ON(LEDG);
	LEDPORT &= LED_OFF(LEDY) & LED_OFF(LEDR) & LED_OFF(LEDG);

	/* Print welcoming message. */
	uart_puts("Up and running...\n");

	/* Wait 10s for user input from UART. */
	/* User has to set threshold. */
	uart_puts("Set threshold, please [0-1023]: ");
	
	char *tmp = uart_gets(10);
	char str[20];
	if(tmp != NULL){
		uint16_t threshold = atoi(tmp);
		itoa(threshold, str, 10);
		uart_puts("Threshold was set to ");
		uart_puts(str);
		uart_puts(".\n");

		while(1) {
			uint16_t ad = adc_read(0);
			if(ad > threshold) {
				LED_ON(LEDR);

				itoa(adc_read(0), str, 10);
				uart_puts("ALARM: ");
				uart_puts(str);
				uart_putc('\n');

				_delay_ms(10);
				LED_OFF(LEDR);
				_delay_ms(100);
				timer_seconds = 0;
			}
		}

	} else {
		timer_seconds = 0;
		while(1) {
			if (timer_seconds >= 2) {
				LED_ON(LEDG);

				itoa(adc_read(0), str, 10);
				uart_puts("REPORT: ");
				uart_puts(str);
				uart_putc('\n');

				_delay_ms(10);
				LED_OFF(LEDG);
				timer_seconds = 0;
			} // if
		} // while
	} // if
} // main
