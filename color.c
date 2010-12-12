/*
 *=============================================================================
 *  color.c - routines to generate colors for AC3D file output
 *
 *  Copyright (C) 2010  Ronald Jensen
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

#include <stdio.h>
#include <math.h>

#include "modeler.h"
#include "modeler_proto.h"

extern int verbose;

#ifdef STANDALONE
int main(int argc, char *argv[])
{
	int i;
	double r=1.0, g=0.5, b=.25, sr, sg, sb;

	for(i=0;i<145;i+=16)
	{
		GetSpecular(r,g,b,i,&sr,&sg,&sb);
	        fprintf(stdout,"rgb %0.3f %0.3f %0.3f spec %0.3f %0.3f %0.3f shi %d\n", r, g, b, sr, sg, sb, i);
	}
}


#endif /* STANDALONE */


int GetShine(double roughness) 
{
/*      
Aerodynamically Smooth  0
Polished Metal  2.00E-005
Polished Wood   8.00E-005
Natural Sheet Metal     1.60E-004
Smooth Matte Paint      2.50E-004
Standard Camo Paint     4.00E-004
Cheap Camo Paint        1.20E-003
Galvanized Metal        6.00E-003
Cast Iron       1.00E-002
*/ 
/* 0 is dull, 128 is shiny */
 
/* I take the nth root of the rougness value times a factor and subtract that from 128. */
#define COLOR_ROOT          0.17    // Root and factor chosen to give 128 at 0 roughness
#define COLOR_FACTOR      280.0     // 65 at default, and 0 at max roughness
#define DEFAULT_ROUGHNESS   0.00016 //inches, from DATCOM manual

        double t;
        if(roughness < 0.) {
		if (verbose > 1) fprintf(stderr,"color: Default Roughness\n");
		roughness = DEFAULT_ROUGHNESS;
	}
        if(roughness > 1.00e-2) roughness = 1.00e-2;
        t = pow(roughness, COLOR_ROOT);
        t *= COLOR_FACTOR; // 128/(Cast Iron)^0.2 
        t = 128.-t;
        return((int)t);
}       
 
#define HARD 80 
int GetSpecular(double ired, double igrn, double iblu, int shiny, double *ored, double *ogrn, double *oblu)
{       
        double factor;
 
        if (shiny > 128) shiny = 128;
 
/*        if (shiny <= HARD)
        {
                factor=(double)shiny/HARD;
                *ored = ired*factor;
                *ogrn = igrn*factor;
                *oblu = iblu*factor;
        } else {
                factor=(double)(128-shiny)/HARD;
                *ored = ired*factor - factor + 1;
                *ogrn = igrn*factor - factor + 1;;
                *oblu = iblu*factor - factor + 1;;
        }
*/

	factor = (double)shiny/128.;
	*ored = factor;
	*ogrn = factor;
	*oblu = factor;
}   

