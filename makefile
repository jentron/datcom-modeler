CFLAGS = -g
OBJECTS = modeler.o datcom-parser.o

datcom-modeler: main.c $(OBJECTS)
	$(CC) $(CFLAGS) main.c $(OBJECTS) -o $@ -lm

modeler.o: modeler.h modeler.c
	$(CC) $(CFLAGS) modeler.c -c -o $@ 

naca: naca.c
	$(CC) naca.c -o $@ -lm
  
datcom-parser.o: datcom-parser.h modeler.h datcom-parser.c
	$(CC) $(CFLAGS) datcom-parser.c -c -o $@ 
 
datcom-parser.c: datcom.flex
	flex -o $@ datcom.flex
