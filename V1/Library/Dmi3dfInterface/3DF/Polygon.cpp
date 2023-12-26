#include "StdAfx.h"

#include "Polygon.h"

#include "Math.h"

using namespace H3DF;

//== PolygonKey ====================================================================================

namespace H3DF
{
	class PolygonKitImpl : public Impl
	{
	public:
		PolygonKitImpl()
		{
			m_cColor.Set(-1, -1, -1);
		}

		~PolygonKitImpl() {}

		void Copy(PolygonKitImpl * pcInThat)
		{
			m_aPoints = pcInThat->m_aPoints;
			m_cColor = pcInThat->m_cColor;
		}

		PointArray m_aPoints;
		H3DF::RGBColor m_cColor;
	};
}

H3DF::PolygonKit::PolygonKit()
{
	m_pcImpl = new PolygonKitImpl();
}

H3DF::PolygonKit::PolygonKit(PolygonKit const & cInThat)
{
	m_pcImpl = new PolygonKitImpl();
	Set(cInThat);
}

void H3DF::PolygonKit::Set(PolygonKit const & cInThat)
{
	PolygonKitImpl * pcImpl = (PolygonKitImpl *)m_pcImpl;
	PolygonKitImpl * pcInThatImpl = (PolygonKitImpl *)cInThat.m_pcImpl;

	pcImpl->Copy(pcInThatImpl);
}

PolygonKit const & H3DF::PolygonKit::operator=(PolygonKit const & cInThat)
{
	Set(cInThat);
	return *this;
}

unsigned int H3DF::PolygonKit::GetPointCount() const
{
	PolygonKitImpl * pcImpl = (PolygonKitImpl *)m_pcImpl;
	return static_cast<unsigned int>(pcImpl->m_aPoints.size());
}

void H3DF::PolygonKit::GetPoints(unsigned int & nOutCount, H3DF::Point * pcOutPoints) const
{
	if (nullptr == pcOutPoints) {
		nOutCount = 0;
		return;
	}

	PolygonKitImpl * pcImpl = (PolygonKitImpl *)m_pcImpl;
	nOutCount = GetPointCount();

	for (unsigned int i = 0; i < nOutCount; i++) {
		pcOutPoints[i] = pcImpl->m_aPoints[i];
	}
}

// Replace the points on this PolygonKey with the specified points.
PolygonKit & H3DF::PolygonKit::SetPoints(size_t nInCount, Point const cInPoints[])
{
	((PolygonKitImpl *)m_pcImpl)->m_aPoints.clear();

	for (size_t nIndex = 0; nIndex < nInCount; nIndex++) {
		((PolygonKitImpl *)m_pcImpl)->m_aPoints.push_back(cInPoints[nIndex]);
	}

	return *this;
}

void H3DF::PolygonKit::GetRGBColor(H3DF::RGBColor & cOutColor) const
{
	PolygonKitImpl * pcImpl = (PolygonKitImpl *)m_pcImpl;
	cOutColor = pcImpl->m_cColor;
}

void H3DF::PolygonKit::SetRGBColor(H3DF::RGBColor const & cInColor)
{
	PolygonKitImpl * pcImpl = (PolygonKitImpl *)m_pcImpl;
	pcImpl->m_cColor = cInColor;
}


//== PolygonKey ====================================================================================
H3DF::PolygonKey::PolygonKey() : GeometryKey()
{
}

H3DF::PolygonKey::PolygonKey(HC_KEY nInKey) : GeometryKey(nInKey)
{
}
