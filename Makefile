main.out: main.o ai.o estadoTablero.o grafic.o events.o popups.o
	gcc -g -Wall -o main.out main.o ai.o estadoTablero.o grafic.o events.o popups.o `pkg-config --cflags --libs gtk+-2.0`

main.o: main.c
	gcc -g -Wall -c main.c `pkg-config --cflags --libs gtk+-2.0`

ai.o: ai.c
	gcc -g -Wall -c ai.c `pkg-config --cflags --libs gtk+-2.0`

estadoTablero.o: estadoTablero.c
	gcc -g -Wall -c estadoTablero.c `pkg-config --cflags --libs gtk+-2.0`

grafic.o: grafic.c
	gcc -g -Wall -c grafic.c `pkg-config --cflags --libs gtk+-2.0`

events.o: events.c
	gcc -g -Wall -c events.c `pkg-config --cflags --libs gtk+-2.0`

popups.o: popups.c
	gcc -g -Wall -c popups.c `pkg-config --cflags --libs gtk+-2.0`

clean:
	rm *.o
	rm *.out