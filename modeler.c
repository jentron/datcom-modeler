#include <stdio.h>
#include "modeler.h"

int tubesurface(FILE *ofp, int a, int b, int count, int type, int color);


int InitAC(FILE *ofp, int kids)
{
	fprintf(ofp,"AC3Db\n");
	fprintf(ofp,"MATERIAL \"zinc chromate\" rgb 0.455 0.722 0.169  amb 0.455 0.722 0.169  emis 0 0 0  spec 0.227 0.161 0.161  shi 65  trans 0\n");
	fprintf(ofp,"MATERIAL \"white\" rgb 1 1 1  amb 1 1 1  emis 0 0 0  spec 0 0 0  shi 10  trans 0 \n");
	fprintf(ofp,"MATERIAL \"white\" rgb 1 1 1  amb 1 1 1  emis 0 0 0  spec 0 0 0  shi 10  trans 0 \n");
	fprintf(ofp,"OBJECT world\nkids %d\n", kids);

}

int WriteWing(FILE *ofp, struct WGPLNF *wing, struct AIRFOIL *airfoil, struct SYNTHS *synths)
{
	int i, ribs, sections, current_rib, current_section;
	double chord[4],span[4];

	switch (wing->TYPE)
	{
	case 1: /* Straight tapered planform */
		ribs=3;
		sections=2;
		chord[0]=wing->CHRDR;
		chord[1]=wing->CHRDTP+(wing->CHRDR-wing->CHRDTP)*(wing->SSPNE/wing->SSPN);
		chord[2]=wing->CHRDTP;
		span[0]=0;
		span[1]=wing->SSPN-wing->SSPNE;
		span[2]=wing->SSPN;
		break;
	case 2: /* Double delta planform (aspect ratio <= 3) */
	case 3: /* Cranked planform (aspect ratio > 3) */
		ribs=4;
		sections=3;
		chord[0]=wing->CHRDR;
		chord[1]=wing->CHRDBP+(wing->CHRDR-wing->CHRDBP)*((wing->SSPNE-wing->SSPNOP)/(wing->SSPN-wing->SSPNOP));
		chord[2]=wing->CHRDBP;
		chord[3]=wing->CHRDTP;
		span[0]=0;
		span[1]=wing->SSPN-wing->SSPNE;
		span[2]=wing->SSPN-wing->SSPNOP;
		span[3]=wing->SSPN;
		break;
	default:
		fprintf(stderr,"Unkown TYPE %d in WGPLNF\n", wing->TYPE);
		return(0);
	}

	fprintf(ofp,"OBJECT poly\nname \"Left Wing\"\ncrease 45.0\nnumvert %d\n", airfoil->COUNT * ribs); // three or four based on type
	for(current_rib=0;current_rib<ribs;current_rib++)
	{
		for (i=0;i<airfoil->COUNT;i++)
		{
			fprintf(ofp,"%f %f %f\n", 
				synths->XW + (airfoil->DATAX[i] * chord[current_rib]), 
				synths->ZW + (airfoil->DATAY[i] * chord[current_rib]),
				span[current_rib]);
		}
	}

	fprintf(ofp,"numsurf %d\n", airfoil->COUNT * 2 * sections); // will be 2 or 3 depending on type
	for(current_section=0;current_section<sections;current_section++)
	{
		tubesurface(ofp, current_section * airfoil->COUNT, (current_section + 1) * airfoil->COUNT , airfoil->COUNT, 0x30, current_section);
	}
	fprintf(ofp,"kids 0\n");

	fprintf(ofp,"OBJECT poly\nname \"Right Wing\"\ncrease 45.0\nnumvert %d\n", airfoil->COUNT * ribs); // three or four based on type
	for(current_rib=0;current_rib<ribs;current_rib++)
	{
		for (i=0;i<airfoil->COUNT;i++)
		{
			fprintf(ofp,"%f %f %f\n", 
				synths->XW + (airfoil->DATAX[i] * chord[current_rib]), 
				synths->ZW + (airfoil->DATAY[i] * chord[current_rib]), 
				-span[current_rib]);
		}
	}


	fprintf(ofp,"numsurf %d\n", airfoil->COUNT * 2 * sections); // will be 2 or 3 depending on type
	for(current_section=0;current_section<sections;current_section++)
	{
		tubesurface(ofp, (current_section + 1) * airfoil->COUNT, (current_section ) * airfoil->COUNT , airfoil->COUNT, 0x30, current_section);
	}
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

