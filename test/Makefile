DEVICE         ?= atmega328p
CLOCK          ?= 16000000

OBJDUMP    ?= avr-objdump
OBJCOPY    ?= avr-objcopy
CC         = avr-gcc
CPPFLAGS   = -I.. -DF_CPU=$(CLOCK)
CFLAGS     = -std=c99 -mmcu=$(DEVICE) -Og -g

VPATH	   = ..

OBJS = serial.o

ALL_TESTS = $(wildcard test_*.c)

all: test

test: $(ALL_TESTS:.c=.ok)
	echo "All tests passed."

%.elf: %.o $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

test_motor.elf: motor.o
test_millis.elf: millis.o
test_distance.elf: distance.o millis.o

%.ok: %.elf
	echo "$@"
	python3 run_test.py $<
	touch $@

clean:
	rm -f $(ALL_TESTS:.c=.o) $(ALL_TESTS:.c=.elf) $(ALL_TESTS:.c=.ok)

ifndef VERBOSE
.SILENT:
endif
