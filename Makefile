CC=arm-linux-gnueabihf-gcc
CCARGS=--static -lm -lwiringPi -lpthread -std=c99 -D_GNU_SOURCE=1


SOURCES=$(wildcard src/*.c)
OUTPUT=out.elf

STRIPARGS=-Os -ffunction-sections -Wl,--gc-sections -fno-asynchronous-unwind-tables -Wl,--strip-all

all: compile

compile:
	$(CC) $(SOURCES) $(CCARGS) -o $(OUTPUT)

compile-stripped:
	$(CC) $(SOURCES) $(CCARGS) $(STRIPARGS) -o $(OUTPUT)

prog: compile
	scp $(OUTPUT) root@192.168.1.12:/root/

clean:
	-@rm $(OUTPUT)
