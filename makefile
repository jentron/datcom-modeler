CFLAGS = -g
OBJECTS = modeler.o datcom-parser.o datcomfoil.o naca.o body.o

datcom-modeler: main.c $(OBJECTS)
	$(CC) $(CFLAGS) main.c $(OBJECTS) -o $@ -lm

modeler.o: modeler.h modeler.c
	$(CC) $(CFLAGS) modeler.c -c -o $@ 

naca.o: naca.c modeler.h modeler_proto.h
	$(CC) $(CFLAGS) naca.c -c -o $@ 

body.o: body.c modeler.h modeler_proto.h
	$(CC) $(CFLAGS) body.c -c -o $@ 

datcom-parser.o: datcom-parser.h modeler.h datcom-parser.c
	$(CC) $(CFLAGS) datcom-parser.c -c -o $@ 
 
datcomfoil.o: datcom-parser.h modeler.h datcomfoil.c
	$(CC) $(CFLAGS) datcomfoil.c -c -o $@

datcom-parser.c: datcom.flex
	flex -o $@ datcom.flex

#below are stand-alone test programs.

naca: naca.c
	$(CC) $(CFLAGS) naca.c -DSTANDALONE -o $@ -lm

body: body.c
	$(CC) $(CFLAGS) body.c -DSTANDALONE -o $@ -lm
 
