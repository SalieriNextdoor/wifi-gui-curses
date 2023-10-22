CC = gcc
CFLAGS = -g -Wall -Werror -O2
LDFLAGS = -lncurses

objects = commands.o mainmenu.o paswin.o

wifi-gui: $(objects) wifi.o tests
	./tests
	rm -f tests tests.o
	$(CC) $(LDFLAGS) $(objects) wifi.o -o wifi-gui

tests: $(objects) tests.o

$(objects) wifi.o tests.o: %.o: %.c

.PHONY: clean
clean:
	rm -f wifi-gui tests *.o
