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

/*
class Line {
public:
	Point p;
	Vector v;

	Line(Point p, Vector v) : p(p), v(v) {}

	Point perpendicular(const Point & q) const {
		Vector w = q - p;
		double t = w.dot(v) / v.dot(v)
		return p + v * t;
	}

	bool contains(const Point & q) const {
		Vector w = q - p;
		double t = w.dot(v) / v.dot(v);
		return (t >= 0 && t <= 1);
	}
};
*/


// Line 범위내에 있는 경우에만 Normal Point 값을 계산.
bool Math::NormalPointWithInRange(Point cSP, Point cEP, Point cOtherPoint, Point & cNormalPoint)
{
	double dPrameter;
	return NormalPointWithInRange(cSP, cEP, cOtherPoint, cNormalPoint, dPrameter);
}

bool Math::NormalPointWithInRange(Point cSP, Point cEP, Point cOtherPoint, Point & cNormalPoint, double & dPrameter)
{
	Vector w = cOtherPoint - cSP;
	Vector v = cEP - cSP;
	double t = w.Dot(v) / v.Dot(v);
	
	// t가 0보다 작거나 1보다 크면 범위 밖에 있음.
	if (t < 0.0 || t > 1.0) {
		return false;
	}

	cNormalPoint = cSP + v * (float)t;
	dPrameter = t;

	return true;
}

/*
class Line {
public:
	Point p;
	Vector v;

	Line(Point p, Vector v) : p(p), v(v) {}

	Point perpendicular(const Point &q) const {
		Vector w = q - p;
		double t = w.dot(v) / v.dot(v);
		return p + v * t;
	}

	bool contains(const Point &q) const {
		Vector w = q - p;
		double t = w.dot(v) / v.dot(v);
		return (t >= 0 && t <= 1);
	}

	bool intersects(const Line &l, Point &intersect) const {
		Vector w = p - l.p;
		double d = v.dot(l.v.cross(v));
		if (std::abs(d) < std::numeric_limits<double>::epsilon()) {
			return false;
		}
		double t = l.v.dot(w.cross(v)) / d;
		intersect = l.p + l.v * t;
		return true;
	}
};
bool intersects(const Line & l, Point & intersect) const {
	Vector w = p - l.p;
	double d = v.dot(l.v.cross(v));
	if (std::abs(d) < std::numeric_limits<double>::epsilon()) {
		return false;
	}
	double t = l.v.dot(w.cross(v)) / d;
	intersect = l.p + l.v * t;
	return true;
}
*/

bool Math::IntersectionPoint(Point cSP1, Point cEP1, Point cSP2, Point cEP2, Point & cIntersectionPoint)
{
	Vector cV1 = cEP1 - cSP1;
	Vector cV2 = cEP2 - cSP2;

	double d = cV1.Dot(cV2.Cross(cV1));
	if (std::abs(d) < std::numeric_limits<double>::epsilon()) {
		return false;
	}

	Vector w = cSP1 - cSP2;
	double t = cV2.Dot(w.Cross(cV1)) / d;
	cIntersectionPoint = cSP2 + cV2 * t;

	return true;
}

bool Math::IntersectionPointInRange(Point cSP1, Point cEP1, Point cSP2, Point cEP2, Point & cIntersectionPoint)
{
	Vector cV1 = cEP1 - cSP1;
	Vector cV2 = cEP2 - cSP2;

	double d = cV1.Dot(cV2.Cross(cV1));
	if (std::abs(d) < std::numeric_limits<double>::epsilon()) {
		return false;
	}

	Vector w = cSP1 - cSP2;
	double t = cV2.Dot(w.Cross(cV1)) / d;

	cIntersectionPoint = cSP2 + cV2 * t;

	// t가 0보다 작거나 1보다 크면 범위 밖에 있음.
	if (t < 0.0 || t > 1.0) {
		return false;
	}

	cIntersectionPoint = cSP2 + cV2 * t;

	return true;
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