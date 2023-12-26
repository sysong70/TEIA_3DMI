#include "StdAfx.h"

#include "Condition.h"
#include "./Impl/ControlImpl.h"

#include "Segment.h"
#include "./Impl/SegmentImpl.h"

#include "../Impl/ViewImpl.h"

#include <Common_Define.h>

#include <HUtility.h>
#include <HTools.h>

using namespace H3DF;

//== ConditionControlPrivate class =================================================================
namespace H3DF
{
	class ConditionControlPrivate : public ControlImpl
	{
	public:
		ConditionControlPrivate() { m_eType = H3DF::Type::ConditionControl; }

		void Copy(ConditionControlPrivate * pcInThat) {
			ControlImpl::Copy(pcInThat);
		}
	};
}

//== ConditionControl class ========================================================================

H3DF::ConditionControl::ConditionControl(SegmentKey & cInSegmentKey)
{
	ConditionControlPrivate * pcImpl = new ConditionControlPrivate();
	pcImpl->m_cOverrideKey = cInSegmentKey;

	m_pcImpl = pcImpl;
}

H3DF::ConditionControl::ConditionControl(ConditionControl const & cInThat)
{
	m_pcImpl = new ConditionControlPrivate();
	Set(cInThat);
}

void H3DF::ConditionControl::Set(ConditionControl const & cInThat)
{
	ConditionControlPrivate * pcImpl = (ConditionControlPrivate *) m_pcImpl;
	ConditionControlPrivate * pcInThatImpl = (ConditionControlPrivate *) cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

ConditionControl & H3DF::ConditionControl::operator = (ConditionControl const & cInThat)
{
	Set(cInThat);
	return *this;
}

size_t H3DF::ConditionControl::GetCount() const
{
	ConditionControlPrivate * pcImpl = (ConditionControlPrivate *) m_pcImpl;

	char chList[MVO_BUFFER_SIZE];
	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		HC_Show_Conditions(chList);
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return 0;
}