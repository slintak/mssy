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

	// Default frequency is 16MHz
	#ifndef F_CPU
	#define F_CPU 16000000UL
	#endif
#else
	#error "Fatal: Unknown hardware!"
#endif

#endif
