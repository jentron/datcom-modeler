#include <stdlib.h>
#include "datcom-parser.h"


int DatcomFoil(DATCOM_AIRFOIL *dfoil, struct AIRFOIL *foil)
{
        int i,j;
	size_t npts = 2 * dfoil->NPTS;
	foil->DATAX = malloc(npts*sizeof(double));
	foil->DATAY = malloc(npts*sizeof(double));
        foil->COUNT = npts;
        for(i=0, j=foil->COUNT-1;i < dfoil->NPTS;i++, j--)
        {
                foil->DATAX[i] = dfoil->XCORD[i];
                foil->DATAY[i] = dfoil->YUPPER[i];
                foil->DATAX[j] = dfoil->XCORD[i];
                foil->DATAY[j] = dfoil->YLOWER[i];
        }
}


