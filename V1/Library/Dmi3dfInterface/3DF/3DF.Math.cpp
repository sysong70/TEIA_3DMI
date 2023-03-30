#include "StdAfx.h"

#include "3DF.Math.h"

#include "3DF.Window.h"

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

//== 수학 함수 =======================================================================================

// Line 범위내에 있는 경우에만 Normal Point 값을 계산.
bool Math::NormalPointWithInRange(Point cSP, Point cEP, Point cOtherPoint, Point & cNormalPoint)
{
	Vector cLineVector = cEP - cSP;
	cLineVector.Normalize();

	Vector cOtherPointVector = cOtherPoint - cSP;

	float fNormalDistance = cLineVector.Dot(cOtherPointVector);

	cNormalPoint = cSP + (cLineVector * fNormalDistance);

	if (0.0 > fNormalDistance) return false;

	double dDistance = cSP.DistanceWith(cEP);

	if (dDistance >= fNormalDistance) return true;

	return false;
}

bool Math::GetPoint(WindowKey const & cInWindow, WorldPointArray const & aInPoints, PixelPointArray & aOutPoints)
{
	size_t nCount = aInPoints.GetCount();

	if (0 == nCount) {
		return false;
	}

	aOutPoints.SetCount(nCount);

	HC_Open_Segment_By_Key(cInWindow.GetSceneKey()); {
		for (size_t nIndex = 0; nIndex < nCount; nIndex++) {
			HC_Compute_Coordinates(".", "world", &aInPoints[nIndex], "outer pixels", &aOutPoints[nIndex]);
		}
	} HC_Close_Segment();

	return true;
}

bool Math::GetPoint(WindowKey const & cInWindow, WorldPointArray const & aInPoints, WindowPointArray & aOutPoints)
{
	size_t nCount = aInPoints.GetCount();

	if (0 == nCount) {
		return false;
	}

	aOutPoints.SetCount(nCount);

	HC_Open_Segment_By_Key(cInWindow.GetSceneKey()); {
		for (size_t nIndex = 0; nIndex < nCount; nIndex++) {
			HC_Compute_Coordinates(".", "world", &aInPoints[nIndex], "outer window", &aOutPoints[nIndex]);
		}
	} HC_Close_Segment();

	return true;
}