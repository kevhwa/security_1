CC = gcc
CFLGAS = -g -Wall - Werror

TARGETS = cstore
OBJS = cstore.o aes.o sha256.o

cstore: $(OBJS)

cstore.o: aes.h sha256.h

aes.o: aes.h

sha256: sha256.h

$(TARGETS): 
$(OBJS):

PHONY += clean
clean:
	rm -rf $(TARGETS) a.out *.o

.PHONY: $(PHONY)
