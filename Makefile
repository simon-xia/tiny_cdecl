FLAGS = -Wall -O2
OBJS = tiny_cdecl.o example.o
PRAM = test_tiny_cdecl
CC = cc

main: $(OBJS)
	$(CC) $(OBJS) -o $(PRAM) $(FLAGS)

tiny_cdecl.o:tiny_cdecl.c
	$(CC) -c tiny_cdecl.c $(FLAGS)

example.o:example.c
	$(CC) -c example.c $(FLAGS)

clean:
	rm *.o
