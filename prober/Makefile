CC = gcc
CFLAGS = -Wall -g -I/opt/homebrew/opt/libpq/include
LIBS = -lcurl -L/opt/homebrew/opt/libpq/lib -lpq -lpthread

probe: prober.c
	$(CC) $(CFLAGS) prober.c -o probe $(LIBS)

db_test: db_test.c
	$(CC) $(CFLAGS) db_test.c -o db_test $(LIBS)

clean: 
	rm -f probe *.o
	rm -f db_test *.o
