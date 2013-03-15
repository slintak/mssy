#ifndef __HARDWARE_H__
#define __HARDWARE_H__

/* Hardware definitions */
#ifdef ZIGBIT
	/* ZIGBIT with ATMega1281, 8MHz */

	// LEDs
	#define LEDPORT (PORTG)
	#define LEDDDR	(DDRG)
	#define LEDG 	(2)
	#define LEDY	(1)
	#define LEDR	(0)

	// UART
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

	// Default frequency is 8Mhz
	#ifndef F_CPU
	#define F_CPU 8000000UL
	#endif
#elif ARDUINO
	/* Arduino with ATMega328P, 16MHz */

	// LEDs
	#define LEDPORT (PORTB)
	#define LEDDDR	(DDRB)
	#define LEDG 	(3)
	#define LEDY	(4)
	#define LEDR	(5)

	// UART
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

	// Default frequency is 16MHz
	#ifndef F_CPU
	#define F_CPU 16000000UL
	#endif
#else
	#error "Fatal: Unknown hardware!"
#endif

#endif
