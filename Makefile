all:
	gcc -o batt batt.c `pkg-config --cflags --libs gtk+-2.0 gdk-pixbuf-2.0`
	./batt
