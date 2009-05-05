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

void PrintAC(struct AIRCRAFT *ac)
{
	return;
}

int main(int argc, char *argv[])
{
	AIRCRAFT ac;
	int verbose;

	int wing=1;
	int htail=1;
	int vtail=1;
	int vfin=1;
	int body=1;

	int objects = 6;

	struct AIRFOIL wingfoil;
	struct AIRFOIL htailfoil;
	struct AIRFOIL vtailfoil;
 
    if (argc > 1) {
      ReadDatcom(argv[1], &ac);
      PrintAC(&ac);

/* Process wingfoil */
	if (ac.wingfoil.NPTS && ac.wingfoil.YUPPER && ac.wingfoil.YLOWER) 
		DatcomFoil(&ac.wingfoil, &wingfoil);
	else if(ac.wingfoil.NACA_DESCR)
	      NacaFoil(ac.wingfoil.NACA_DESCR, &wingfoil);
	else {
		wing=0;
		objects -= 2;
        }		
/* Process htailfoil */
	if (ac.htailfoil.NPTS && ac.htailfoil.YUPPER && ac.htailfoil.YLOWER) 
		DatcomFoil(&ac.htailfoil, &htailfoil);
	else if(ac.htailfoil.NACA_DESCR)
	      NacaFoil(ac.htailfoil.NACA_DESCR, &htailfoil);
	else {
              htail=0;
              objects -= 2;
        }
/* Process vtailfoil */
	if (ac.vtailfoil.NPTS && ac.vtailfoil.YUPPER && ac.vtailfoil.YLOWER) 
		DatcomFoil(&ac.vtailfoil, &vtailfoil);
	else if(ac.vtailfoil.NACA_DESCR)
	      NacaFoil(ac.vtailfoil.NACA_DESCR, &vtailfoil);
	else {
              vtail=0;
              objects -= 1;
        }

      InitAC(stdout, objects );
      if (wing) WriteWing(stdout, &ac.wing, &wingfoil, "Wing", ac.synths.XW, ac.synths.ZW);
      if (htail) WriteWing(stdout, &ac.htail, &htailfoil, "H-Tail", ac.synths.XH, ac.synths.ZH);
      if (vtail) WriteFin(stdout, &ac.vtail, &vtailfoil, "V-Tail", ac.synths.XV, ac.synths.ZV);
      if (body) WriteBody(stdout, &ac.body, &ac.synths);
    } else {
      fprintf(stderr, "Usage: %s datcom-file\n", argv[0]);
    }
    return(0);
}

