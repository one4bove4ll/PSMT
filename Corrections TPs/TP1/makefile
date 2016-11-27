cc := gcc
cflags := -Wall -g -std=gnu99
executables := $(basename $(wildcard *.c))

all: $(executables)

%: %.c
	$(cc) $(cflags) $< -o $@

clean:
	-rm -f $(executables)
