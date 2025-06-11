#include "StdAfx.h"

#include "Condition.h"
#include "./Impl/ControlImpl.h"

#include "Segment.h"
#include "./Impl/SegmentImpl.h"

#include "../Sprocket/Impl/3DF.View.Impl.h"

#include <Common_Define.h>

#include <HUtility.h>
#include <HTools.h>

using namespace H3DF;

namespace H3DF
{ 
	class ConditionalExpressionImpl : public Impl
	{
	public:
		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<ConditionalExpressionImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy(const ConditionalExpressionImpl * pcInThat) {
			m_strCondition = pcInThat->m_strCondition;
			m_fNumber = pcInThat->m_fNumber;
			m_eInSpecial = pcInThat->m_eInSpecial;
		}

		CStringA m_strCondition;
		float m_fNumber = -1.0f;
		Condition::Intrinsic m_eInSpecial = Condition::Intrinsic::None;
	};
}

H3DF::ConditionalExpression::ConditionalExpression()
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = std::make_unique<ConditionalExpressionImpl>();
	DEBUG_VALID(m_pcImpl);
}

H3DF::ConditionalExpression::ConditionalExpression(CStringA strInCondition)
{
	m_pcImpl = std::make_unique<ConditionalExpressionImpl>();
	DEBUG_VALID(m_pcImpl);

	auto pcImpl = static_cast<ConditionalExpressionImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_strCondition = strInCondition;
}

H3DF::ConditionalExpression::ConditionalExpression(float fInNumber)
{
	m_pcImpl = std::make_unique<ConditionalExpressionImpl>();
	DEBUG_VALID(m_pcImpl);

	auto pcImpl = static_cast<ConditionalExpressionImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_fNumber = fInNumber;
}

H3DF::ConditionalExpression::ConditionalExpression(Condition::Intrinsic eInSpecial)
{
	m_pcImpl = std::make_unique<ConditionalExpressionImpl>();
	DEBUG_VALID(m_pcImpl);

	auto pcImpl = static_cast<ConditionalExpressionImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_eInSpecial = eInSpecial;
}

H3DF::ConditionalExpression::ConditionalExpression(ConditionalExpression const & cInThat)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = (nullptr == cInThat.m_pcImpl) ? cInThat.m_pcImpl->Clone() : nullptr;
}

ConditionalExpression & H3DF::ConditionalExpression::operator = (ConditionalExpression const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

bool H3DF::ConditionalExpression::ShowCondition(CStringA & strOutCondition) const
{
	auto pcImpl = static_cast<ConditionalExpressionImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	
	if (true == pcImpl->m_strCondition.IsEmpty()) {
		return false;
	}

	strOutCondition = pcImpl->m_strCondition;

	return true;
}

bool H3DF::ConditionalExpression::ShowNumber(float & fOutNumber) const
{
	auto pcImpl = static_cast<ConditionalExpressionImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	if (0.0f > pcImpl->m_fNumber) {
		return false;
	}
	
	fOutNumber = pcImpl->m_fNumber;

	return true;
}

bool H3DF::ConditionalExpression::ShowIntrinsic(Condition::Intrinsic & eOutSpecial) const
{
	auto pcImpl = static_cast<ConditionalExpressionImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	if (Condition::Intrinsic::None == pcImpl->m_eInSpecial) {
		return false;
	}

	eOutSpecial = pcImpl->m_eInSpecial;

	return true;
}

bool H3DF::ConditionalExpression::Equals(ConditionalExpression const & cInThat) const
{
	auto pcImpl = static_cast<ConditionalExpressionImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	auto pcInThatImpl = static_cast<ConditionalExpressionImpl *>(cInThat.m_pcImpl.get());
	DEBUG_VALID(pcInThatImpl);

	if (pcImpl->m_strCondition != pcInThatImpl->m_strCondition) {
		return false;
	}

	if (pcImpl->m_fNumber != pcInThatImpl->m_fNumber) {
		return false;
	}

	if (pcImpl->m_eInSpecial != pcInThatImpl->m_eInSpecial) {
		return false;
	}

	return true;
}

bool H3DF::ConditionalExpression::operator == (ConditionalExpression const & cInThat) const
{
	return Equals(cInThat);
}

bool H3DF::ConditionalExpression::operator != (ConditionalExpression const & cInThat) const
{
	return !Equals(cInThat);
}

ConditionalExpression OR(ConditionalExpression const & cInOperand1, ConditionalExpression const & cInOperand2)
{
	auto pcImpl1 = static_cast<ConditionalExpressionImpl *>((ConditionalExpressionImpl *)cInOperand1.GetImpl());
	DEBUG_VALID(pcImpl1);

	auto pcImpl2 = static_cast<ConditionalExpressionImpl *>((ConditionalExpressionImpl *)cInOperand2.GetImpl());
	DEBUG_VALID(pcImpl1);

	CStringA strCondition;
	strCondition.Format("(%s OR %s)", pcImpl1->m_strCondition, pcImpl2->m_strCondition);

	ConditionalExpression cOut(strCondition);
	return cOut;
}

ConditionalExpression AND(ConditionalExpression const & cInOperand1, ConditionalExpression const & cInOperand2)
{
	auto pcImpl1 = static_cast<ConditionalExpressionImpl *>((ConditionalExpressionImpl *)cInOperand1.GetImpl());
	DEBUG_VALID(pcImpl1);

	auto pcImpl2 = static_cast<ConditionalExpressionImpl *>((ConditionalExpressionImpl *)cInOperand2.GetImpl());
	DEBUG_VALID(pcImpl1);

	CStringA strCondition;
	strCondition.Format("(%s AND %s)", pcImpl1->m_strCondition, pcImpl2->m_strCondition);

	ConditionalExpression cOut(strCondition);
	return cOut;
}

//== ConditionControl class ========================================================================
namespace H3DF
{
	class ConditionControlImpl : public ControlImpl
	{
	public:
		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<ConditionControlImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy(const ConditionControlImpl * pcInThat) {
			ControlImpl::Copy(pcInThat);
		}
	};
}

H3DF::ConditionControl::ConditionControl(SegmentKey & cInSegment)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = std::make_unique<ConditionControlImpl>();
	DEBUG_VALID(m_pcImpl);

	auto pcImpl = static_cast<ConditionControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_cOverrideKey = cInSegment;
}

H3DF::ConditionControl::ConditionControl(ConditionControl const & cInThat)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = (nullptr == cInThat.GetImpl()) ? cInThat.GetImpl()->Clone() : nullptr;
}

ConditionControl & H3DF::ConditionControl::operator = (ConditionControl const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

size_t H3DF::ConditionControl::GetCount() const
{
	auto pcImpl = static_cast<ConditionControlImpl *>(m_pcImpl.get());

	int nCount = 0;
	char chList[MVO_BUFFER_SIZE];
	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		HC_Show_Conditions(chList);
		HC_Show_Conditions_Length(&nCount);
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return 0;
}

// Adds a condition to the collection of active conditions on this segment.
ConditionControl & H3DF::ConditionControl::AddCondition(CStringA strInCondition)
{
	auto pcImpl = static_cast<ConditionControlImpl *>(m_pcImpl.get());

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		//HC_Add_Condition(strInCondition);
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

// Sets a condition as the only active condition on this segment, replacing any existing conditions.
ConditionControl & H3DF::ConditionControl::SetCondition(CStringA strInCondition)
{
	auto pcImpl = static_cast<ConditionControlImpl *>(m_pcImpl.get());

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		HC_Set_Conditions(strInCondition);
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

// Removes a specified condition from the active conditions on this segment.
ConditionControl & H3DF::ConditionControl::UnsetCondition(CStringA strInCondition)
{
	auto pcImpl = static_cast<ConditionControlImpl *>(m_pcImpl.get());

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		HC_UnSet_One_Condition(strInCondition);
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

// Unsets all conditions on this segment.
ConditionControl & H3DF::ConditionControl::UnsetEverything()
{
	auto pcImpl = static_cast<ConditionControlImpl *>(m_pcImpl.get());

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		HC_UnSet_Conditions();
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

// Queries the existence of one condition on this segment.
bool H3DF::ConditionControl::ShowCondition(CStringA & strOutCondition) const
{
	auto pcImpl = static_cast<ConditionControlImpl *>(m_pcImpl.get());

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		HC_Show_Conditions(strOutCondition.GetBuffer());
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return true;
}

// Shows all conditions on this segment.
bool H3DF::ConditionControl::ShowConditions(AStringArray & strOutConditions) const
{
	DEBUG_STOP;
	return true;
}
