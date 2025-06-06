#include "StdAfx.h"

#include "Polygon.h"

#include "Math.h"

#include "Impl/KeyImpl.h"

using namespace H3DF;

//== PolygonKey ====================================================================================

namespace H3DF
{
	class PolygonKitImpl : public Impl
	{
	public:
		PolygonKitImpl() {
			m_cColor.Set(-1, -1, -1);
		}

		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<PolygonKitImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy(const PolygonKitImpl * pcInThat)
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
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = std::make_unique<PolygonKitImpl>();
	DEBUG_VALID(m_pcImpl);
}

H3DF::PolygonKit::PolygonKit(PolygonKit const & cInThat)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = (nullptr == cInThat.m_pcImpl) ? cInThat.m_pcImpl->Clone() : nullptr;
}

PolygonKit const & H3DF::PolygonKit::operator=(PolygonKit const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

unsigned int H3DF::PolygonKit::GetPointCount() const
{
	auto pcImpl = static_cast<PolygonKitImpl *>(m_pcImpl.get());
	return static_cast<unsigned int>(pcImpl->m_aPoints.size());
}

void H3DF::PolygonKit::GetPoints(unsigned int & nOutCount, H3DF::Point * pcOutPoints) const
{
	if (nullptr == pcOutPoints) {
		nOutCount = 0;
		return;
	}

	auto pcImpl = static_cast<PolygonKitImpl *>(m_pcImpl.get());
	nOutCount = GetPointCount();

	for (unsigned int i = 0; i < nOutCount; i++) {
		pcOutPoints[i] = pcImpl->m_aPoints[i];
	}
}

// Replace the points on this PolygonKey with the specified points.
PolygonKit & H3DF::PolygonKit::SetPoints(size_t nInCount, Point const cInPoints[])
{
	auto pcImpl = static_cast<PolygonKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_aPoints.clear();

	for (size_t nIndex = 0; nIndex < nInCount; nIndex++) {
		pcImpl->m_aPoints.push_back(cInPoints[nIndex]);
	}

	return *this;
}

void H3DF::PolygonKit::GetRGBColor(H3DF::RGBColor & cOutColor) const
{
	auto pcImpl = static_cast<PolygonKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	cOutColor = pcImpl->m_cColor;
}

void H3DF::PolygonKit::SetRGBColor(H3DF::RGBColor const & cInColor)
{
	auto pcImpl = static_cast<PolygonKitImpl *>(m_pcImpl.get());
	pcImpl->m_cColor = cInColor;
}


//== PolygonKey ====================================================================================
namespace H3DF
{
	class PolygonKeyImpl : public KeyImpl
	{
	public:
		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<PolygonKeyImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy(const PolygonKeyImpl * pcInThat)
		{
			KeyImpl::Copy(pcInThat);
		}
	};
}

H3DF::PolygonKey::PolygonKey()
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = std::make_unique<PolygonKeyImpl>();
	DEBUG_VALID(m_pcImpl);
}

H3DF::PolygonKey::PolygonKey(HC_KEY nInKey)
{
	if (staticType != Type()) {
		return;
	}

	if (INVALID_KEY == nInKey) {
		return;
	}

	m_pcImpl = std::make_unique<PolygonKeyImpl>();
	static_cast<PolygonKeyImpl *>(m_pcImpl.get())->SetKeyValue(nInKey);

}
