
CFLAGS=-g -Wall -W

CHEZ_SCHEME_DIR=/usr/local/lib/csv9.5.8/ta6le

all: schemesh

clean:
	rm -f *~ *.o schemesh

hash_iterator.o: hash_iterator.c hash_iterator.h main.h
	$(CC) -c $< -o $@ $(CFLAGS) -I$(CHEZ_SCHEME_DIR)

main.o: main.c main.h hash_iterator.h posix.h
	$(CC) -c $< -o $@ $(CFLAGS) -I$(CHEZ_SCHEME_DIR) -DCHEZ_SCHEME_DIR="$(CHEZ_SCHEME_DIR)"

posix.o: posix.c posix.h main.h
	$(CC) -c $< -o $@ $(CFLAGS) -I$(CHEZ_SCHEME_DIR)


schemesh: hash_iterator.o posix.o main.o
	$(CC) $^ -o $@ -L$(CHEZ_SCHEME_DIR) -lkernel -lz -llz4 -lm -lncurses -luuid
