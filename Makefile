PROGNAME ?= mbot-backoff

DEVICE         ?= atmega328p
AVR_PORT       ?= /dev/ttyUSB4
AVR_PROGRAMMER ?= arduino
CLOCK          ?= 16000000

PORT = -P $(AVR_PORT) $(if $(AVR_SPEED), -b $(AVR_SPEED))

ifndef PID_P
PID_P = 1
endif

ifndef PID_I
PID_I=0
endif

ifndef PID_D
PID_D=0
endif

OBJDUMP    ?= avr-objdump
OBJCOPY    ?= avr-objcopy
DEBUGFLAGS ?= -Os
CC         = avr-gcc
PID_TERMS   = -DPID_P=$(PID_P) -DPID_I=$(PID_I) -DPID_D=$(PID_D)
CPPFLAGS   = $(PID_TERMS) -IFastPID/src -DF_CPU=$(CLOCK)
CFLAGS     = -std=c99 -mmcu=$(DEVICE) $(DEBUGFLAGS)

AVRDUDE = avrdude $(PORT) -p $(DEVICE) -c $(AVR_PROGRAMMER)

OBJS = \
	   main.o \
	   motor.o \
	   millis.o \
	   serial.o \
	   distance.o \
	   FastPID/src/FastPID.o

%.s: %.c
	$(CC) $(CFLAGS) -o $@ -S $<

%.pre: %.c
	$(CC) $(CFLAGS) -o $@ -E $<

all: $(PROGNAME).hex

$(PROGNAME).elf: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

$(PROGNAME).hex: $(PROGNAME).elf
	rm -f $(PROGNAME).hex
	$(OBJCOPY) -j .text -j .data -O ihex $(PROGNAME).elf $(PROGNAME).hex
	avr-size --format=avr --mcu=$(DEVICE) $(PROGNAME).elf

flash: $(PROGNAME).hex
	$(AVRDUDE) -U flash:w:$(PROGNAME).hex:i

check:
	$(AVRDUDE)

clean:
	rm -f $(OBJS) $(PROGNAME).hex $(PROGNAME).elf
