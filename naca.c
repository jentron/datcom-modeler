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

void naca4digit(double m, double p, double t, struct AIRFOIL *airfoil, int stations);
double *TaperSeq(int s);


#ifdef STANDALONE
#include <unistd.h>

void Usage(char *name)
{
	fprintf(stderr, "Usage: %s [-a|r] [-n (number of points)] [-v] [-q] [-o file] NACA-X-Y-ZZZZ\n", name);
	fprintf(stderr, "\ta|r output in ac3d or raw format\n\tv verbose\n\tquiet\n\tn number of points in foil (at least 5)\n");
	fprintf(stderr, "\to filename to use instead of stdout\n");
	fprintf(stderr, "NACA name is in datcom format:\n The four letters NACA, a hyphen, any character, a hyphen, ");
	fprintf(stderr, "foil type (1, 4, 5, 6, S), a hypen, the foil number.\n");
}

#define OUT_RAW  0
#define OUT_AC3D 1
//#define OUT_

int verbose = 3;
int main(int argc, char *argv[])
{
	struct AIRFOIL airfoil;
	int i, count;
	int stations=20;
	int opt;
	int output_type=OUT_RAW;
	FILE *ofp=stdout;

	while ((opt = getopt(argc, argv, "ar:n:o:qv")) != -1)
	{
		switch (opt)
		{
			case 'a':
				output_type=OUT_AC3D;
				break;
			case 'r':
				output_type=OUT_RAW;
				break;

			case 'n':
				stations=atoi(optarg);
				if(stations<5)
				{
					Usage(argv[0]);
					exit(EXIT_FAILURE);
				}
				break;

			case 'o':
				fprintf(stderr, "Creating file: %s\n",  optarg);
				if( ( ofp = fopen(optarg, "w")) == NULL)
				{
					fprintf(stderr,"Unable to open %s for writing\n", optarg);
					exit(EXIT_FAILURE);
				}
				break;
			case 'v':
				verbose++;
				break;
			case 'q':
				verbose = 0;
				break;
			default: /* '?' */
				Usage(argv[0]);
				exit(EXIT_FAILURE);
		}
	}

//	fprintf(stderr, "verbose=%d; optind=%d\n", verbose, optind);

	if (optind >= argc) {
		Usage(argv[0]);
		exit(EXIT_FAILURE);
	}
	NacaFoil(argv[optind], &airfoil, stations);
if(output_type==OUT_AC3D) 
{		count=airfoil.COUNT-1;
	        fprintf(ofp,"AC3Db\n");
        	fprintf(ofp,"MATERIAL \"white\" rgb 0.788 0.788 0.788  amb 0.788 0.788 0.788  emis 0 0 0  spec 1 1 1  shi 65  trans 0\n");
	        fprintf(ofp,"OBJECT world\nkids %d\n", 1);
		fprintf(ofp,"OBJECT polyline\nname \"%s\"\ncrease 89.0\nnumvert %d\n", argv[optind], count);
		for(i=0;i<count;i++)
		{
			fprintf(ofp, "%0.4f 0.0 %0.4f\n", airfoil.DATAX[i], airfoil.DATAY[i]);
		}
		fprintf(ofp,"numsurf 1\nSURF 0x31\nmat 0\nrefs %d\n", count);
        	for(i=0;i<count;i++)
	        {
        	        fprintf(ofp, "%d 0.0 0.0\n", i);
	        }

		fprintf(ofp,"kids 0\n");
	} else {
		for(i=0;i<airfoil.COUNT;i++)
		{
			fprintf(ofp, "%0.4f %0.4f\n", airfoil.DATAX[i], airfoil.DATAY[i]);
		}
	}
}
#else
extern int verbose;
#endif

int NacaFoil(char *name, struct AIRFOIL *foil, int stations)
{
	double m, p, t;
	int i=16, q;
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
if(verbose > 1 )fprintf(stderr," m = %0.2f, p = %0.2f, t = %0.2f\n", m, p, t);
	naca4digit(m, p, t, foil, stations);
	 break;

	 case '4':
if(verbose > 1 )fprintf(stderr,"%s CASE 4:\n", name);
		bar[0] = name[9];
		bar[1] = 0;
		m = atol(bar)/100.;

		bar[0] = name[10];
		bar[1] = 0;
		p = atol(bar)/10.;

		bar[0] = name[11];
		bar[1] = name[12];
		bar[2] = 0;
		t = atol(bar)/100.;
if(verbose > 1 )fprintf(stderr," m = %0.2f, p = %0.2f, t = %0.2f\n", m, p, t);
	naca4digit(m, p, t, foil, stations);

	 break;

	 case '5':  /* Using Naca 4 series math */
if(verbose > 1 )fprintf(stderr,"%s CASE 5:\n", name);
		bar[0] = name[9];
		bar[1] = 0;
		m = atol(bar)*0.15;
if(verbose > 2 )fprintf(stderr,"design lift coefficient = %s%% (%0.2f)\n", bar, m);
m/=10; //make the 4 series pretty

		bar[0] = name[10];
		bar[1] = 0;
		p = atol(bar)/20.;
if(verbose > 2 )fprintf(stderr,"max camber position = (%s/2)%% (%0.2f)\n", bar, p);
p*=2; //make the 4 series pretty

		bar[0] = name[11];
		bar[1] = 0;
		q = atoi(bar);
if(verbose > 2 )fprintf(stderr,"reflex = %s (%d)\n", bar, q);

		bar[0] = name[12];
		bar[1] = name[13];
		bar[2] = 0;
		t = atol(bar)/100.;
if(verbose > 2 )fprintf(stderr,"thickness = %s%% (%0.2f)\n", bar, t);
if(verbose > 1 )fprintf(stderr," m = %0.2f, p = %0.2f, t = %0.2f\n", m, p, t);
	naca4digit(m, p, t, foil, stations);

	 break;

	 case '6':
// NACA-V-6-631-012
// NACA-W-6-64210.68
// NACA-W-6-64-210.68
// 0123456789012345
if(verbose > 1 )fprintf(stderr,"%s CASE 6\n", name);
		i=11;
		if((name[i] == '-')||(name[i] == 'A')) i++;
		i++;
		if((name[i] == '-')||(name[i] == 'A')) i+=2;
		bar[0]=name[i++];
		bar[1]=name[i];
		bar[2]=0;
		t = atol(bar)/100.;
if(verbose > 2 )fprintf(stderr,"NACA 6 thickness = %s%% = %0.2f\n", bar, t);
if(verbose > 1 )fprintf(stderr," m = %0.2f, p = %0.2f, t = %0.2f\n", m, p, t);
	naca4digit(m, p, t, foil, stations);

	 break;

	 case 'S':
if(verbose > 1 )fprintf(stderr,"%s CASE S\n", name);
if(verbose > 1 )fprintf(stderr," m = %0.2f, p = %0.2f, t = %0.2f\n", m, p, t);
	naca4digit(m, p, t, foil, stations);
	 break;

	 default:
if(verbose > 0 )fprintf(stderr,"%s Unknown airfoil\n", name);
m=0;t=0;
if(verbose > 1 )fprintf(stderr," m = %0.2f, p = %0.2f, t = %0.2f\n", m, p, t);
	naca4digit(m, p, t, foil, stations);
	}


}

void naca4digit(double m, double p, double t, struct AIRFOIL *airfoil, int stations )
{
	int i, j;
	double yc, yt, x2, x3, x4, xroot;
	double Xu, Yu, Xl, Yl,dx=0, dyc=0, oyc=0, theta;
	double *x;
	x=TaperSeq(stations);

airfoil->DATAX = malloc( 2 * stations * sizeof(double) );
airfoil->DATAY = malloc( 2 * stations * sizeof(double) );
airfoil->COUNT = 2 * stations;

	for(i=0, j=2*stations-1;i<stations;i++, j--){
		x2 = x[i] * x[i];
		x3 = x2 * x[i];
		x4 = x3 * x[i];
		xroot = sqrt(x[i]);
		if(x[i] < p){
			yc = (m / (p * p)) * (2*p*x[i] - x2);
		} else {
			yc = ( m /  ((1-p)*(1-p)) ) * ( (1 -2*p)+2*p*x[i]-x2);
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

double *TaperSeq(int s)
{
	double k, f;
	int i;
	double *seq;
	seq=malloc(sizeof(double)*s);

	if(!seq) return 0;

	f=pow(2.,1./(double)(s-1));
	k=1.;
	for(i=0;i<s;i++)
	{
		seq[i]=(k-1.)*(k-1.);
		k *= f;
	}
	return seq;
}

