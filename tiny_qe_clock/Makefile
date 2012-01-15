CC=/usr/bin/avr-gcc
CFLAGS=-g -Os -Wall -mcall-prologues -mmcu=attiny13a
OBJ2HEX=/usr/bin/avr-objcopy 
UISP=avrdude 
TARGET=tiny_qe_clock

program : $(TARGET).hex
	sudo $(UISP) -c usbtiny -p t13 -U flash:w:$(TARGET).hex -v

%.obj : %.o
	$(CC) $(CFLAGS) $< -o $@

%.hex : %.obj
	$(OBJ2HEX) -j .text -O ihex $< $@

clean :
	rm -f *.hex *.obj *.o
