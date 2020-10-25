CC = gcc
CFLAGS = -g -Wall -Werror
LDFLAGS = -g -lpthread

#TARGETS = mail-in
#OBJS = mail-in.o

#$(TARGETS):
#$(OBJS):

objects = mail-in mail-out
all: $(objects)

$(objects): %: %.c
	$(CC) $(CFLAGS) -o $@ $<

PHONY += clean
clean:
	rm -rf $(objects) a.out *.o

.PHONY: $(PHONY)
