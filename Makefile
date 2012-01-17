CC=gcc
FLAGS=`mysql_config --cflags --libs` -lpthread -Wall
EXECUTABLE=mysql_async
OBJECTS=mysql_async.o

all: $(EXECUTABLE)

$(EXECUTABLE): mysql_async.c
	$(CC) mysql_async.c -o $@ $(FLAGS)

clean:
	rm -rf $(EXECUTABLE)
	rm -rf $(OBJECTS)
