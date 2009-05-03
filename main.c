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
fprintf(stderr, "NACA_DESCR = %s\n", ac.wingfoil.NACA_DESCR);
      NacaFoil(ac.wingfoil.NACA_DESCR, &wingfoil);

      InitAC(stdout, 2);
      WriteWing(stdout, &ac.wing, &wingfoil, &ac.synths);
    }
    return(0);
}

