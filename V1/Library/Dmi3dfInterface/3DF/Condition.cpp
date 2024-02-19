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

namespace H3DF
{ 
	class ConditionalExpressionImpl : public Impl
	{
	public:
		ConditionalExpressionImpl() { m_eType = H3DF::Type::ConditionalExpression; }

		void Copy(ConditionalExpressionImpl * pcInThat) {
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
	m_pcImpl = new ConditionalExpressionImpl();
	DEBUG_VALID(m_pcImpl);
}

H3DF::ConditionalExpression::ConditionalExpression(CStringA strInCondition)
{
	ConditionalExpressionImpl * pcImpl = new ConditionalExpressionImpl();
	DEBUG_VALID(pcImpl);
	pcImpl->m_strCondition = strInCondition;

	m_pcImpl = pcImpl;
}

H3DF::ConditionalExpression::ConditionalExpression(float fInNumber)
{
	ConditionalExpressionImpl * pcImpl = new ConditionalExpressionImpl();
	DEBUG_VALID(pcImpl);
	pcImpl->m_fNumber = fInNumber;

	m_pcImpl = pcImpl;
}

H3DF::ConditionalExpression::ConditionalExpression(Condition::Intrinsic eInSpecial)
{
	ConditionalExpressionImpl * pcImpl = new ConditionalExpressionImpl();
	DEBUG_VALID(pcImpl);
	pcImpl->m_eInSpecial = eInSpecial;

	m_pcImpl = pcImpl;
}

H3DF::ConditionalExpression::ConditionalExpression(ConditionalExpression const & cInThat)
{
	m_pcImpl = new ConditionalExpressionImpl();
	DEBUG_VALID(m_pcImpl);
	Set(cInThat);
}

void H3DF::ConditionalExpression::Set(ConditionalExpression const & cInThat)
{
	ConditionalExpressionImpl * pcImpl = dynamic_cast<ConditionalExpressionImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);
	ConditionalExpressionImpl * pcInThatImpl = dynamic_cast<ConditionalExpressionImpl *>(cInThat.m_pcImpl);
	DEBUG_VALID(pcInThatImpl);

	pcImpl->Copy(pcInThatImpl);
}

ConditionalExpression & H3DF::ConditionalExpression::operator = (ConditionalExpression const & cInThat)
{
	Set(cInThat);
	return *this;
}

bool H3DF::ConditionalExpression::ShowCondition(CStringA & strOutCondition) const
{
	ConditionalExpressionImpl * pcImpl = dynamic_cast<ConditionalExpressionImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);
	
	if (true == pcImpl->m_strCondition.IsEmpty()) {
		return false;
	}

	strOutCondition = pcImpl->m_strCondition;

	return true;
}

bool H3DF::ConditionalExpression::ShowNumber(float & fOutNumber) const
{
	ConditionalExpressionImpl * pcImpl = dynamic_cast<ConditionalExpressionImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	if (0.0f > pcImpl->m_fNumber) {
		return false;
	}
	
	fOutNumber = pcImpl->m_fNumber;

	return true;
}

bool H3DF::ConditionalExpression::ShowIntrinsic(Condition::Intrinsic & eOutSpecial) const
{
	ConditionalExpressionImpl * pcImpl = dynamic_cast<ConditionalExpressionImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	if (Condition::Intrinsic::None == pcImpl->m_eInSpecial) {
		return false;
	}

	eOutSpecial = pcImpl->m_eInSpecial;

	return true;
}

bool H3DF::ConditionalExpression::Equals(ConditionalExpression const & cInThat) const
{
	ConditionalExpressionImpl * pcImpl = dynamic_cast<ConditionalExpressionImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);
	ConditionalExpressionImpl * pcInThatImpl = dynamic_cast<ConditionalExpressionImpl *>(cInThat.m_pcImpl);
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
	ConditionalExpressionImpl * pcImpl1 = dynamic_cast<ConditionalExpressionImpl *>((ConditionalExpressionImpl *)cInOperand1.GetImpl());
	DEBUG_VALID(pcImpl1);

	ConditionalExpressionImpl * pcImpl2 = dynamic_cast<ConditionalExpressionImpl *>((ConditionalExpressionImpl *)cInOperand2.GetImpl());
	DEBUG_VALID(pcImpl1);

	CStringA strCondition;
	strCondition.Format("(%s OR %s)", pcImpl1->m_strCondition, pcImpl2->m_strCondition);

	ConditionalExpression cOut(strCondition);
	return cOut;
}

ConditionalExpression AND(ConditionalExpression const & cInOperand1, ConditionalExpression const & cInOperand2)
{
	ConditionalExpressionImpl * pcImpl1 = dynamic_cast<ConditionalExpressionImpl *>((ConditionalExpressionImpl *)cInOperand1.GetImpl());
	DEBUG_VALID(pcImpl1);

	ConditionalExpressionImpl * pcImpl2 = dynamic_cast<ConditionalExpressionImpl *>((ConditionalExpressionImpl *)cInOperand2.GetImpl());
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
		ConditionControlImpl() { m_eType = H3DF::Type::ConditionControl; }

		void Copy(ConditionControlImpl * pcInThat) {
			ControlImpl::Copy(pcInThat);
		}
	};
}

H3DF::ConditionControl::ConditionControl(SegmentKey & cInSegmentKey)
{
	ConditionControlImpl * pcImpl = new ConditionControlImpl();
	pcImpl->m_cOverrideKey = cInSegmentKey;

	m_pcImpl = pcImpl;
}

H3DF::ConditionControl::ConditionControl(ConditionControl const & cInThat)
{
	m_pcImpl = new ConditionControlImpl();
	Set(cInThat);
}

void H3DF::ConditionControl::Set(ConditionControl const & cInThat)
{
	ConditionControlImpl * pcImpl = (ConditionControlImpl *) m_pcImpl;
	ConditionControlImpl * pcInThatImpl = (ConditionControlImpl *) cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

ConditionControl & H3DF::ConditionControl::operator = (ConditionControl const & cInThat)
{
	Set(cInThat);
	return *this;
}

size_t H3DF::ConditionControl::GetCount() const
{
	ConditionControlImpl * pcImpl = (ConditionControlImpl *) m_pcImpl;

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
	ConditionControlImpl * pcImpl = (ConditionControlImpl *) m_pcImpl;

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		//HC_Add_Condition(strInCondition);
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

// Sets a condition as the only active condition on this segment, replacing any existing conditions.
ConditionControl & H3DF::ConditionControl::SetCondition(CStringA strInCondition)
{
	ConditionControlImpl * pcImpl = (ConditionControlImpl *) m_pcImpl;

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		HC_Set_Conditions(strInCondition);
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

// Removes a specified condition from the active conditions on this segment.
ConditionControl & H3DF::ConditionControl::UnsetCondition(CStringA strInCondition)
{
	ConditionControlImpl * pcImpl = (ConditionControlImpl *) m_pcImpl;

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		HC_UnSet_One_Condition(strInCondition);
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

// Unsets all conditions on this segment.
ConditionControl & H3DF::ConditionControl::UnsetEverything()
{
	ConditionControlImpl * pcImpl = (ConditionControlImpl *) m_pcImpl;

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		HC_UnSet_Conditions();
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

// Queries the existence of one condition on this segment.
bool H3DF::ConditionControl::ShowCondition(CStringA & strOutCondition) const
{
	ConditionControlImpl * pcImpl = (ConditionControlImpl *) m_pcImpl;

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
