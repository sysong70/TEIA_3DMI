#include "StdAfx.h"

#include "3DF.Polygon.h"

#include "3DF.Math.h"

USING_3DF_NAMESPACE

//== PolygonKey ====================================================================================
class PolygonKitPrivate : public PrivateImpl
{
public:
	PolygonKitPrivate()
	{
 		m_cColor.Set(-1, -1, -1);
	}

	~PolygonKitPrivate() {}

	void Copy(PolygonKitPrivate * pcInThat)
	{
		m_aPoints = pcInThat->m_aPoints;
		m_cColor = pcInThat->m_cColor;
	}

	PointArray m_aPoints;
	TDF::RGBColor m_cColor;
};

PolygonKit::PolygonKit()
{
	m_pcImpl = new PolygonKitPrivate();
}

PolygonKit::PolygonKit(PolygonKit const & cInThat)
{
	m_pcImpl = new PolygonKitPrivate();
	Set(cInThat);
}

void PolygonKit::Set(PolygonKit const & cInThat)
{
	PolygonKitPrivate * pcImpl = (PolygonKitPrivate *)m_pcImpl;
	PolygonKitPrivate * pcInThatImpl = (PolygonKitPrivate *)cInThat.m_pcImpl;

	pcImpl->Copy(pcInThatImpl);
}

PolygonKit const & PolygonKit::operator=(PolygonKit const & cInThat)
{
	Set(cInThat);
	return *this;
}

unsigned int PolygonKit::GetPointCount() const
{
	PolygonKitPrivate * pcImpl = (PolygonKitPrivate *)m_pcImpl;
	return static_cast<unsigned int>(pcImpl->m_aPoints.size());
}

void PolygonKit::GetPoints(unsigned int & nOutCount, TDF::Point * pcOutPoints) const
{
	if (nullptr == pcOutPoints) {
		nOutCount = 0;
		return;
	}

	PolygonKitPrivate * pcImpl = (PolygonKitPrivate *)m_pcImpl;
	nOutCount = GetPointCount();

	for (unsigned int i = 0; i < nOutCount; i++) {
		pcOutPoints[i] = pcImpl->m_aPoints[i];
	}
}

// Replace the points on this PolygonKey with the specified points.
PolygonKit & PolygonKit::SetPoints(size_t nInCount, Point const cInPoints[])
{
	((PolygonKitPrivate *)m_pcImpl)->m_aPoints.clear();

	for (size_t nIndex = 0; nIndex < nInCount; nIndex++) {
		((PolygonKitPrivate *)m_pcImpl)->m_aPoints.push_back(cInPoints[nIndex]);
	}

	return *this;
}

void PolygonKit::GetRGBColor(TDF::RGBColor & cOutColor) const
{
	PolygonKitPrivate * pcImpl = (PolygonKitPrivate *)m_pcImpl;
	cOutColor = pcImpl->m_cColor;
}

void PolygonKit::SetRGBColor(TDF::RGBColor const & cInColor)
{
	PolygonKitPrivate * pcImpl = (PolygonKitPrivate *)m_pcImpl;
	pcImpl->m_cColor = cInColor;
}


//== PolygonKey ====================================================================================
PolygonKey::PolygonKey(HC_KEY nInKey) :
	GeometryKey(nInKey)
{
}
