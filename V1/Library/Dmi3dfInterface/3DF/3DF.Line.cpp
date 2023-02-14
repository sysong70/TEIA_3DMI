#include "StdAfx.h"

#include "3DF.Line.h"
#include "3DF.Math.h"

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
LineKey::LineKey(HC_KEY nInKey) :
	GeometryKey(nInKey)
{
}
