CFLAGS = -g -Wall
OBJECTS = color.o modeler.o datcom-parser.o datcomfoil.o naca.o body.o

datcom-modeler: main.c $(OBJECTS) datcom-parser.h  matrix.h  modeler.h  modeler_proto.h
	$(CC) $(CFLAGS) main.c $(OBJECTS) -o $@ -lm

modeler.o: modeler.h modeler.c modeler_proto.h
	$(CC) $(CFLAGS) modeler.c -c -o $@ 

naca.o: naca.c modeler.h modeler_proto.h
	$(CC) $(CFLAGS) naca.c -c -o $@ 

body.o: body.c modeler.h modeler_proto.h
	$(CC) $(CFLAGS) body.c -c -o $@ 

color.o: color.c modeler.h modeler_proto.h
	$(CC) $(CFLAGS) color.c -c -o $@ 

datcom-parser.o: datcom-parser.h modeler.h datcom-parser.c
	$(CC) $(CFLAGS) datcom-parser.c -c -o $@ 
 
datcomfoil.o: datcom-parser.h modeler.h datcomfoil.c
	$(CC) $(CFLAGS) datcomfoil.c -c -o $@


matrix.o: matrix.c matrix.h
	$(CC) $(CFLAGS) matrix.c -c -o $@ 

datcom-parser.c: datcom.flex
	flex -o $@ datcom.flex

#below are stand-alone test programs.

naca: naca.c 
	$(CC) $(CFLAGS) naca.c -DSTANDALONE -o $@ -lm

color: color.c 
	$(CC) $(CFLAGS) color.c -DSTANDALONE -o $@ -lm

body: body.c
	$(CC) $(CFLAGS) body.c -DSTANDALONE -o $@ -lm
 
matrixtest: matrix.o matrixtest.c
	$(CC) $(CFLAGS) matrixtest.c matrix.o -DSTANDALONE -o $@ -lm

triangle: matrix.o triangle.c
	$(CC) $(CFLAGS) triangle.c matrix.o -DSTANDALONE -o $@ -lm
