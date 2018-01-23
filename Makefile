CFLAGS = -g
OPTFLAGS = -O3

all: timelife cache-ref cache-test

cache-ref: cache-test.o cache-ref.o
	$(CC) -g -o cache-ref cache-test.o cache-ref.o
cache-test: cache-test.o cache.o
cache-test.o: cache-test.c cache.h
cache.o: cache.c cache.h


timelife.o:	life.h timelife.c
	$(CC) -c $(CFLAGS) $(OPTFLAGS)  timelife.c
life.o:	life.h life.c
	$(CC) -c $(CFLAGS)  $(OPTFLAGS) life.c

timelife:	timelife.o life.o
	$(CC) -o timelife $(CFLAGS) timelife.o life.o

clean:
	rm -f cache-test cache-test.o cache.o cache-ref timelife timelife.o life.o

run:	timelife 
	time ./timelife
