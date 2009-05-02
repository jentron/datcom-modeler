#include <stdio.h>
#include <stdlib.h>
#include "modeler.h"

int main(int argc, char *argv[])
{
	double data[]={0.00000,  0.06559,  0.08280,  0.08817,  0.08817,  0.08065,  0.07097,  0.05699,  0.04194,  0.02366,
                       0.00000, -0.01075, -0.01935, -0.02796, -0.03548, -0.04194, -0.04946, -0.05161, -0.05161, -0.04516};
//0.0,0.075,0.08,0.07,0.06,0.0,-0.015,-0.025,-0.03,-0.03};
	struct WGPLNF *wing;
	struct AIRFOIL wingfoil;

	wing=malloc(sizeof(struct WGPLNF));
	wing->CHRDR = 1.0;
	wing->CHRDTP = 0.8;
	wing->SSPN = 10.0;
	wing->SSPNE = 8.0;
	wing->TYPE = 1.0;

	wingfoil.COUNT=20;
	wingfoil.DATA=data;


	InitAC(stdout, 2);
	WriteWing(stdout, wing, &wingfoil);

	if(wing) free(wing);
	return(0);
}

