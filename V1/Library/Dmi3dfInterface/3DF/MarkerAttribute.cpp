#include "StdAfx.h"

#include "MarkerAttribute.h"

#include "Segment.h"
#include "./Impl/SegmentImpl.h"

#include "3DF.Utility.h"

#include <hc.h>

using namespace H3DF;

class MarkerAttributeControlPrivate : public Impl
{
public:
	SegmentKey & GetSegmentKey () { return m_cKey; }

	SegmentKey m_cKey;
};

H3DF::MarkerAttributeControl::MarkerAttributeControl(SegmentKey nInKey)
{
	MarkerAttributeControlPrivate * pcImpl = new MarkerAttributeControlPrivate();
	if (nullptr == pcImpl) { assert(false); }

	m_pcImpl = pcImpl;

	pcImpl->m_cKey = nInKey;
}

MarkerAttributeControl & H3DF::MarkerAttributeControl::SetSize(float fInSize, Marker::SizeUnits nInUnits)
{
	MarkerAttributeControlPrivate * pcImpl = static_cast<MarkerAttributeControlPrivate *>(m_pcImpl);
	if (nullptr == pcImpl) { assert(false); }

	SegmentKeyImpl::LocalOpen(pcImpl->GetSegmentKey()); {
		HC_Set_Marker_Size(fInSize);
	} SegmentKeyImpl::LocalClose(pcImpl->GetSegmentKey());

	return *this;
}