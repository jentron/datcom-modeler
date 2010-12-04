/*
 *=============================================================================
 *  naca.c generate naca airfoil profiles
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


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "modeler.h"
#include "modeler_proto.h"

void naca4digit(double m, double p, double t, struct AIRFOIL *airfoil);

#ifdef STANDALONE
int verbose = 3;
int main(int argc, char *argv[])
{
	struct AIRFOIL airfoil;
	int i;

	if (argc !=2) return -1;
	NacaFoil(argv[1], &airfoil);

	for(i=0;i<airfoil.COUNT;i++)
	{
		fprintf(stdout, "%0.4f\t%0.4f\n", airfoil.DATAX[i], airfoil.DATAY[i]);
	}
}
#else
extern int verbose;
#endif

int NacaFoil(char *name, struct AIRFOIL *foil)
{
	double m, p, t;
	int i=16;
	char *foo;
	char bar[3];
	if(strncmp("NACA", name, 4) )
	{
		return(0); //not a naca description
	}
	m = i/1000;
	i -= m*1000;
	m *= 0.01;

	p = i/100;
	i -= p*100;
	p *= 0.1;

	t = (double)i * 0.01;

	switch (name[7])
	{
	 case '1':
if(verbose > 1 )fprintf(stderr,"%s CASE 1\n", name);
	 break;

	 case '4':
		foo = &name[9];
		i = atoi(foo);
if(verbose > 1 )fprintf(stderr,"%s CASE 4: %d\n", name, i);
		m = i/1000;
		i -= m*1000;
		m *= 0.01;

		p = i/100;
		i -= p*100;
		p *= 0.1;

		t = (double)i * 0.01;

	 break;

	 case '5':  /* Using Naca 4 series math */
		foo = &name[9];
		i = atoi(foo);
if(verbose > 1 )fprintf(stderr,"%s CASE 5: %d ", name, i);
		m = i/10000;
		i -= m*10000;
		m *= 0.01;

		p = i/100;
		i -= p*100;
		p *= 0.005;

		t = (double)i * 0.01;
if(verbose > 1 )fprintf(stderr," m = %0.2f, p = %0.2f, t = %0.2f\n", m, p, t);
	 break;

	 case '6':
if(verbose > 1 )fprintf(stderr,"%s CASE 6\n", name);
		bar[0]=name[13];
		bar[1]=name[14];
		bar[2]=0;
		t = atol(bar)/100.;
if(verbose > 1 )fprintf(stderr,"thickness = %s%% = %0.2f\n", bar, t);

	 break;

	 case 'S':
if(verbose > 1 )fprintf(stderr,"%s CASE S\n", name);
	 break;

	 default:
if(verbose > 0 )fprintf(stderr,"%s Unknown airfoil\n", name);
	}



	naca4digit(m, p, t, foil);
}

void naca4digit(double m, double p, double t, struct AIRFOIL *airfoil )
{
	int i, j;
	double yc, yt, x2, x3, x4, xroot, stations=10;
	double Xu, Yu, Xl, Yl,dx=0, dyc=0, oyc=0, theta;
	double x[]={0.0,0.0125,0.05,0.1,0.15,0.2,0.3,0.5,0.7,1.0};

airfoil->DATAX = malloc( 2 * stations * sizeof(double) );
airfoil->DATAY = malloc( 2 * stations * sizeof(double) );
airfoil->COUNT = 2 * stations;

	for(i=0, j=2*stations-1;i<stations;i++, j--){
		x2 = x[i] * x[i];
		x3 = x2 * x[i];
		x4 = x3 * x[i];
		xroot = sqrt(x[i]);
		if(x[i] < p){
			yc = (m / p * p) * (2*p*x[i] - x2);
		} else {
			yc = ( m /  (1-p)*(1-p) ) * ( (1 -2*p)+2*p*x[i]-x2);
		}
		yt = (t/0.2)*(0.2969*xroot - 0.1260 * x[i] - 0.3516*x2 + 0.2843*x3 - 0.1015*x4);
		airfoil->DATAX[i] = x[i];
		airfoil->DATAY[i] = yc + yt;
		airfoil->DATAX[j] = x[i];
		airfoil->DATAY[j] = yc - yt;

/*
theta = atan2(dyc,dx) ;
Xu = x[i] - yt * sin(theta);
Yu = yc + yt * cos (theta);
Xl = x[i] + yt * sin(theta);
Yl = yc - yt * cos(theta);

		fprintf(stdout, "%0.4f\t%0.4f\t%0.4f\t%0.4f\n", Xu, Yu, Xl, Yl);
		fprintf(stderr, "calculating %0.2f %0.2f %0.2f at %0.2f. Yc = %0.8f, Yt = %0.4f\n", m, p, t, x[i], yc, yt);
		if(i)
		{
			dx = x[i] - x[i-1];
			dyc = yc - oyc;
			oyc -yc;
		}
*/
	}

}

