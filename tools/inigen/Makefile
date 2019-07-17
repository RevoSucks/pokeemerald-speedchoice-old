CC := gcc
CFLAGS := -O2 -Wno-unused-result -iquote ../../include
SRCS := inigen.c elf.c util.c
HEADERS := \
	global.h \
	elf.h \
	util.h \
	../../include/constants/global.h \
	../../include/constants/moves.h \
	../../include/constants/items.h \
	../../include/constants/species.h
LDFLAGS := -lcapstone

.PHONY: all clean

all: inigen
	@:

inigen: $(SRCS) $(HEADERS)
	$(CC) $(CFLAGS) $(SRCS) -o $@ $(LDFLAGS)

clean:
	$(RM) inigen inigen.exe
