#include "StdAfx.h"

#include "3DF.Line.h"

#include "3DF.Math.h"
#include "3DF.Point.h"


#include "Private/3DF.KeyPrivate.h"

#include <HTools.h>

USING_3DF_NAMESPACE

class LineKitPrivate : public PrivateImpl
{
public:
	void Copy(LineKitPrivate * that)
	{
		m_aPoints.Copy(that->m_aPoints);
 		m_cColor = that->m_cColor;
 		strncpy(m_chPattern, that->m_chPattern, PATTERN_BUFFER_SIZE);
	}

	PointArray m_aPoints;
	_3DF::RGBColor m_cColor;
	char m_chPattern[PATTERN_BUFFER_SIZE];
};

LineKit::LineKit()
{
	m_pcImpl = new LineKitPrivate();
}

LineKit::LineKit(LineKit const & cInThat)
{
	m_pcImpl = new LineKitPrivate();
	Set(cInThat);
}

void LineKit::Set(LineKit const & cInThat)
{
	LineKitPrivate * pcImpl = (LineKitPrivate *)m_pcImpl;
	LineKitPrivate * pcInThatImpl = (LineKitPrivate *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

LineKit const & LineKit::operator=(LineKit const & cInThat)
{
	Set(cInThat);
	return *this;
}

unsigned int LineKit::GetPointCount() const
{
	LineKitPrivate * pcImpl = (LineKitPrivate *)m_pcImpl;
	return static_cast<unsigned int>(pcImpl->m_aPoints.GetCount());
}

void LineKit::GetPoints(unsigned int & nOutCount, _3DF::Point pcOutPoints[]) const
{
	LineKitPrivate * pcImpl = (LineKitPrivate *)m_pcImpl;

	nOutCount = GetPointCount();

	if (nullptr == pcOutPoints) {
		return;
	}

	for (unsigned int i = 0; i < nOutCount; i++) {
		pcOutPoints[i] = pcImpl->m_aPoints[i];
	}
}

void LineKit::SetPoints(unsigned int nInCount, Point const pcInPoints[])
{
	LineKitPrivate * pcImpl = (LineKitPrivate *)m_pcImpl;
	pcImpl->m_aPoints.SetCount(nInCount);

	for (size_t i = 0; i < nInCount; i++) {
		pcImpl->m_aPoints[i] = pcInPoints[i];
	}
}

void LineKit::GetRGBColor(_3DF::RGBColor & cOutColor) const
{
	LineKitPrivate * pcImpl = (LineKitPrivate *)m_pcImpl;
	cOutColor = pcImpl->m_cColor;
}

void LineKit::SetRGBColor(RGBColor const & cInColor)
{
	LineKitPrivate * pcImpl = (LineKitPrivate *)m_pcImpl;
	pcImpl->m_cColor = cInColor;
}

void LineKit::GetLinePattern(char out_pattern[PATTERN_BUFFER_SIZE]) const
{
	LineKitPrivate * pcImpl = (LineKitPrivate *)m_pcImpl;
	strncpy(out_pattern, pcImpl->m_chPattern, PATTERN_BUFFER_SIZE);
}

//== LineKey =======================================================================================
namespace _3DF {

	class LineKeyPrivate : public _3DF::KeyPrivate
	{
	public:
		_3DF::Type Type() const override { return _3DF::Type::LineKey; }

		void Copy(LineKeyPrivate * pcInThat) {
			KeyPrivate::Copy(pcInThat);
		}
	};
};

LineKey::LineKey()
{
	m_pcImpl = new LineKeyPrivate();
}

LineKey::LineKey(Key const & cInKey)
{
	LineKeyPrivate * pcImpl = new LineKeyPrivate();
	m_pcImpl = pcImpl;

	((KeyPrivate *)pcImpl)->Copy((KeyPrivate *)(cInKey.GetImpl()));
}

LineKey::LineKey(LineKey const & cInThat)
{
	m_pcImpl = new LineKeyPrivate();
	Set(cInThat);
}

void LineKey::Set(LineKey const & cInThat)
{
	if (nullptr == m_pcImpl || nullptr == cInThat.m_pcImpl) {
		return;
	}

	SetKeyValue(cInThat.KeyValue());

	LineKeyPrivate * pcImpl = (LineKeyPrivate *)m_pcImpl;
	LineKeyPrivate * pcInThatImpl = (LineKeyPrivate *)cInThat.m_pcImpl;

	pcImpl->Copy(pcInThatImpl);
}

LineKey & LineKey::operator=(LineKey const & cInThat)
{
	Set(cInThat);
	return *this;
}

int LineKey::GetPointCount() const
{
	int nCount = 0;
	HC_Show_Polyline_Count(KeyValue(), &nCount);

	return nCount;
}

bool LineKey::ShowPoints(WorldPointArray & aOutPoints) const
{
	int nCount = 0;
	HC_Show_Polyline_Count(KeyValue(), &nCount);
	if (0 == nCount) {
		return false;
	}

	Point * pcPoints = new Point[nCount];

	HC_Show_Polyline(KeyValue(), &nCount, pcPoints);

	aOutPoints.SetCount(nCount);

	for (int nIndex = 0; nIndex < nCount; nIndex++) {
		aOutPoints[nIndex] = pcPoints[nIndex];
	}

	delete[] pcPoints;

	return true;
}

bool LineKey::GetEndPoint(Point & cSP, Point & cEP)
{
	WorldPointArray aPoints;
	if (false == ShowPoints(aPoints)) {
		return false;
	}

	// 점이 2개 이상인 경우만 처리
	if (1 >= aPoints.GetCount()) {
		return false;
	}

	cSP = aPoints[0];
	cEP = aPoints[aPoints.GetCount() - 1];

	return true;
}

bool LineKey::GetMidPoint(Point & cMP)
{
	WorldPointArray aPoints;
	if (false == ShowPoints(aPoints)) {
		return false;
	}

	// 점이 2개 이상인 경우만 처리
	if (1 >= aPoints.GetCount()) {
		return false;
	}

	Point cSP, cEP;

	if (2 == aPoints.GetCount()) {
		cSP = aPoints[0];
		cEP = aPoints[1];

		cMP = (cSP + cEP) / 2.0;
		return true;
	}

	// 전체 길이 계산
	double dLength = 0.0;
	for (size_t nIndex = 0; nIndex < aPoints.GetCount() - 1; nIndex++) {
		dLength += aPoints[nIndex].DistanceWith(aPoints[nIndex + 1]);
	}

	dLength = 0;
	double dMidLength = dLength / 2.0;

	for (size_t nIndex = 0; nIndex < aPoints.GetCount() - 1; nIndex++) {
		dLength += aPoints[nIndex].DistanceWith(aPoints[nIndex + 1]);
		if (dMidLength < dLength) {
			double dDiff = dLength - dMidLength;

			Vector cVec = aPoints[nIndex + 1] - aPoints[nIndex];
			cVec.Normalize();

			cMP = aPoints[nIndex + 1] + (cVec * (float)dDiff);

			return true;
		}
	}

	return false;
}
//== 계산 함수 ===================================================================================
bool LineKey::NearPoint(WindowKey const & cInWindow, const WorldPoint & cInPoint, WorldPoint & cOutPoint) const
{
	WorldPointArray aPoints;

	if (false == ShowPoints(aPoints)) {
		return false;
	}

	double dDist;
	double dMinDist = DBL_MAX;
	
	WorldPoint cNomalPoint;

	bool bResultFlag = false;

	for (size_t nIndex = 0; nIndex < aPoints.GetCount() - 1; nIndex++) {
		if (true == Math::NormalPointWithInRange(aPoints[nIndex], aPoints[nIndex + 1], cInPoint, cNomalPoint)) {
			dDist = cInPoint.DistanceWith(cNomalPoint);
			if (dDist < dMinDist) {
				dMinDist = dDist;
				cOutPoint = cNomalPoint;
				bResultFlag = true;
			}
		}
	}

	return bResultFlag;
}

bool LineKey::DistanceToPoint(const WorldPoint & cInPoint, double & nOutDistance) const
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
