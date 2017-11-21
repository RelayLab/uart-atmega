SRC=$(wildcard *.c)	#имена всех файлов .с в папке
OBJ=$(SRC:.c=.o)	#заменить .с на .о
ELF=$(SRC:.c=.elf)
LST=$(SRC:.c=.lst)
MMCU=atmega328p
CFLAGS=-Os -mmcu=$(MMCU) -g
LDFLAGS=

all: $(OBJ) gdbinit $(ELF) $(LST)			#собрать все .о

#	avr-gcc -c $< -o $@ $(CFLAGS) $(LDFLAGS)
#	avr-gcc $(OBJ) -o out.exe $(CFLAGS) $(LDFLAGS)

#%.o : %.c 
gdbinit:
	@echo target remote localhost:1212>gdbinit
	@echo.
	@echo load>>gdbinit
	@echo.
	@echo b main>>gdbinit
	@echo.
	@echo c>>gdbinit

%.o: %.c
	avr-gcc -c $< -o $@ $(CFLAGS) $(LDFLAGS)

%.elf: %.o 
	avr-gcc $< -o $@ -mmcu=$(MMCU)

%.lst: %.elf
	avr-objdump $< -S > $@

debug: $(OBJ) gdbinit
	start "simulavr" cmd /c simulavr -g -d atmega48
	avr-gdb -x gdbinit main.elf
clean:
	rm /s *.exe *.o *.a *.elf *.lst *.hex
	rm gdbinit

rebuild:
	make clean
	make all

prog:main.hex
	#for dirty windowses, where COM number should be found with devmgmt.msc
	#avrdude -p atmega328p -b 57600 -P COM3 -c arduino  -e -U flash:w:main.hex
	#for gnu find /dev/serial/... can be different
	sudo avrdude -p atmega328p -b 57600 -P /dev/serial/by-id/usb-1a86_USB2.0-Serial-if00-port0 -c arduino  -e -U flash:w:main.hex
main.hex:main.elf
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex
