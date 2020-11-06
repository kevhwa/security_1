CC = gcc
CFLAGS = -g -Wall 
LDFLAGS = -g 

objects = mail-in mail-out
all: $(objects)

$(objects): %: %.c
	$(CC) $(CFLAGS) -o $@ $<

PHONY += clean
clean:
	rm -rf $(objects) a.out *.o

install: mail-in mail-out
	cp mail-in mail-out $(DEST)

.PHONY: $(PHONY)
