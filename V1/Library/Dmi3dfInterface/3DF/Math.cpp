#include "StdAfx.h"

#include "Math.h"

#include "Window.h"

#include "Circle.h"

#include <hc.h>

USING_3DF_NAMESPACE

const float H3DF::Float::Infinity = ((float)(_HUGE_ENUF * _HUGE_ENUF));
const float H3DF::Float::NegativeInfinity = -H3DF::Float::Infinity;

/*
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
*/

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

bool H3DF::Math::GetCircle(WorldPointArray & cPoints, CircleKit & cCircle)
{
	//cPoints의 갯수가 4개 이상인 경우에만 계산한다.
	if (4 > cPoints.size()) {
		return false;
	}

	// 들어온 3개의 Point를 이용해서 Normal Vector를 구한다.
	DVector cVec1 = cPoints[1] - cPoints[0];
	DVector cVec2 = cPoints[2] - cPoints[0];
	DVector cNormal = cVec1.Cross(cVec2);

	// 3개의 점이 직선인 경우에는 Circle을 구할 수 없다.
	if (cNormal.LengthSquared() < std::numeric_limits<double>::epsilon()) {
		return false;
	}

	cNormal.Normalize();

	DVector cXAxis = cVec1;
	cXAxis.Normalize();

	DVector cYAxis = cXAxis.Cross(cNormal);

	DPoint cOrigin = cPoints[0];	

	// Projection Point 계산 함수에서 dParameter값이 Zero라면 같은 평면이라고 생각할 수 있으므로
	// 간단히 수정해서 사용한다.

	// Origin Point를 ZAxis 방향으로 투영한 내적값, 물론 ZAxis는 단위 벡터라야함.
	double dOriginPointProjectionDistance = cNormal.Dot(cOrigin);

	DPoint2DArray cPoints2D;
	cPoints2D.resize(cPoints.size());

	// 순회하면서 Points의 모든 점이 같은 평면에 있는지 확인한다.
	for (size_t nIndex = 0; nIndex < cPoints.size(); nIndex++)
	{
		// Target Point를 ZAxis 방향으로 투영한 내적값.
		double dInputPointProjectionDistance = cNormal.Dot(cPoints[nIndex]);

		// 두 내적값의 차이
		double dParameter = dOriginPointProjectionDistance - dInputPointProjectionDistance;

		// 3개의 점이 평면에 있지 않은 경우에는 Circle을 구할 수 없다.
		if (abs(dParameter) > 1.0e-4) {
			return false;
		}

		DPoint cPlanePoint = cPoints[nIndex] + cNormal * dParameter;
		DPoint2D cDropPoint = cPlanePoint.DropPoint(cOrigin, cXAxis, cYAxis);

		cPoints2D[nIndex] = cDropPoint;	
	}

	double dRadius, dSigma;

	DPoint2D cPlaneCenterPoint;

	if (false == CircleFitByHyper(cPoints2D, cPlaneCenterPoint.x, cPlaneCenterPoint.y, dRadius, dSigma)) {
		return false;
	}

	if (0.001 < dSigma) {
		return false;
	}

	DPoint cCenterPoint = cPlaneCenterPoint.LiftPoint(cOrigin, cXAxis, cYAxis);
	
	cCircle.SetCenter(cCenterPoint);
	cCircle.SetRadius(dRadius);
	cCircle.SetXAxis(cXAxis);
	cCircle.SetYAxis(cYAxis);

	return true;
}

//  2D Point를 이용해서 Circle을 계산하기 때문에 2D Point만 사용해야 한다.
bool H3DF::Math::CircleFitByHyper(DPoint2DArray & cPoints, double & dCX, double & dCY, double & dRadius, double & dSigma)
{
	int nIter, nIterMax = 99;

	double dXi, dYi, dZi;
	double Mz, Mxy, Mxx, Myy, Mxz, Myz, Mzz, Cov_xy, Var_z;
	double A0, A1, A2, A22;
	double Dy, xnew, x, ynew, y;
	double DET, Xcenter, Ycenter;

	double dAverX = 0.0, dAverY = 0.0;

	size_t nPointCount = cPoints.size();

	for (size_t nIndex = 0; nIndex < nPointCount; nIndex++)
	{
		dAverX += cPoints[nIndex].x;
		dAverY += cPoints[nIndex].y;
	}

	dAverX /= (double)nPointCount;
	dAverY /= (double)nPointCount;

	Mxx = Myy = Mxy = Mxz = Myz = Mzz = 0.;

	for (int nIndex = 0; nIndex < nPointCount; nIndex++)
	{
		dXi = cPoints[nIndex].x - dAverX;   //  centered x-coordinates
		dYi = cPoints[nIndex].y - dAverY;   //  centered y-coordinates
		dZi = dXi * dXi + dYi * dYi;

		Mxy += dXi * dYi;
		Mxx += dXi * dXi;
		Myy += dYi * dYi;
		Mxz += dXi * dZi;
		Myz += dYi * dZi;
		Mzz += dZi * dZi;
	}

	Mxx /= nPointCount;
	Myy /= nPointCount;
	Mxy /= nPointCount;
	Mxz /= nPointCount;
	Myz /= nPointCount;
	Mzz /= nPointCount;

	// computing the coefficients of the characteristic polynomial

	Mz = Mxx + Myy;
	Cov_xy = Mxx * Myy - Mxy * Mxy;
	Var_z = Mzz - Mz * Mz;

	A2 = 4.0 * Cov_xy - 3.0 * Mz * Mz - Mzz;
	A1 = Var_z * Mz + 4.0 * Cov_xy * Mz - Mxz * Mxz - Myz * Myz;
	A0 = Mxz * (Mxz * Myy - Myz * Mxy) + Myz * (Myz * Mxx - Mxz * Mxy) - Var_z * Cov_xy;
	A22 = A2 + A2;

	//    finding the root of the characteristic polynomial
	//    using Newton's method starting at x=0  
	//     (it is guaranteed to converge to the right root)

	for (x = 0.0, y = A0, nIter = 0; nIter < nIterMax; nIter++)  // usually, 4-6 iterations are enough
	{
		Dy = A1 + x * (A22 + 16.0 * x * x);
		xnew = x - y / Dy;
		//if ((xnew == x) || (!isfinite(xnew))) {
		if (xnew == x) {
			break;
		}

		ynew = A0 + xnew * (A1 + xnew * (A2 + 4.0 * xnew * xnew));

		if (abs(ynew) >= abs(y)) {
			break;
		}

		x = xnew;  y = ynew;
	}

	// computing paramters of the fitting circle

	DET = x * x - x * Mz + Cov_xy;
	if (0.0 == DET) {
		return false;
	}

	Xcenter = (Mxz * (Myy - x) - Myz * Mxy) / DET / 2.0;
	Ycenter = (Myz * (Mxx - x) - Mxz * Mxy) / DET / 2.0;

	// assembling the output

	dCX = Xcenter + dAverX;
	dCY = Ycenter + dAverY;
	dRadius = sqrt(Xcenter * Xcenter + Ycenter * Ycenter + Mz - x - x);

	// 	circle.a = Xcenter + dAverX;
	// 	circle.b = Ycenter + dAverY;
	// 	circle.r = sqrt(Xcenter*Xcenter + Ycenter*Ycenter + Mz - x - x);
	// 	circle.s = Sigma(data, circle);
	// 	circle.i = 0;
	// 	circle.j = iter;  //  return the number of iterations, too

	double sum = 0.0, dx, dy, dTemp;

	for (int nIndex = 0; nIndex < nPointCount; nIndex++)
	{
		dx = cPoints[nIndex].x - dCX;   //  centered x-coordinates
		dy = cPoints[nIndex].y - dCY;   //  centered y-coordinates
		dTemp = sqrt(dx * dx + dy * dy) - dRadius;
		sum += dTemp * dTemp;
	}

	dSigma = sqrt(sum / nPointCount);

	return true;
}


// Line 범위내에 있는 경우에만 Normal Point 값을 계산.
bool H3DF::Math::NormalPointWithInRange(Point cSP, Point cEP, Point cOtherPoint, Point & cNormalPoint)
{
	double dPrameter;
	return NormalPointWithInRange(cSP, cEP, cOtherPoint, cNormalPoint, dPrameter);
}

bool H3DF::Math::NormalPointWithInRange(Point cSP, Point cEP, Point cOtherPoint, Point & cNormalPoint, double & dPrameter)
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

bool H3DF::Math::IntersectionPoint(Point cSP1, Point cEP1, Point cSP2, Point cEP2, Point & cIntersectionPoint)
{
	DVector cVec1 = cEP1 - cSP1;
	DVector cVec2 = cEP2 - cSP2;
	DVector cStartVec = cSP2 - cSP1;

	DVector cNormal = cVec1.Cross(cVec2);

	double dNormalLengthSquared = cNormal.LengthSquared();

	// 라인이 같은 평면에 존재하는지 여부 확인
	if (dNormalLengthSquared < 1.0e-9) {
		return false;
	}

	// 교차점을 계산하기 위한 파라미터 계산
	double dParam = cStartVec.Cross(cVec2).Dot(cNormal) / dNormalLengthSquared;
	if (dParam < 1.0e-9) {
		return false;
	}

	// 두 라인의 교차점 계산
	cIntersectionPoint = cSP1 + cVec1 * dParam;

	return true;
}

bool H3DF::Math::IntersectionPointInRange(Point cSP1, Point cEP1, Point cSP2, Point cEP2, Point & cIntersectionPoint)
{
	DVector cVec1 = cEP1 - cSP1;
	DVector cVec2 = cEP2 - cSP2;
	DVector cStartVec = cSP2 - cSP1;

	DVector cNormal = cVec1.Cross(cVec2);

	double dNormalLengthSquared = cNormal.LengthSquared();

	//TRACE(L"NL: %f\n", dNormalLengthSquared);

/*
	vec3 p2_p1 = sub_vec3(p2, p1);
	vec3 cross_v1_v2 = cross(v1, v2);
	double denom = dot(v1, cross_v1_v2);
	if (denom == 0.0) { // 두 직선이 평행한 경우
		return false;
	}
*/

	// 라인이 같은 평면에 존재하는지 여부 확인
	// 여기서 denom은 선분과 평면이 만드는 교차점을 계산하기 위한 중요한 값 중 하나로, 선분의 방향벡터와 평면의 법선벡터가 이루는 각에 대한 코사인값을 나타냅니다.
	// 따라서, denom 값이 0에 가까울수록 선분과 평면이 평행에 가까우며, denom 값이 0이라면 선분과 평면이 평행합니다.
	// 그렇지 않은 경우, t = (point - line.start).dot(normal) / denom 식을 사용하여 선분과 평면이 만나는 점의 위치를 계산할 수 있습니다.
	double denom = cVec1.Dot(cNormal);
	if (denom < 1.0e-9) {
	//if (dNormalLengthSquared < 1.0e-9) {
// 		if ((cNormal.x) != 0 || (cNormal.y) != 0 || (cNormal.z) != 0) {
// 			// "Lines are parallel and not in the same plane"
// 		}
// 		else {
// 			// "Lines are parallel"/
// 		}

		return false;
	}

	// 교차점을 계산하기 위한 파라미터 계산
	double dParam1 = cStartVec.Cross(cVec2).Dot(cNormal) / dNormalLengthSquared;

	// Line 1의 범위내에 있는지 여부 확인
	if (dParam1 < 0.0 || dParam1 > 1.0) {
		return false;
	}

	// 교차점을 계산하기 위한 파라미터 계산
	double dParam2 = cStartVec.Cross(cVec1).Dot(cNormal) / dNormalLengthSquared;

	//TRACE(L"Pa1: %f, Pa2: %f\n", dParam1, dParam2);

	// Line 2의 범위내에 있는지 여부 확인
	if (dParam2 < 0.0 || dParam2 > 1.0) {
		return false;
	}

	// 두 라인의 교차점 계산
	cIntersectionPoint = cSP1 + cVec1 * dParam1;

	return true;
}

bool H3DF::Math::GetPoint(WindowKey const & cInWindow, WorldPointArray const & aInPoints, PixelPointArray & aOutPoints)
{
	size_t nCount = aInPoints.size();

	if (0 == nCount) {
		return false;
	}

	aOutPoints.resize(nCount);

	HC_Open_Segment_By_Key(cInWindow.GetSceneKey()); {
		for (size_t nIndex = 0; nIndex < nCount; nIndex++) {
			HC_Compute_Coordinates(".", "world", &aInPoints[nIndex], "local pixels", &aOutPoints[nIndex]);
		}
	} HC_Close_Segment();

	return true;
}

bool H3DF::Math::GetPoint(WindowKey const & cInWindow, WorldPointArray const & aInPoints, WindowPointArray & aOutPoints)
{
	size_t nCount = aInPoints.size();

	if (0 == nCount) {
		return false;
	}

	aOutPoints.resize(nCount);

	HC_Open_Segment_By_Key(cInWindow.GetSceneKey()); {
		for (size_t nIndex = 0; nIndex < nCount; nIndex++) {
			HC_Compute_Coordinates(".", "world", &aInPoints[nIndex], "outer window", &aOutPoints[nIndex]);
		}
	} HC_Close_Segment();

	return true;
}