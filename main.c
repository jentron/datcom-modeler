#include <stdio.h>
#include <stdlib.h>
#include "modeler.h"

int main(int argc, char *argv[])
{
	double datay[]={0.00000,  0.06559,  0.08280,  0.08817,  0.08817,  0.08065,  0.07097,  0.05699,  0.04194,  0.02366,
                       0.00000, -0.01075, -0.01935, -0.02796, -0.03548, -0.04194, -0.04946, -0.05161, -0.05161, -0.04516};
	double datax[]={0.0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0,0.9,0.8,0.7,0.6,0.5,0.4,0.3,0.2,0.1,0};
	struct BODY body;
	struct SYNTHS synths;
	struct WGPLNF wing;
	struct AIRFOIL wingfoil;

	synths.XCG=0.68;
	synths.ZCG=0.16;
	synths.XW=-0.76;
	synths.ZW=-0.075;
	synths.ALIW=0.0;
	synths.XH=3.256;
	synths.ZH=0.60;
	synths.ALIH=-2.0;
	synths.XV=3.73;
	synths.ZV=0.618;
	synths.XVF=3.73;
	synths.ZVF=0.618;
	synths.SCALE=1.0;
	synths.VERTUP=1;


	wing.CHRDR = 2.0;
	wing.CHRDBP = 1.5;
	wing.CHRDTP = 0.8;
	wing.SSPN = 10.0;
	wing.SSPNOP = 5.0;
	wing.SSPNE = 8.0;
	wing.SAVSI = 10;
	wing.SAVSO = 0.;
	wing.CHSTAT = 1;
	wing.TYPE = 2.0;

	wingfoil.COUNT=20;
	wingfoil.DATAY=datay;
	wingfoil.DATAX=datax;


	InitAC(stdout, 2);
	WriteWing(stdout, &wing, &wingfoil, &synths);

	return(0);
}

