#ifndef THREED_H
#define THREED_H

#include "matrix.h"

typedef struct surf3 {
	int type; // surface type;
	int mat;
	int refs;
	int point[3];
	double u[3];
	double v[3];
} Surface3;

typedef struct obj3d {
	void *child; //Points to first child, null for no children
	void *next;  //Points to next sibling object.  null for last entry
	int type; // 0=world, 1=group, 2=poly
	int kids;
	char *name;
	char *texture;
	char *data;
	int  datac;
	int texrep_u; //docs say double but practice disagrees.
	int texrep_v;
	double crease;
	Matrix3 *rot;
	Vector3 loc;
	char *url;
	int numverts;
	int allocverts;
	Vector3 *vert;
	int numsurf;
	int allocsurf;
	Surface3 *surf;
} Object3D;



//Prototypes
Object3D *CreateObject(unsigned int numvert, unsigned int numsurf, char *name, char *texture, int type);
void DestroyObject(Object3D *obj);
int  WriteObject(Object3D *obj, FILE *fp);

#endif //not defined THREED_H
