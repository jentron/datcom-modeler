#include <stdio.h>
#include "modeler.h"

int tubesurface(FILE *ofp, int a, int b, int count, int type, int color);


int InitAC(FILE *ofp, int kids)
{
	fprintf(ofp,"AC3Db\n");
	fprintf(ofp,"MATERIAL \"white\" rgb 1 1 1  amb 1 1 1  emis 0 0 0  spec 0 0 0  shi 10  trans 0 \n");
	fprintf(ofp,"MATERIAL \"zinc chromate\" rgb 0.455 0.722 0.169  amb 0.455 0.722 0.169  emis 0 0 0  spec 0.227 0.161 0.161  shi 65  trans 0\n");
	fprintf(ofp,"OBJECT world\nkids %d\n", kids);

}

int WriteWing(FILE *ofp, struct WGPLNF *wing, struct AIRFOIL *airfoil)
{
	int i;
	double chord,x,y;

	fprintf(ofp,"OBJECT poly\nname \"Left Wing\"\ncrease 45.0\nnumvert %d\n", airfoil->COUNT * 3); // three or four based on type
	x=0.0;
	y=1.0/(double)(airfoil->COUNT/2.0);
	chord=wing->CHRDR;
	for (i=0;i<airfoil->COUNT;i++)
	{
		fprintf(ofp,"%f %f %f\n", x * chord, airfoil->DATA[i] * chord, 0.0);
		x += y;
		if(x >= 1.0) y *= -1;
	}

	x=0.0;
	y*=-1;
	chord=wing->CHRDTP+(wing->CHRDR-wing->CHRDTP)*(wing->SSPNE/wing->SSPN);
	for (i=0;i<airfoil->COUNT;i++)
	{
		fprintf(ofp,"%f %f %f\n", x * chord, airfoil->DATA[i] * chord, (wing->SSPN-wing->SSPNE));
		x += y;
		if(x >= 1.0) y *= -1;
	}

	x=0.0;
	y*=-1;
	chord=wing->CHRDTP;
	for (i=0;i<airfoil->COUNT;i++)
	{
		fprintf(ofp,"%f %f %f\n", x * chord, airfoil->DATA[i] * chord, (wing->SSPNE));
		x += y;
		if(x >= 1.0) y *= -1;
	}

	fprintf(ofp,"numsurf %d\n", airfoil->COUNT * 2 * 2); // will be 2 or 3 depending on type
	tubesurface(ofp, 0, airfoil->COUNT, airfoil->COUNT, 0x30, 1);
	tubesurface(ofp, airfoil->COUNT, airfoil->COUNT*2, airfoil->COUNT, 0x30, 0);
	fprintf(ofp,"kids 0\n");

	fprintf(ofp,"OBJECT poly\nname \"Right Wing\"\ncrease 45.0\nnumvert %d\n", airfoil->COUNT * 3); // three or four based on type
	x=0.0;
	y=1.0/(double)(airfoil->COUNT/2.0);
	chord=wing->CHRDR;
	for (i=0;i<airfoil->COUNT;i++)
	{
		fprintf(ofp,"%f %f %f\n", x * chord, airfoil->DATA[i] * chord, 0.0);
		x += y;
		if(x >= 1.0) y *= -1;
	}

	x=0.0;
	y*=-1;
	chord=wing->CHRDTP+(wing->CHRDR-wing->CHRDTP)*(wing->SSPNE/wing->SSPN);
	for (i=0;i<airfoil->COUNT;i++)
	{
		fprintf(ofp,"%f %f %f\n", x * chord, airfoil->DATA[i] * chord, -(wing->SSPN-wing->SSPNE));
		x += y;
		if(x >= 1.0) y *= -1;
	}

	x=0.0;
	y*=-1;
	chord=wing->CHRDTP;
	for (i=0;i<airfoil->COUNT;i++)
	{
		fprintf(ofp,"%f %f %f\n", x * chord, airfoil->DATA[i] * chord, -(wing->SSPNE));
		x += y;
		if(x >= 1.0) y *= -1;
	}

	fprintf(ofp,"numsurf %d\n", airfoil->COUNT * 2 * 2); // will be 2 or 3 depending on type
	tubesurface(ofp, airfoil->COUNT, 0, airfoil->COUNT, 0x30, 1);
	tubesurface(ofp, airfoil->COUNT*2, airfoil->COUNT, airfoil->COUNT, 0x30, 0);
	fprintf(ofp,"kids 0\n");

}


int tubesurface(FILE *ofp, int a, int b, int count, int type, int color)
{
	int i;
	for(i=0;i<count;i++)
	{
		fprintf(ofp, "SURF 0x%02x\nmat %d\nrefs 3\n%d 0 0\n%d 0 0\n%d 0 0\n", type, color, a+i, b+i, a+(i+1)%count); 
		fprintf(ofp, "SURF 0x%02x\nmat %d\nrefs 3\n%d 0 0\n%d 0 0\n%d 0 0\n", type, color, b+i, b+(i+1)%count, a+(i+1)%count); 
	}
}

