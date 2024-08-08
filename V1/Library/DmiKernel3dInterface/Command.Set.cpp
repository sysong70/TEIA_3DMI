#include "StdAfx.h"

#include "Command.Set.h"
#include "Impl/Command.Set.Impl.h"

#include "Command.Step.h"

#include <deque>

using namespace KERNEL;
using namespace KERNEL::Command;

KERNEL::Command::SetIterator::SetIterator()
{
	m_pcImpl = new SetIteratorImpl();
}

KERNEL::Command::SetIterator::SetIterator(SetIterator const & cInThat)
{
	m_pcImpl = new SetIteratorImpl();
	Set(cInThat);
}

void KERNEL::Command::SetIterator::Set(SetIterator const & cInThat)
{
	SetIteratorImpl * pcImpl = (SetIteratorImpl *)m_pcImpl;
	SetIteratorImpl * pcInThatImpl = (SetIteratorImpl *)cInThat.m_pcImpl;

	pcImpl->Copy(pcInThatImpl);
}

KERNEL::Command::SetIterator & KERNEL::Command::SetIterator::operator=(SetIterator const & cInThat)
{
	Set(cInThat);
	return *this;
}

void KERNEL::Command::SetIterator::Next()
{
	SetIteratorImpl * pcImpl = (SetIteratorImpl *)m_pcImpl;
	++pcImpl->pcIterator;
}

KERNEL::Command::SetIterator & KERNEL::Command::SetIterator::operator++()
{
	Next();
	return *this;
}

KERNEL::Command::SetIterator & KERNEL::Command::SetIterator::operator++(int nInVal)
{
	SetIteratorImpl * pcImpl = (SetIteratorImpl *)m_pcImpl;
	std::advance(pcImpl->pcIterator, nInVal);
	return *this;
}

bool KERNEL::Command::SetIterator::operator == (SetIterator const & cInSearchResultsIterator)
{
	SetIteratorImpl * pcImpl = (SetIteratorImpl *)m_pcImpl;
	SetIteratorImpl * pcInThatImpl = (SetIteratorImpl *)cInSearchResultsIterator.m_pcImpl;
	if (pcImpl->pcIterator != pcInThatImpl->pcIterator) {
		return false;
	}

	if (pcImpl->pcBeginIterator != pcInThatImpl->pcBeginIterator) {
		return false;
	}

	if (pcImpl->pcEndIterator != pcInThatImpl->pcEndIterator) {
		return false;
	}

	return true;
}

bool KERNEL::Command::SetIterator::operator != (SetIterator const & cInSearchResultsIterator)
{
	return !(*this == cInSearchResultsIterator);
}

// End까지 도달했는지를 확인.
bool KERNEL::Command::SetIterator::IsValid() const
{
	SetIteratorImpl * pcImpl = (SetIteratorImpl *)m_pcImpl;
	return pcImpl->pcIterator != pcImpl->pcEndIterator;
}

void KERNEL::Command::SetIterator::Reset()
{
	SetIteratorImpl * pcImpl = (SetIteratorImpl *)m_pcImpl;
	pcImpl->pcIterator = pcImpl->pcBeginIterator;
}

KERNEL::Command::Step * KERNEL::Command::SetIterator::GetStep() const
{
	SetIteratorImpl * pcImpl = (SetIteratorImpl *)m_pcImpl;

	return *pcImpl->pcIterator;
}

KERNEL::Command::Step * KERNEL::Command::SetIterator::operator * () const
{
	return GetStep();
}

//== Set Class =====================================================================================
KERNEL::Command::Set::Set(const Session * pcInSession)
{
	m_pcImpl = new SetImpl(pcInSession);
	DEBUG_VALID(m_pcImpl);
}

KERNEL::Command::Set::~Set()
{
	Clear();
}

// Set에 포함되어 있는 값 Iterator, Event 등을 초기화.
void KERNEL::Command::Set::Reset()
{
	if (nullptr == m_pcImpl) {
		DEBUG_STOP;
		return;
	}

	SetImpl * pcImpl = (SetImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_cStepIterator = GetIterator();

	pcImpl->m_vcEventInfos.clear();
}

void KERNEL::Command::Set::Reset() const
{
	if (nullptr == m_pcImpl) {
		DEBUG_STOP;
		return;
	}

	SetImpl * pcImpl = (SetImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_cStepIterator = GetIterator();

	pcImpl->m_vcEventInfos.clear();
}

// Step을 모두 삭제한다.
void KERNEL::Command::Set::Clear()
{
	if (nullptr == m_pcImpl) {
		DEBUG_STOP;
		return;
	}

	SetImpl * pcImpl = (SetImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	for (auto cItem : pcImpl->m_deStep) {
		delete cItem;
	}

	pcImpl->m_deStep.clear();
}

void KERNEL::Command::Set::Clear() const
{
	if (nullptr == m_pcImpl) {
		DEBUG_STOP;
		return;
	}

	SetImpl * pcImpl = (SetImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	for (auto cItem : pcImpl->m_deStep) {
		delete cItem;
	}

	pcImpl->m_deStep.clear();
}

size_t KERNEL::Command::Set::GetCount() const
{
	if (nullptr == m_pcImpl) {
		DEBUG_STOP;
		return 0;
	}

	SetImpl * pcImpl = (SetImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return  pcImpl->m_deStep.size();
}

KERNEL::Command::SetIterator KERNEL::Command::Set::GetIterator() const
{
	SetImpl * pcImpl = (SetImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	if (false == pcImpl->m_cStepIterator.IsValid()) {

		SetIteratorImpl * pcIteratorImpl = (SetIteratorImpl *) pcImpl->m_cStepIterator.GetImpl();
		DEBUG_VALID(pcIteratorImpl);

		pcIteratorImpl->pcBeginIterator = pcImpl->m_deStep.begin();
		pcIteratorImpl->pcEndIterator = pcImpl->m_deStep.end();
		pcIteratorImpl->pcIterator = pcIteratorImpl->pcBeginIterator;
	}

	return pcImpl->m_cStepIterator;
}

KERNEL::Command::Step * KERNEL::Command::Set::Front()
{
	SetImpl * pcImpl = (SetImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->m_deStep.front();
}

KERNEL::Command::Step * KERNEL::Command::Set::Front() const
{
	SetImpl * pcImpl = (SetImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->m_deStep.front();
}

void KERNEL::Command::Set::PushFront(Step * pcInStep)
{
	if (nullptr == m_pcImpl) {
		DEBUG_STOP;
		return;
	}

	SetImpl * pcImpl = (SetImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_deStep.push_front(pcInStep);
}

void KERNEL::Command::Set::PushBack(Step * pcInStep)
{
	if (nullptr == m_pcImpl) {
		DEBUG_STOP;
		return;
	}

	SetImpl * pcImpl = (SetImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_deStep.push_back(pcInStep);
}

//== 입력된 사용자 명령어 처리 =========================================================================
bool KERNEL::Command::Set::EventExecution(Command::Event & cInEvent)
{
	if (nullptr == m_pcImpl) {
		DEBUG_STOP;
		return false;
	}
	// Command Step이 없으면 처리하지 않는다.
	if (0 == GetCount()) {
		return false;
	}

	Command::SetImpl * pcImpl = (Command::SetImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->EventExecution(cInEvent);

	return true;
}

KERNEL::Command::Type KERNEL::Command::Set::GetType() const
{
	return Type::None;
}