/*
 *=============================================================================
 *  Main program for DATCOM-MODELER
 *
 *  Copyright (C) 2009  Anders Gidenstam (anders(at)gidenstam.org)
 *  Copyright (C) 2009  Ronald Jensen    (ron(at)jentronics.com)
 *  http://www.gidenstam.org 
 *  http://www.jentronics.com
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *=============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "datcom-parser.h"

int dofoils(DATCOM_AIRFOIL *datcomfoil, struct AIRFOIL *foil, char * defaultfoil);


void PrintAC(struct AIRCRAFT *ac)
{
	return;
}

int main(int argc, char *argv[])
{
	AIRCRAFT ac;

/* eventually we should read these on the command line */
	int verbose=0;
	int wing=1;
	int htail=1;
	int vtail=1;
	int vfin=1;
	int body=1;


/* internal variables */
	int objects = 0;
	struct AIRFOIL wingfoil;
	struct AIRFOIL htailfoil;
	struct AIRFOIL vtailfoil;
	struct AIRFOIL vfinfoil;
 
    if (argc > 1) {
      ReadDatcom(argv[1], &ac);
      PrintAC(&ac);

/* Process wingfoil */
	if (wing) wing = ac.wing.CHRDR > 0. ? 1 : 0; // only render if root chord exists
	if (wing) wing = dofoils(&ac.wingfoil, &wingfoil, "NACA-W-4-2414");
	if (wing) objects += 2;
 		
/* Process htailfoil */
	if (htail) htail = ac.htail.CHRDR > 0. ? 1 : 0; // only render if root chord exists
	if (htail) htail =  dofoils(&ac.htailfoil, &htailfoil, "NACA-H-4-0014");
	if (htail) objects += 2;
 	
/* Process vtailfoil */
	if (vtail) vtail = ac.vtail.CHRDR > 0. ? 1 : 0; // only render if root chord exists
       	if (vtail) vtail =  dofoils(&ac.vtailfoil, &vtailfoil, "NACA-V-4-0012");
	if (vtail) objects += 1;

//* Process vfinfoil */
	if (vfin) vfin = ac.vfin.CHRDR > 0. ? 1 : 0; // only render if root chord exists
       	if (vfin) vfin =  dofoils(&ac.vfinfoil, &vfinfoil, "NACA-F-4-0012");
	if (vfin) objects += 1;

/* Process Body */
	if (body) body=ac.body.NX > 0. ? 1 : 0;
	if (body) objects +=1;

      InitAC(stdout, objects );
      if (wing) WriteWing(stdout, &ac.wing, &wingfoil, "Wing", ac.synths.XW, ac.synths.ZW);
      if (htail) WriteWing(stdout, &ac.htail, &htailfoil, "H-Tail", ac.synths.XH, ac.synths.ZH);
      if (vtail) WriteFin(stdout, &ac.vtail, &vtailfoil, "V-Tail", ac.synths.XV, ac.synths.ZV);
      if (vfin) WriteFin(stdout, &ac.vfin, &vfinfoil, "V-Fin", ac.synths.XVF, ac.synths.ZVF);
      if (body) WriteBody(stdout, &ac.body, &ac.synths);
    } else {
      fprintf(stderr, "Usage: %s datcom-file\n", argv[0]);
    }
    return(0);
}

int dofoils(DATCOM_AIRFOIL *datcomfoil, struct AIRFOIL *foil, char * defaultfoil)
{
        if (datcomfoil->NPTS && datcomfoil->YUPPER && datcomfoil->YLOWER)
                DatcomFoil(datcomfoil, foil);
        else if(datcomfoil->NACA_DESCR)
              NacaFoil(datcomfoil->NACA_DESCR, foil);
	else if(defaultfoil) 
		NacaFoil(defaultfoil, foil);
        else return(0); 


	return(1);
}
