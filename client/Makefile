SOURCES:=$(wildcard *.c)
OBJS:=$(patsubst %.c,%.o,$(SOURCES))
ELF:=client
CC:=gcc
CFLAGS:=  -I../include
$(ELF):$(OBJS)
	gcc $^ -o $@ -lcrypt

clean:
	rm -rf $(OBJS) $(ELF)
