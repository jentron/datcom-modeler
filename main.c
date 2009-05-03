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


	struct AIRFOIL wingfoil;
	struct AIRFOIL htailfoil;
 
    if (argc > 1) {
      ReadDatcom(argv[1], &ac);
      PrintAC(&ac);

/* Process wingfoil */
	if (ac.wingfoil.NPTS && ac.wingfoil.YUPPER && ac.wingfoil.YLOWER) 
		DatcomFoil(&ac.wingfoil, &wingfoil);
	else if(ac.wingfoil.NACA_DESCR)
	      NacaFoil(ac.wingfoil.NACA_DESCR, &wingfoil);
	else
	      NacaFoil("NACA-W-4-2414", &wingfoil);

/* Process htailfoil */
	if (ac.htailfoil.NPTS && ac.htailfoil.YUPPER && ac.htailfoil.YLOWER) 
		DatcomFoil(&ac.htailfoil, &htailfoil);
	else if(ac.htailfoil.NACA_DESCR)
	      NacaFoil(ac.htailfoil.NACA_DESCR, &htailfoil);
	else
	      NacaFoil("NACA-W-4-0014", &htailfoil);


      InitAC(stdout, 5);
      WriteWing(stdout, &ac.wing, &wingfoil, "Wing", ac.synths.XW, ac.synths.ZW);
      WriteWing(stdout, &ac.htail, &htailfoil, "H-Tail", ac.synths.XH, ac.synths.ZH);
      WriteBody(stdout, &ac.body, &ac.synths);
    }
    return(0);
}

