#include <math.h>
#include <stdio.h>

void naca4digit(double m, double p, double t, int stations);
int main()
{
	naca4digit(0.02, 0.4, 0.15, 10); // naca 2415)
}

void naca4digit(double m, double p, double t, int stations)
{
	double x, j;
	j=1.0/(double) stations;

	
//	fprintf(stdout, "calculating %f %f %f\n", m, p, t);

}




