#include <stdio.h>
#include <math.h>

#include "modeler.h"

int tubesurface(FILE *ofp, int a, int b, int count, int type, int color);
int skinsurface(FILE *ofp, int a, int count, int type, int color, int reverse);


int InitAC(FILE *ofp, int kids)
{
	fprintf(ofp,"AC3Db\n");
	fprintf(ofp,"MATERIAL \"zinc chromate\" rgb 0.455 0.722 0.169  amb 0.455 0.722 0.169  emis 0 0 0  spec 0.227 0.161 0.161  shi 65  trans 0\n");
	fprintf(ofp,"MATERIAL \"white\" rgb 1 1 1  amb 1 1 1  emis 0 0 0  spec 0 0 0  shi 10  trans 0 \n");
	fprintf(ofp,"MATERIAL \"red\" rgb 1 0.5 0.5  amb 1 0.5 0.5  emis 0 0 0  spec 0 0 0  shi 10  trans 0 \n");
	fprintf(ofp,"OBJECT world\nkids %d\n", kids);

}
int WriteBody(FILE *ofp, struct BODY *body, struct SYNTHS *synths)
{
	int i, j;
	double theta=0, step=0.314159, ZR, ZC;

        fprintf(ofp,"OBJECT poly\nname \"Body\"\ncrease 45.0\nnumvert %d\n", body->NX * 20); // 
 
	for(i=0; i < body->NX; i++)
	{
		theta=0;
		for(j = 0; j < 20; j++)
		{
			ZR = (body->ZU[i]-body->ZL[i])/2;
			ZC = body->ZU[i]-ZR; 
			fprintf(ofp,"%f %f %f\n", body->X[i], 
				cos(theta)*ZR + ZC,
				sin(theta)*body->R[i]);
			theta+=step;
		}
	}
        fprintf(ofp,"numsurf %d\n", 40 * ( body->NX -1 ));
        for(i=0;i<body->NX;i++)
        {
                tubesurface(ofp, (i+1) * 20, (i) * 20 , 20, 0x30, 0);
        }
 }

int WriteWing(FILE *ofp, struct WGPLNF *wing, struct AIRFOIL *airfoil, struct SYNTHS *synths)
{
	int i, ribs, sections, current_rib, current_section;
	double chord[4],span[4], offset_x[4], offset_z[4];

	if(wing->SSPNOP > 0.0)
	{
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
		offset_x[0]=synths->XW+wing->CHSTAT*wing->CHRDR;
		offset_x[1]=offset_x[0]+tan(wing->SAVSI * 0.017453293)*span[1];
		offset_x[2]=offset_x[0]+tan(wing->SAVSI * 0.017453293)*span[2];
		offset_x[3]=offset_x[0]+tan(wing->SAVSI * 0.017453293)*span[2] + tan(wing->SAVSO * 0.017453293)*wing->SSPNOP;
// SSPNDD  * cos (dihedral) = SSPNOP ??
		offset_z[0]=synths->ZW;
		offset_z[1]=synths->ZW;
		offset_z[2]=synths->ZW + tan(wing->DHDADI * 0.017453293) * (wing->SSPNE-wing->SSPNOP);
		offset_z[3]=synths->ZW + tan(wing->DHDADI * 0.017453293) * (wing->SSPNE-wing->SSPNOP) 
                                       + tan(wing->DHDADO * 0.017453293) * (wing->SSPNOP) ;
	} else {
		ribs=3;
		sections=2;
		chord[0]=wing->CHRDR;
		chord[1]=wing->CHRDTP+(wing->CHRDR-wing->CHRDTP)*(wing->SSPNE/wing->SSPN);
		chord[2]=wing->CHRDTP;
		span[0]=0;
		span[1]=wing->SSPN-wing->SSPNE;
		span[2]=wing->SSPN;
		offset_x[0]=synths->XW;
		offset_x[1]=synths->XW+tan(wing->SAVSI * 0.017453293)*span[1];
		offset_x[2]=synths->XW+tan(wing->SAVSI * 0.017453293)*span[2];
		offset_z[0]=synths->ZW;
		offset_z[1]=synths->ZW;
		offset_z[2]=synths->ZW + tan(wing->DHDADI * 0.017453293) * (wing->SSPNE);
	}

	fprintf(ofp,"OBJECT poly\nname \"Left Wing\"\ncrease 45.0\nnumvert %d\n", airfoil->COUNT * ribs); // three or four based on type
	for(current_rib=0;current_rib<ribs;current_rib++)
	{
		for (i=0;i<airfoil->COUNT;i++)
		{
			fprintf(ofp,"%f %f %f\n", 
				offset_x[current_rib] + ((airfoil->DATAX[i] - wing->CHSTAT) * chord[current_rib]), 
				offset_z[current_rib] + (airfoil->DATAY[i] * chord[current_rib]),
				span[current_rib]);
		}
	}

	fprintf(ofp,"numsurf %d\n", airfoil->COUNT * 2 * sections + airfoil->COUNT - 2); 
	for(current_section=0;current_section<sections;current_section++)
	{
		tubesurface(ofp, current_section * airfoil->COUNT, (current_section + 1) * airfoil->COUNT , airfoil->COUNT, 0x30, current_section);
	}
	skinsurface(ofp, (sections)*airfoil->COUNT, airfoil->COUNT-1, 0x30, sections-1, 0);

	fprintf(ofp,"kids 0\n");

	fprintf(ofp,"OBJECT poly\nname \"Right Wing\"\ncrease 45.0\nnumvert %d\n", airfoil->COUNT * ribs); // three or four based on type
	for(current_rib=0;current_rib<ribs;current_rib++)
	{
		for (i=0;i<airfoil->COUNT;i++)
		{
			fprintf(ofp,"%f %f %f\n", 
				offset_x[current_rib] + ((airfoil->DATAX[i] - wing->CHSTAT) * chord[current_rib]), 
				offset_z[current_rib] + (airfoil->DATAY[i] * chord[current_rib]), 
				-span[current_rib]);
		}
	}


	fprintf(ofp,"numsurf %d\n", airfoil->COUNT * 2 * sections + airfoil->COUNT - 2); 
	for(current_section=0;current_section<sections;current_section++)
	{
		tubesurface(ofp, (current_section + 1) * airfoil->COUNT, (current_section ) * airfoil->COUNT , airfoil->COUNT, 0x30, current_section);
	}
	skinsurface(ofp, (sections)*airfoil->COUNT, airfoil->COUNT-1, 0x30, sections-1, 1);
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

int skinsurface(FILE *ofp, int a, int count, int type, int color, int reverse)
{
	int i;
	if(reverse){
		for(i=1;i<count;i++)
		{
			fprintf(ofp,"SURF 0x%02x\nmat %d\nrefs 3\n%d 0 0\n%d 0 0\n%d 0 0\n", type, color, a, a+i, a+i+1);
		}
	} else {
		for(i=1;i<count;i++)
		{
			fprintf(ofp,"SURF 0x%02x\nmat %d\nrefs 3\n%d 0 0\n%d 0 0\n%d 0 0\n", type, color, a, a+i+1, a+i);
		}
	}

}

