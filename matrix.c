#include "matrix.h"


void matrix_translate(Matrix4* matrix, double dx, double dy, double dz)
{
	matrix_identity(matrix);
	matrix->element[0][3] = dx;
	matrix->element[1][3] = dy;
	matrix->element[2][3] = dz;
}

void matrix_rotateXYZ(Matrix4* matrix, double rx, double ry, double rz)
{
	Matrix4 mx, my, mz, ma;

	matrix_identity(&ma);
	matrix_identity(&mx);
	matrix_identity(&my);
	matrix_identity(&mz);

	mz.element[0][0] =  cos(rz);
	mz.element[0][1] = -sin(rz);
	mz.element[1][0] =  sin(rz);
	mz.element[1][1] =  cos(rz);

	mx.element[1][1] =  cos(rx);
	mx.element[1][2] = -sin(rx);
	mx.element[2][1] =  sin(rx);
	mx.element[2][2] =  cos(rx);

	my.element[0][0] =  cos(ry);
	my.element[0][2] = -sin(ry);
	my.element[2][0] =  sin(ry);
	my.element[2][2] =  cos(ry);

	matrix_multiply(&mx, &my, &ma);
	matrix_multiply(&ma, &mz, matrix);

}

void matrix_rotateZ(Matrix4* matrix, double rz)
{
	matrix_identity(matrix);

	matrix->element[0][0] =  cos(rz);
	matrix->element[0][1] = -sin(rz);
	matrix->element[1][0] =  sin(rz);
	matrix->element[1][1] =  cos(rz);

}

void matrix_rotateX(Matrix4* matrix, double rx)
{
	matrix_identity(matrix);

	matrix->element[1][1] =  cos(rx);
	matrix->element[1][2] = -sin(rx);
	matrix->element[2][1] =  sin(rx);
	matrix->element[2][2] =  cos(rx);

}

void matrix_rotateY(Matrix4* matrix, double ry)
{
	matrix_identity(matrix);

	matrix->element[0][0] =  cos(ry);
	matrix->element[0][2] = -sin(ry);
	matrix->element[2][0] =  sin(ry);
	matrix->element[2][2] =  cos(ry);

}

void matrix_identity(Matrix4* matrix)
{
	matrix->element[0][0] = 1;
	matrix->element[0][1] = 0;
	matrix->element[0][2] = 0;
	matrix->element[0][3] = 0;
	matrix->element[1][0] = 0;
	matrix->element[1][1] = 1;
	matrix->element[1][2] = 0;
	matrix->element[1][3] = 0;
	matrix->element[2][0] = 0;
	matrix->element[2][1] = 0;
	matrix->element[2][2] = 1;
	matrix->element[2][3] = 0;
	matrix->element[3][0] = 0;
	matrix->element[3][1] = 0;
	matrix->element[3][2] = 0;
	matrix->element[3][3] = 1;
}

void matrix_multiply(Matrix4 *a, Matrix4 *b, Matrix4 *result)
{
	int i,j,k;
	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			result->element[i][j] = 0.0;
			for(k=0;k<4;k++)
			{
				result->element[i][j] += b->element[i][k] * a->element[k][j];
			}
		}
	}
}

void vector_multiply(Matrix4 *a, Vector4 *b, Vector4 *result)
{
	int i,j;
	for(i=0;i<4;i++)
	{
		result->element[i] = 0.0;
		for(j=0;j<4;j++)
		{
			result->element[i] += a->element[i][j] * b->element[j];
		}
	}
}


void matrix_add(Matrix4 *a, Matrix4 *b, Matrix4 *result)
{
	int i,j;
	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			result->element[i][j] += a->element[i][j] + b->element[i][j];
		}
	}
}

void matrix_print(Matrix4 *matrix)
{
	int i;
	for(i=0;i<4;i++)
	{
		printf("% 0.4f, % 0.4f, % 0.4f, % 0.4f\n", matrix->element[i][0], matrix->element[i][1], matrix->element[i][2], matrix->element[i][3]);
	}

}

void matrix_fprint(FILE *fp, Matrix4 *matrix)
{
        int i;
        for(i=0;i<4;i++)
        {
                fprintf(fp, "% 0.4f, % 0.4f, % 0.4f, % 0.4f\n", matrix->element[i][0], matrix->element[i][1], matrix->element[i][2], matrix->element[i][3]);
        }

}

