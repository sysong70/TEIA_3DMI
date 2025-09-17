#include "StdAfx.h"

#include "MarkerAttribute.h"

#include "Segment.h"
#include "./Impl/SegmentImpl.h"
#include "./Impl/ControlImpl.h"

#include "3DF.Utility.h"

#include <hc.h>



using namespace H3DF;

namespace H3DF
{
	class MarkerAttributeControlImpl : public ControlImpl
	{
	public:

		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<MarkerAttributeControlImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy(const MarkerAttributeControlImpl * pcInThat) {
		}
	};
}

H3DF::MarkerAttributeControl::MarkerAttributeControl(SegmentKey cInSegment)
{
	m_pcImpl = std::make_unique<MarkerAttributeControlImpl>();
	DEBUG_VALID(m_pcImpl);

	auto pcImpl = static_cast<MarkerAttributeControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_cOverrideKey = cInSegment;
}

MarkerAttributeControl const & H3DF::MarkerAttributeControl::operator = (MarkerAttributeControl const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

MarkerAttributeControl & H3DF::MarkerAttributeControl::SetSize(float fInSize, Marker::SizeUnits nInUnits)
{
	auto pcImpl = static_cast<MarkerAttributeControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		HC_Set_Marker_Size(fInSize);
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}