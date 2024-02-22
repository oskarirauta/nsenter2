all: world
GCC?=gcc
CFLAGS?=-Wall

OBJS:= objs/main.o

ifeq ($(DEBUG), 1)
	CFLAGS += "-DDEBUG"
endif

world: nsenter2

$(shell mkdir -p objs)

objs/main.o: main.c
	$(GCC) $(CFLAGS) $(INCLUDES) -c -o $@ $<;

nsenter2: $(OBJS)
	$(GCC) $(CFLAGS) $(LDFLAGS) $^ -o $@;

.PHONY: clean
clean:
	@rm -rf objs nsenter2
