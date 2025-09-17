#include "StdAfx.h"

#include "Include.h"

#include "./Impl/KeyImpl.h"

#include "Segment.h"

#include "Condition.h"

#include <HTools.h>

using namespace H3DF;

namespace H3DF
{
	class IncludeKeyImpl : public KeyImpl
	{
	public:
		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<IncludeKeyImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy(const IncludeKeyImpl * pcInThat) {
			KeyImpl::Copy(pcInThat);
		}
	};
}

H3DF::IncludeKey::IncludeKey()
{
}

H3DF::IncludeKey::IncludeKey(HC_KEY nInKey)
{
	m_pcImpl = std::make_unique<IncludeKeyImpl>();
	DEBUG_VALID(m_pcImpl);

	auto pcImpl = static_cast<IncludeKeyImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->SetKeyValue(nInKey);
}

H3DF::IncludeKey::IncludeKey(IncludeKey const & cInThat)
{
	m_pcImpl = (nullptr != cInThat.GetImpl()) ? cInThat.GetImpl()->Clone() : nullptr;
	DEBUG_VALID(m_pcImpl);
}

IncludeKey & H3DF::IncludeKey::operator = (IncludeKey const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

SegmentKey H3DF::IncludeKey::GetTarget() const
{
	HC_KEY nSegment = HC_Show_Include_Segment(KeyValue(), nullptr);

	SegmentKey cSegment(nSegment);
	return cSegment;
}

IncludeKey & H3DF::IncludeKey::SetConditionalExpression(ConditionalExpression const & cInConditional)
{
	CStringA strText;
	HC_Show_Include_Segment(KeyValue(), strText.GetBuffer(MVO_BUFFER_SIZE));

	CStringA strCondition;
	cInConditional.ShowCondition(strCondition);
	
	HC_Conditional_Include_By_Key(KeyValue(), strCondition);

	return *this;
}

IncludeKey & H3DF::IncludeKey::UnsetConditionalExpression()
{
	return *this;
}

bool H3DF::IncludeKey::ShowConditionalExpression(ConditionalExpression & cOutConditional) const
{
	return true;
}
