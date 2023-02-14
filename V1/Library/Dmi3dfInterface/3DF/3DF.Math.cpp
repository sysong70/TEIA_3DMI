#include "StdAfx.h"

#include "3DF.Math.h"

#include <hc.h>

USING_3DF_NAMESPACE

void MatrixCal::InverseMatrix(const float * matrix, float * out_matrix)
{
	HC_Compute_Matrix_Inverse(matrix, out_matrix);
}

void MatrixCal::ComputeMatrixProduct(const float * matrix1, const float * matrix2, float * out_matrix)
{
	HC_Compute_Matrix_Product(matrix1, matrix2, out_matrix);
}

void MatrixCal::ComputeIdentityMatrix(float * out_matrix)
{
	HC_Compute_Identity_Matrix(out_matrix);
}