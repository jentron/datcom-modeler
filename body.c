#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define pi 3.1415927

#include "modeler.h"
#include "modeler_proto.h"


#ifdef STANDALONE
header(int verts)
{
 fprintf(stdout,"AC3Db\nMATERIAL \"ac3dmat1\" rgb 0.5 1 0.2  amb 0.5 1.0 0.2  emis 0 0 0  spec 0 0 0  shi 10  trans 0\nOBJECT world\n");
 fprintf(stdout,"kids 1\nOBJECT poly\nname \"ellipse\"\nloc 0 0 0\ncrease 45.000000\nnumvert %d\n", verts);
}

footer(int surfs)
{
 int i;
 fprintf(stdout,"numsurf 1\nSURF 0x30\nmat 0\nrefs %d\n", surfs);
 for (i = surfs-1; i>=0;i--)
 {
  fprintf(stdout,"%d 0 0\n", i);
 }
 fprintf(stdout,"kids 0\n");
}


int main(int argc, char *argv[])
{
	double S;
	int points=40;

	if (argc < 2) return 1;
	S=strtod(argv[1], NULL);

header(points);
//	if(S < pi) return 1;
	GetRib(S, 0., 1., 1., 0, 0, 0, points);
footer(points);
}
#endif

RIB3D *GetRib(double S, double P, double YR, double ZR, double XC, double YC, double ZC, int points)
{
	double d,f=1.0,theta=0.0,step,s_norm, area;
	int i;


	area = (YR*ZR);
	if (area) s_norm = S / area;
	else s_norm = 0.;
	d = (s_norm-pi)/(4.0-pi);
	step = (pi/4.0)/(points/8);
	for(i=0;i < (points/8)+1 ; i++, theta += step)
	{
		f = (fabs(1.0/cos(theta)) - 1) * d  + 1;
		fprintf(stdout,"%f %f %f\n", XC, cos(theta)*f*ZR+ZC, sin(theta)*f*YR+YC);
	}
	for(i=0 ;i < points/4 ; i++, theta += step)
	{
		f = (fabs(1.0/sin(theta)) - 1) * d + 1;
		fprintf(stdout,"%f %f %f\n", XC, cos(theta)*f*ZR+ZC, sin(theta)*f*YR+YC);
	}
	for(i=0;i < points/4; i++, theta += step)
	{
		f = (fabs (1.0/cos(theta)) - 1) * d + 1;
		fprintf(stdout,"%f %f %f\n", XC, cos(theta)*f*ZR+ZC, sin(theta)*f*YR+YC);
	}
	for(i=0 ;i < points/4 ; i++, theta += step)
	{
		f = (fabs(1.0/sin(theta)) - 1) * d + 1;
		fprintf(stdout,"%f %f %f\n", XC, cos(theta)*f*ZR+ZC, sin(theta)*f*YR+YC);
	}
	for(i=0;i < (points/8) - 1; i++, theta += step)
	{
		f = (fabs (1.0/cos(theta)) - 1) * d + 1;
		fprintf(stdout,"%f %f %f\n", XC, cos(theta)*f*ZR+ZC, sin(theta)*f*YR+YC);
	}
	
}
