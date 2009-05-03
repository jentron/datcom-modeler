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
 
    if (argc > 1) {
      ReadDatcom(argv[1], &ac);
      PrintAC(&ac);
	if (ac.wingfoil.NPTS && ac.wingfoil.YUPPER && ac.wingfoil.YLOWER) 
		DatcomFoil(&ac.wingfoil, &wingfoil);
	else if(ac.wingfoil.NACA_DESCR)
	      NacaFoil(ac.wingfoil.NACA_DESCR, &wingfoil);
	else
	      NacaFoil("NACA-W-4-2414", &wingfoil);

      InitAC(stdout, 3);
      WriteWing(stdout, &ac.wing, &wingfoil, &ac.synths);
      WriteBody(stdout, &ac.body, &ac.synths);
    }
    return(0);
}

