#include "matrix.h"

int main(int argc, char *argv[])
{
	Matrix4 foo, a, b;
	Vector4 ipoints[32], opoints[32];
	int i;
	double theta, step;

	matrix_identity(&foo);
printf("The Identity Matrix\n");
	matrix_print(&foo);

printf("The Translate Matrix\n");
	matrix_translate(&foo, 1., 2., 3.);
	matrix_print(&foo);

printf("The X Rotation Matrix\n");
	matrix_rotateXYZ(&foo, 3.1415927, 0, 0);
	matrix_print(&foo);


printf("The Z Rotation Matrix\n");
	matrix_rotateXYZ(&foo, 0, 0, 3.1415927);
	matrix_print(&foo);

printf("The Y Rotation Matrix\n");
	matrix_rotateXYZ(&foo, 0, 3.1415927, 0);
	matrix_print(&foo);

printf("The Un-Rotated Circle\n");
	theta=0.;
	step=3.1415927*2./32.;
	for(i=0;i<32;i++)
	{
		ipoints[i].element[0]=0.;
		ipoints[i].element[1]=cos(theta);
		ipoints[i].element[2]=sin(theta);
		ipoints[i].element[3]= 1.;
		fprintf(stdout,"% 0.4f % 0.4f % 0.4f\n", ipoints[i].element[0], ipoints[i].element[1],ipoints[i].element[2],ipoints[i].element[3]);
		theta += step;
	}

//	matrix_identity(&foo);
	matrix_rotateZ(&a, 0.78539816);
	matrix_rotateX(&b, 0.0);
	matrix_multiply(&a, &b, &foo);
printf("The Matrix A\n");
	matrix_print(&a);
printf("The Matrix B\n");
	matrix_print(&b);
printf("The Matrix Result\n");
	matrix_print(&foo);

printf("The Rotated Circle\n");
	for(i=0;i<32;i++)
	{
		vector_multiply(&foo, &ipoints[i], &opoints[i]);
		fprintf(stdout,"% 0.4f % 0.4f % 0.4f\n", opoints[i].element[0], opoints[i].element[1],opoints[i].element[2]);
		theta += step;
	}



}
