#include "StdAfx.h"

#include "Line.h"

#include "Math.h"
#include "Math.Matrix.h"
#include "Point.h"

#include "Impl/KeyImpl.h"
#include "Impl/GeometryImpl.h"

#include <HTools.h>

using namespace H3DF;

namespace H3DF
{
	class LineKitImpl : public Impl
	{
	public:
		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<LineKitImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy(const LineKitImpl * that)
		{
			m_aPoints = that->m_aPoints;
			m_cColor = that->m_cColor;
			strncpy(m_chPattern, that->m_chPattern, PATTERN_BUFFER_SIZE);
		}

		PointArray m_aPoints;
		H3DF::RGBColor m_cColor;
		char m_chPattern[PATTERN_BUFFER_SIZE];
	};
}

H3DF::LineKit::LineKit()
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = std::make_unique<LineKitImpl>();
	DEBUG_VALID(m_pcImpl);
}

H3DF::LineKit::LineKit(LineKit const & cInThat)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = (nullptr == cInThat.GetImpl()) ? cInThat.GetImpl()->Clone() : nullptr;
	DEBUG_VALID(m_pcImpl);

}

LineKit & H3DF::LineKit::operator = (LineKit const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}
	return *this;
}

unsigned int H3DF::LineKit::GetPointCount() const
{
	auto pcImpl = static_cast<LineKitImpl *>(m_pcImpl.get());
	return static_cast<unsigned int>(pcImpl->m_aPoints.size());
}

void H3DF::LineKit::GetPoints(unsigned int & nOutCount, H3DF::Point pcOutPoints[]) const
{
	auto pcImpl = static_cast<LineKitImpl *>(m_pcImpl.get());

	nOutCount = GetPointCount();

	if (nullptr == pcOutPoints) {
		return;
	}

	for (unsigned int i = 0; i < nOutCount; i++) {
		pcOutPoints[i] = pcImpl->m_aPoints[i];
	}
}

void H3DF::LineKit::SetPoints(unsigned int nInCount, Point const pcInPoints[])
{
	auto pcImpl = static_cast<LineKitImpl *>(m_pcImpl.get());
	pcImpl->m_aPoints.resize(nInCount);

	for (size_t i = 0; i < nInCount; i++) {
		pcImpl->m_aPoints[i] = pcInPoints[i];
	}
}

void H3DF::LineKit::GetRGBColor(H3DF::RGBColor & cOutColor) const
{
	auto pcImpl = static_cast<LineKitImpl *>(m_pcImpl.get());
	cOutColor = pcImpl->m_cColor;
}

void H3DF::LineKit::SetRGBColor(RGBColor const & cInColor)
{
	auto pcImpl = static_cast<LineKitImpl *>(m_pcImpl.get());
	pcImpl->m_cColor = cInColor;
}

void H3DF::LineKit::GetLinePattern(char pcOutPattern[PATTERN_BUFFER_SIZE]) const
{
	auto pcImpl = static_cast<LineKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	strncpy(pcOutPattern, pcImpl->m_chPattern, PATTERN_BUFFER_SIZE);
}

void H3DF::LineKit::SetLinePattern(char pcInPattern[PATTERN_BUFFER_SIZE]) const
{
	auto pcImpl = static_cast<LineKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	strncpy(pcImpl->m_chPattern, pcInPattern, PATTERN_BUFFER_SIZE);
}

//== LineKey =======================================================================================
namespace H3DF {

	class LineKeyImpl : public GeometryKeyImpl
	{
	public:
		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<LineKeyImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy(const LineKeyImpl * pcInThat) {
			GeometryKeyImpl::Copy(pcInThat);
		}
	};
};

H3DF::LineKey::LineKey()
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = std::make_unique<LineKeyImpl>();
	DEBUG_VALID(m_pcImpl);
}

H3DF::LineKey::LineKey(HC_KEY nInKey)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = std::make_unique<LineKeyImpl>();
	DEBUG_VALID(m_pcImpl);

	auto pcImpl = static_cast<LineKeyImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->SetKeyValue(nInKey);
}

H3DF::LineKey::LineKey(Key const & cInKey)
{
	if (staticType != Type()) {
		return;
	}

	// PolygonShapeElementImpl 생성
	m_pcImpl = std::make_unique<LineKeyImpl>();
	auto pcImpl = static_cast<LineKeyImpl *>(m_pcImpl.get());

	auto pcInThatImpl = static_cast<const KeyImpl *>(cInKey.GetImpl());

	if (nullptr != pcImpl && nullptr != pcInThatImpl) {
		pcImpl->KeyImpl::Copy(pcInThatImpl);
	}
	else {
		DEBUG_STOP;
	}
}

H3DF::LineKey::LineKey(LineKey const & cInThat)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = (nullptr == cInThat.GetImpl()) ? cInThat.GetImpl()->Clone() : nullptr;
	DEBUG_VALID(m_pcImpl);
}

LineKey & H3DF::LineKey::operator=(LineKey const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

int H3DF::LineKey::GetPointCount() const
{
	int nCount = 0;
	HC_Show_Polyline_Count(KeyValue(), &nCount);

	return nCount;
}

bool H3DF::LineKey::ShowPoints(WorldPointArray & aOutPoints) const
{
	int nCount = 0;
	HC_Show_Polyline_Count(KeyValue(), &nCount);
	if (0 == nCount) {
		return false;
	}

	Point * pcPoints = new Point[nCount];

	HC_Show_Polyline(KeyValue(), &nCount, pcPoints);

	aOutPoints.resize(nCount);

	for (int nIndex = 0; nIndex < nCount; nIndex++) {
		aOutPoints[nIndex] = pcPoints[nIndex];
	}

	delete[] pcPoints;

	return true;
}

bool H3DF::LineKey::IsCoincident(const LineKey & cInThat, const MatrixKit & cMatrix1, const MatrixKit & cMatrix2) const
{
	WorldPointArray aPoints;
	if (false == ShowPoints(aPoints)) {
		return false;
	}

	WorldPointArray aInPoints;
	if (false == cInThat.ShowPoints(aInPoints)) {
		return false;
	}

	// 점이 2개 이상인 경우만 처리
	if (1 >= aPoints.size() || 1 >= aInPoints.size()) {
		return false;
	}

	aPoints = cMatrix1.Transform(aPoints);
	aInPoints = cMatrix2.Transform(aInPoints);

	double dLength = 0, dInThatLength = 0;

	for (int nIndex = 0; nIndex < aPoints.size() - 1; nIndex++) {
		dLength += aPoints[nIndex].DistanceWith(aPoints[nIndex + 1]);
	}

	for (int nIndex = 0; nIndex < aInPoints.size() - 1; nIndex++) {
		dInThatLength += aInPoints[nIndex].DistanceWith(aInPoints[nIndex + 1]);
	}

	// 길이가 다르면 false
	if (1e-3 < fabs(dLength - dInThatLength)) {
		return false;
	}

	TRACE(L"dLength, dInThatLength: %f, %f\n", dLength, dInThatLength);

	SimpleCuboid cCuboid, cInThatCuboid;
	cCuboid.Merge(aPoints.size(), aPoints.data());
	cInThatCuboid.Merge(aInPoints.size(), aInPoints.data());

	if (false == cCuboid.Equals(cInThatCuboid)) {
		return false;
	}

	return true;
}

bool H3DF::LineKey::GetEndPoint(Point & cSP, Point & cEP)
{
	WorldPointArray aPoints;
	if (false == ShowPoints(aPoints)) {
		return false;
	}

	// 점이 2개 이상인 경우만 처리
	if (1 >= aPoints.size()) {
		return false;
	}

	cSP = aPoints[0];
	cEP = aPoints[aPoints.size() - 1];

	return true;
}

bool H3DF::LineKey::GetMidPoint(Point & cMP)
{
	WorldPointArray aPoints;
	if (false == ShowPoints(aPoints)) {
		return false;
	}

	// 점이 2개 이상인 경우만 처리
	if (1 >= aPoints.size()) {
		return false;
	}

	Point cSP, cEP;

	if (2 == aPoints.size()) {
		cSP = aPoints[0];
		cEP = aPoints[1];

		cMP = (cSP + cEP) / 2.0;
		return true;
	}

	// 전체 길이 계산
	double dTotalLength = 0.0;
	for (size_t nIndex = 0; nIndex < aPoints.size() - 1; nIndex++) {
		dTotalLength += aPoints[nIndex].DistanceWith(aPoints[nIndex + 1]);
	}

	double dMidLength = dTotalLength / 2.0;
	double dLength = 0;
	dTotalLength = 0;
 
	for (size_t nIndex = 0; nIndex < aPoints.size() - 1; nIndex++) {
		dLength = aPoints[nIndex].DistanceWith(aPoints[nIndex + 1]);
		dTotalLength += dLength;

		if (dMidLength < dTotalLength) {
			double dDiff = dMidLength - (dTotalLength - dLength);

			Vector cVec = aPoints[nIndex + 1] - aPoints[nIndex];
			cVec.Normalize();

			cMP = aPoints[nIndex] + (cVec * (float)dDiff);

			return true;
		}
	}

	return false;
}

bool H3DF::LineKey::GetIntersectionPoint(LineKey & cInLine, PointArray & aOutIntersectionPoints)
{
	WorldPointArray aPoints;
	if (false == ShowPoints(aPoints)) {
		return false;
	}

	WorldPointArray aInPoints;
	if (false == cInLine.ShowPoints(aInPoints)) {
		return false;
	}

	// 점이 2개 이상인 경우만 처리
	if (1 >= aPoints.size() || 1 >= aInPoints.size()) {
		return false;
	}

	return GetIntersectionPoint(aPoints, aInPoints, aOutIntersectionPoints);
}

// Object Snap 계산을 위한 함수, 각 Line이 Modeling Matrix에 의해서 좌표가 변환되어 있기때문에, 이를 고려하여 계산한다.
bool H3DF::LineKey::GetIntersectionPoint(LineKey & cInLine, const MatrixKit & cMatrix1, const MatrixKit & cMatrix2, PointArray & aOutIntersectionPoints)
{
	WorldPointArray aPoints;
	if (false == ShowPoints(aPoints)) {
		return false;
	}

	WorldPointArray aInPoints;
	if (false == cInLine.ShowPoints(aInPoints)) {
		return false;
	}

	// 점이 2개 이상인 경우만 처리
	if (1 >= aPoints.size() || 1 >= aInPoints.size()) {
		return false;
	}

	aPoints = cMatrix1.Transform(aPoints);
	aInPoints = cMatrix2.Transform(aInPoints);

	return GetIntersectionPoint(aPoints, aInPoints, aOutIntersectionPoints);
}

bool H3DF::LineKey::GetIntersectionPoint(const WorldPointArray & aPoints1, const WorldPointArray & aPoints2, PointArray & aOutIntersectionPoints)
{
	bool bResult = false;
	Point cSP[2], cEP[2], cIntersectionPoint;

	// 직선인 경우 범위밖에 있는 교차점도 구하도록 한다.
	if (2 == aPoints1.size() && 2 == aPoints2.size()) {
		cSP[0] = aPoints1[0];
		cEP[0] = aPoints1[1];

		cSP[1] = aPoints2[0];
		cEP[1] = aPoints2[1];

		if (true == Math::IntersectionPoint(cSP[0], cEP[0], cSP[1], cEP[1], cIntersectionPoint)) {
			aOutIntersectionPoints.push_back(cIntersectionPoint);
			bResult = true;
		}

		return bResult;
	}

	// aPoints1와 aPoints2를 비교해서 교차점을 구함.
	for (size_t nIndex = 0; nIndex < aPoints1.size() - 1; nIndex++) {
		cSP[0] = aPoints1[nIndex];
		cEP[0] = aPoints1[nIndex + 1];

		for (size_t nInIndex = 0; nInIndex < aPoints2.size() - 1; nInIndex++) {
			cSP[1] = aPoints2[nInIndex];
			cEP[1] = aPoints2[nInIndex + 1];

			if (true == Math::IntersectionPointInRange(cSP[0], cEP[0], cSP[1], cEP[1], cIntersectionPoint)) {
				aOutIntersectionPoints.push_back(cIntersectionPoint);
				bResult = true;
			}
		}
	}

	return bResult;
}

//== 계산 함수 ===================================================================================
bool H3DF::LineKey::NearPoint(WindowKey const & cInWindow, const MatrixKit & cModelingMatrix, const WindowPoint & cInPoint, WorldPoint & cOutPoint) const
{
	WorldPointArray aPoints;

	if (false == ShowPoints(aPoints)) {
		return false;
	}

	aPoints = cModelingMatrix.Transform(aPoints);

	// 선택 정밀도를 높이기 위해서 스크린 좌표계로 환산해서 계산한다.
	// Z 좌표는 작은 값으로 0.01 정도로 나오기 때문에, 계산에 주는 영향이 적다고 가정한다.
	PixelPointArray aPixelPoints;
	Math::GetPoint(cInWindow, aPoints, aPixelPoints);

	PixelPoint cInPixelPoint(cInWindow, cInPoint);

	double dDist;
	double dMinDist = DBL_MAX;
	double dParameter = 0;

	WorldPoint cNomalPoint;

	bool bResultFlag = false;

	for (size_t nIndex = 0; nIndex < aPoints.size() - 1; nIndex++) {
		if (true == Math::NormalPointWithInRange(aPixelPoints[nIndex], aPixelPoints[nIndex + 1], cInPixelPoint, cNomalPoint, dParameter)) {
			dDist = cInPixelPoint.DistanceWith(cNomalPoint);
			if (dDist < dMinDist) {
				dMinDist = dDist;
				Vector cVec = aPoints[nIndex + 1] - aPoints[nIndex];
				cOutPoint = aPoints[nIndex] + cVec * (float)dParameter;
				bResultFlag = true;
			}
		}
	}

	return bResultFlag;
}

bool H3DF::LineKey::DistanceToPoint(const WorldPoint & cInPoint, double & nOutDistance) const
{
/*
	PointArray aPoints;

	if (false == ShowPoints(aPoints)) {
		return false;
	}

	MbCurve3D
*/

	return false;
}

bool H3DF::LineKey::Length(double & dLength) const
{
	int nCount = 0;
	HC_Show_Polyline_Count(KeyValue(), &nCount);
	if (0 == nCount) {
		return false;
	}

	Point * pcPoints = new Point[nCount];

	HC_Show_Polyline(KeyValue(), &nCount, pcPoints);

	dLength = -1;

	for (int nIndex = 0; nIndex < nCount - 1; nIndex++) {
		dLength += pcPoints[nIndex].DistanceWith(pcPoints[nIndex + 1]);
	}

	delete[] pcPoints;

	if (0 > dLength) {
		return false;
	}

	return true;
}