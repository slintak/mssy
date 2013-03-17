MCU	= atmega328p
F_CPU	= 16000000
BOARD	= ARDUINO

CC = avr-gcc 
OBJCOPY = avr-objcopy
DFU_PRG = dfu-programmer
BAUDRATE = 57600
AVRDUDE = avrdude
PROGRAMMER = arduino
FORMAT	= ihex
TTY = python -m serial.tools.miniterm

ifeq ($(OS),Windows_NT)
	PORT = COM8
	RST = ./uno_rst.py $(PORT) $(BAUDRATE)
else
	PORT ?= /dev/tty.usbmodem*
	RST = stty -f $(PORT) hupcl
endif

FLAGS = -g -std=c99 -Werror --pedantic -Wextra -Wall -Os -mmcu=$(MCU) -DF_CPU=$(F_CPU) -I. -D$(BOARD)

OUT = main
OBJS = main.o uart.o

prog: $(OUT).hex
	$(RST)
	$(AVRDUDE) -p $(MCU) -P $(PORT) -c $(PROGRAMMER) -U flash:w:$<
	$(TTY) $(PORT) -b $(BAUDRATE) -e

$(OUT).lst: $(OUT).elf
	avr-objdump -h -S $< > $@
	less $(FILE).lst

$(OUT).hex: $(OUT).elf
	$(OBJCOPY) -j .text -j .data  -O $(FORMAT) $< $@

$(OUT).elf: $(OBJS)
	$(CC) $(FLAGS) -o $@ $+
	
%.o: %.c
	$(CC) $(FLAGS) -c -o $@ $<

lst: $(OUT).elf
	avr-objdump -h -S $< > $(OUT).lst
	less $(OUT).lst

clean:
	rm -f *.elf *.o *.hex
