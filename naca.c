#include <math.h>
#include <stdio.h>

void naca4digit(double m, double p, double t, int stations, double *x );
int main()
{
	double x[]={0.0,0.0125,0.025,0.05,0.075,0.1,0.125,0.15,0.175,0.2,0.225,0.25,0.3,0.35,0.4,0.5,0.6,0.7,0.8,1.0};
	naca4digit(0.02, 0.4, 0.15, 20, x); // naca 2415)
}

/* double x[] contains the % cord station values */

void naca4digit(double m, double p, double t, int stations, double *x )
{
	int i;
	double yc, yt, x2, x3, x4, xroot;
	double Xu, Yu, Xl, Yl,dx=0, dyc=0, oyc=0, theta;
	for(i=0;i<stations;i++){
		x2 = x[i] * x[i];
		x3 = x2 * x[i];
		x4 = x3 * x[i];
		xroot = sqrt(x[i]);
		if(x[i] <= p){
			yc = (m / p * p) * (2*p*x[i] - x2);
		} else {
			yc = ( m /  (1-p)*(1-p) ) * ( (1 -2*p)+2*p*x[i]-x2);
		}
		yt = (t/0.2)*(0.2969*xroot - 0.1260 * x[i] - 0.3516*x2 + 0.2843*x3 - 0.1015*x4);
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
	}

}




