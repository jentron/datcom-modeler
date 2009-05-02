
datcom-modeler: main.c modeler.h modeler.o
	$(CC) main.c modeler.o -o $@ -lm

modeler.o: modeler.h modeler.c
	$(CC) modeler.c -c -o $@ 


