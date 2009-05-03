/*
 *=============================================================================
 *  Parser to generate an airfoil profile from DATCOM input cards
 *
 *  Copyright (C) 2009  Ronald Jensen
 *  ron(at)jentronics.com
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


