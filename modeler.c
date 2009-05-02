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

int WriteWing(FILE *ofp, struct WGPLNF *wing, struct AIRFOIL *airfoil, struct SYNTHS *synths)
{
	int i, ribs, sections;
	double chord;

	switch (wing->TYPE)
	{
	case 1: /* Straight tapered planform */
		ribs=3;
		sections=2;
		break;
	case 2: /* Double delta planform (aspect ratio <= 3) */
		ribs=4;
		sections=3;
		break;
	case 3: /* Cranked planform (aspect ratio > 3) */
		ribs=4;
		sections=3;
		break;
	default:
		fprintf(stderr,"Unkown TYPE %d in WGPLNF\n", wing->TYPE);
		return(0);
	}

	fprintf(ofp,"OBJECT poly\nname \"Left Wing\"\ncrease 45.0\nnumvert %d\n", airfoil->COUNT * ribs); // three or four based on type
	chord=wing->CHRDR;
	for (i=0;i<airfoil->COUNT;i++)
	{
		fprintf(ofp,"%f %f %f\n", airfoil->DATAX[i] * chord, airfoil->DATAY[i] * chord, 0.0);
	}

	chord=wing->CHRDTP+(wing->CHRDR-wing->CHRDTP)*(wing->SSPNE/wing->SSPN);
	for (i=0;i<airfoil->COUNT;i++)
	{
		fprintf(ofp,"%f %f %f\n", airfoil->DATAX[i] * chord, airfoil->DATAY[i] * chord, (wing->SSPN - wing->SSPNE));
	}

	chord=wing->CHRDTP;
	for (i=0;i<airfoil->COUNT;i++)
	{
		fprintf(ofp,"%f %f %f\n", airfoil->DATAX[i] * chord, airfoil->DATAY[i] * chord, (wing->SSPNE));
	}

	fprintf(ofp,"numsurf %d\n", airfoil->COUNT * 2 * sections); // will be 2 or 3 depending on type
	tubesurface(ofp, 0, airfoil->COUNT, airfoil->COUNT, 0x30, 1);
	tubesurface(ofp, airfoil->COUNT, airfoil->COUNT*2, airfoil->COUNT, 0x30, 0);
	fprintf(ofp,"kids 0\n");

	fprintf(ofp,"OBJECT poly\nname \"Right Wing\"\ncrease 45.0\nnumvert %d\n", airfoil->COUNT * ribs); // three or four based on type
	chord=wing->CHRDR;
	for (i=0;i<airfoil->COUNT;i++)
	{
		fprintf(ofp,"%f %f %f\n", airfoil->DATAX[i] * chord, airfoil->DATAY[i] * chord, 0.0);
	}

	chord=wing->CHRDTP+(wing->CHRDR-wing->CHRDTP)*(wing->SSPNE/wing->SSPN);
	for (i=0;i<airfoil->COUNT;i++)
	{
		fprintf(ofp,"%f %f %f\n", airfoil->DATAX[i] * chord, airfoil->DATAY[i] * chord, -(wing->SSPN - wing->SSPNE));
	}

	chord=wing->CHRDTP;
	for (i=0;i<airfoil->COUNT;i++)
	{
		fprintf(ofp,"%f %f %f\n", airfoil->DATAX[i] * chord, airfoil->DATAY[i] * chord, -(wing->SSPN));
	}

	fprintf(ofp,"numsurf %d\n", airfoil->COUNT * 2 * sections); // will be 2 or 3 depending on type
	tubesurface(ofp, airfoil->COUNT, 0, airfoil->COUNT, 0x30, 1);
	tubesurface(ofp, airfoil->COUNT*2, airfoil->COUNT, airfoil->COUNT, 0x30, 0);
	fprintf(ofp,"kids 0\n");
	return(1);
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

