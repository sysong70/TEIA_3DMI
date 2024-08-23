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
		MarkerAttributeControlImpl() { m_eType = H3DF::Type::MarkerAttributeControl; }

		void Copy(MarkerAttributeControlImpl * pcInThat) {
		}
	};
}

H3DF::MarkerAttributeControl::MarkerAttributeControl(SegmentKey cInSegmentKey)
{
	MarkerAttributeControlImpl * pcImpl = new MarkerAttributeControlImpl();
	DEBUG_VALID(pcImpl);

	pcImpl->m_cOverrideKey = cInSegmentKey;

	m_pcImpl = pcImpl;
}

void H3DF::MarkerAttributeControl::Set(MarkerAttributeControl const & cInThat)
{
	MarkerAttributeControlImpl * pcImpl = static_cast<MarkerAttributeControlImpl *>(m_pcImpl);
	MarkerAttributeControlImpl * pcInThatImpl = static_cast<MarkerAttributeControlImpl *>(cInThat.m_pcImpl);
	pcImpl->Copy(pcInThatImpl);
}

MarkerAttributeControl const & H3DF::MarkerAttributeControl::operator = (MarkerAttributeControl const & cInThat)
{
	Set(cInThat);
	return *this;
}

MarkerAttributeControl & H3DF::MarkerAttributeControl::SetSize(float fInSize, Marker::SizeUnits nInUnits)
{
	MarkerAttributeControlImpl * pcImpl = static_cast<MarkerAttributeControlImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		HC_Set_Marker_Size(fInSize);
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}