CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99 -pedantic
TARGET  = banker
OBJS    = main.o banker.o entrada.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

main.o: main.c banker.h entrada.h
	$(CC) $(CFLAGS) -c main.c

banker.o: banker.c banker.h
	$(CC) $(CFLAGS) -c banker.c

entrada.o: entrada.c entrada.h banker.h
	$(CC) $(CFLAGS) -c entrada.c

log:
	script -a evidencias.log -c "date; whoami; pwd; bash"

clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all clean log