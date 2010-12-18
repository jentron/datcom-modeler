#include "matrix.h"

double TriangleArea(Vector3 *P, Vector3 *Q, Vector3 *R);

int main(int argc, char *argv[])
{
	double area;
	Vector3 P, Q, R;

	P.element[0]=0.;
	P.element[1]=0.;
	P.element[2]=0.;

	Q.element[0]=1.;
	Q.element[1]=0.;
	Q.element[2]=0.;

	R.element[0]=0.;
	R.element[1]=1.;
	R.element[2]=0.;

	area = TriangleArea(&P, &Q, &R);

	fprintf(stdout,"Area = %f\n", area);

}

double TriangleArea(Vector3 *P, Vector3 *Q, Vector3 *R)
{
	Vector3 PQ, PR, Cross;
	double area;

	vector3_subtract(P, Q, &PQ);
	vector3_subtract(P, R, &PR);
vector3_print(&PQ);
vector3_print(&PR);
	vector3_cross(&PQ, &PR, &Cross);
vector3_print(&Cross);
	area=vector3_magnitude(&Cross);
printf("Step 1 = %f\n", area);

	area /= 2;
printf("Step 2 = %f\n", area);
	return(area);

}

