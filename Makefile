all:
	gcc -o bbat bbat.c `pkg-config --cflags --libs gtk+-2.0 gdk-pixbuf-2.0`
	./bbat
