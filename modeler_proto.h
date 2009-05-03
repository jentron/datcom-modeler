
#ifndef MODELER_PROTO_H

/* Funtions in modeler.c */
int InitAC(FILE *ofp, int kids);
int WriteBody(FILE *ofp, struct BODY *body, struct SYNTHS *synths);
int WriteWing(FILE *ofp, struct WGPLNF *wing, struct AIRFOIL *airfoil, char *name, double X, double Z);

/* Functions in naca.c */
int NacaFoil(char *name, struct AIRFOIL *foil);

#define MODELER_PROTO_H
#endif

