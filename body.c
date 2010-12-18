#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define pi    3.1415927
#define ROOT2 1.4142136

#include "modeler.h"
#include "modeler_proto.h"

#ifdef STANDALONE
int verbose=3;
header(int verts, FILE *ofp)
{
 fprintf(ofp,"AC3Db\nMATERIAL \"ac3dmat1\" rgb 0.5 1 0.2  amb 0.5 1.0 0.2  emis 0 0 0  spec 0 0 0  shi 10  trans 0\nOBJECT world\n");
 fprintf(ofp,"kids 1\nOBJECT poly\nname \"ellipse\"\nloc 0 0 0\ncrease 45.000000\nnumvert %d\n", verts);
}

footer(int surfs, FILE *ofp)
{
 int i;
 fprintf(ofp,"numsurf 1\nSURF 0x30\nmat 0\nrefs %d\n", surfs);
 for (i = surfs-1; i>=0;i--)
 {
  fprintf(ofp,"%d 0 0\n", i);
 }
 fprintf(ofp,"kids 0\n");
}


int main(int argc, char *argv[])
{
	double S;
	int points=40;

	if (argc < 2) return 1;
	S=strtod(argv[1], NULL);

header(points, stdout);
//	if(S < pi) return 1;
	GetRib(S, 0., 1., 1., 0, 0, 0, points, stdout);
footer(points, stdout);
	exit(EXIT_SUCCESS);
}
#else
extern int verbose;
#endif
/* S=Area, Perimeter, Y_Radius, Z_Radius, X_Center, Y_Center, Z_Center, Number of Points, File Pointer */
RIB3D *GetRib(double S, double P, double YR, double ZR, double XC, double YC, double ZC, int points, FILE *ofp)
{
	double d,f=1.0,theta=0.0,step,s_norm, area;
	int i;

	step = (pi/4.0)/(points/8);
	area = (YR*ZR);
	if (area) s_norm = S / area;
	else s_norm = 0.;

	if (s_norm > 4.0)
		if(verbose > 0) fprintf(stderr,"Detected area (S) greater than physically possible.  Station = %0.3f, Width = %0.3f, Height = %0.3f, Area %0.3f > %0.3f\n",
		XC, YR*2., ZR*2., S, YR*ZR*4.);
	if( s_norm > 3.14 ) /* interpolate towards a square shape */
	{
		d = (s_norm-pi)/(4.0-pi);
		for(i=0;i < (points/8)+1 ; i++, theta += step)
		{
			f = (fabs(1.0/cos(theta)) - 1) * d  + 1;
			fprintf(ofp,"%f %f %f\n", XC, cos(theta)*f*ZR+ZC, sin(theta)*f*YR+YC);
		}
		for(i=0 ;i < points/4 ; i++, theta += step)
		{
			f = (fabs(1.0/sin(theta)) - 1) * d + 1;
			fprintf(ofp,"%f %f %f\n", XC, cos(theta)*f*ZR+ZC, sin(theta)*f*YR+YC);
		}
		for(i=0;i < points/4; i++, theta += step)
		{
			f = (fabs (1.0/cos(theta)) - 1) * d + 1;
			fprintf(ofp,"%f %f %f\n", XC, cos(theta)*f*ZR+ZC, sin(theta)*f*YR+YC);
		}
		for(i=0 ;i < points/4 ; i++, theta += step)
		{
			f = (fabs(1.0/sin(theta)) - 1) * d + 1;
			fprintf(ofp,"%f %f %f\n", XC, cos(theta)*f*ZR+ZC, sin(theta)*f*YR+YC);
		}
		for(i=0;i < (points/8) - 1; i++, theta += step)
		{
			f = (fabs (1.0/cos(theta)) - 1) * d + 1;
			fprintf(ofp,"%f %f %f\n", XC, cos(theta)*f*ZR+ZC, sin(theta)*f*YR+YC);
		}
	} else { /* s_norm < 3.14, interpolate towards a diamond shape */
		d = (s_norm-pi)/(4.0-pi)*(-pi/4);
		for(i=0;i < points/4 ; i++, theta += step)
		{            //     ROOOT2/( 2 * sin(0.75*pi-theta))
			f = (( ROOT2/( 2 * sin(0.75*pi-theta)) ) - 1) * d  + 1;
			fprintf(ofp,"%f %f %f\n", XC, cos(theta)*f*ZR+ZC, sin(theta)*f*YR+YC);
		}
		for(i=0 ;i < points/4 ; i++, theta += step)
		{
			f = (( ROOT2/( 2 * cos(0.75*pi-theta)) ) - 1) * d  + 1;
			fprintf(ofp,"%f %f %f\n", XC, cos(theta)*f*ZR+ZC, sin(theta)*f*YR+YC);
		}
		for(i=0;i < points/4; i++, theta += step)
		{
			f = (( ROOT2/-( 2 * sin(0.75*pi-theta)) ) - 1) * d  + 1;
			fprintf(ofp,"%f %f %f\n", XC, cos(theta)*f*ZR+ZC, sin(theta)*f*YR+YC);
		}
		for(i=0 ;i < points/4 ; i++, theta += step)
		{
			f = (( ROOT2/-( 2 * cos(0.75*pi-theta)) ) - 1) * d  + 1;
			fprintf(ofp,"%f %f %f\n", XC, cos(theta)*f*ZR+ZC, sin(theta)*f*YR+YC);
		}

	}
}

/* Radius, X_Center, Y_Center, Z_Center, X_Rotate, Y_Rotate, Z_rotate, Number of Points, File Pointer */

RIB3D *GetRibCircle(double R, double XC, double YC, double ZC, double Xrot, double Yrot, double Zrot, int points, FILE *ofp)
{
	int i,j;
	double theta, step;
	double omatrix[4][4], xmatrix[4][4], ymatrix[4][4], zmatrix[4][4], fmatrix[4][4];
}

