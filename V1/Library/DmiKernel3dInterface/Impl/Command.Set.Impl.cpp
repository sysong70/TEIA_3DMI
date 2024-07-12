#include "StdAfx.h"

#include "Command.Set.Impl.h"

#include "../Command.Step.h"

#include "../Kernel.Session.h"
#include "Kernel.Session.Impl.h"

using namespace KERNEL;
using namespace KERNEL::Command;

KERNEL::Command::SetImpl::SetImpl(const KERNEL::Session * pcInSession)
{
	DEBUG_VALID(pcInSession);

	m_eType = KERNEL::Type::CommandSet;
	m_pcSession = pcInSession;
}

void KERNEL::Command::SetImpl::Copy(SetImpl * pcInThat)
{
	m_eType = pcInThat->m_eType;
	m_pcSession = pcInThat->m_pcSession;

	m_deStep.clear();
	for (auto cItem : pcInThat->m_deStep) {
		m_deStep.push_back(cItem);
	}

	m_vstrTexts.clear();
	for (auto & cItem : pcInThat->m_vstrTexts) {
		m_vstrTexts.push_back(cItem);
	}

	m_vcPoints.clear();
	for (auto & cItem : pcInThat->m_vcPoints) {
		m_vcPoints.push_back(cItem);
	}

	m_vfValues.clear();
	for (auto & cItem : pcInThat->m_vfValues) {
		m_vfValues.push_back(cItem);
	}
}

H3DF::WindowKey & KERNEL::Command::SetImpl::Window()
{
	DEBUG_VALID(m_pcSession);

	SessionImpl * pcImpl = dynamic_cast<SessionImpl *>((SessionImpl *)m_pcSession->GetImpl());
	DEBUG_VALID(pcImpl);

	return pcImpl->GetCanvas().GetFrontView().GetWindowKey();
}

const H3DF::WindowKey & KERNEL::Command::SetImpl::Window() const
{
	DEBUG_VALID(m_pcSession);

	SessionImpl * pcImpl = dynamic_cast<SessionImpl *>((SessionImpl *)m_pcSession->GetImpl());
	DEBUG_VALID(pcImpl);

	return pcImpl->GetCanvas().GetFrontView().GetWindowKey();
}

H3DF::View & KERNEL::Command::SetImpl::View()
{
	DEBUG_VALID(m_pcSession);

	SessionImpl * pcImpl = dynamic_cast<SessionImpl *>((SessionImpl *)m_pcSession->GetImpl());
	DEBUG_VALID(pcImpl);

	return pcImpl->GetCanvas().GetFrontView();
}

const H3DF::View & KERNEL::Command::SetImpl::View() const
{
	DEBUG_VALID(m_pcSession);

	SessionImpl * pcImpl = dynamic_cast<SessionImpl *>((SessionImpl *)m_pcSession->GetImpl());
	DEBUG_VALID(pcImpl);

	return pcImpl->GetCanvas().GetFrontView();
}

Signal::Delivery & KERNEL::Command::SetImpl::Delivery()
{
	DEBUG_VALID(m_pcSession);

	SessionImpl * pcImpl = dynamic_cast<SessionImpl *>((SessionImpl *)m_pcSession->GetImpl());
	DEBUG_VALID(pcImpl);

	return pcImpl->Delivery();
}

const Signal::Delivery & KERNEL::Command::SetImpl::Delivery() const
{
	DEBUG_VALID(m_pcSession);

	SessionImpl * pcImpl = dynamic_cast<SessionImpl *>((SessionImpl *)m_pcSession->GetImpl());
	DEBUG_VALID(pcImpl);

	return pcImpl->Delivery();
}

Session & KERNEL::Command::SetImpl::GetSession()
{
	DEBUG_VALID(m_pcSession);
	return *((KERNEL::Session *)m_pcSession);
}

const Session & KERNEL::Command::SetImpl::GetSession() const
{
	DEBUG_VALID(m_pcSession);
	return *m_pcSession;
}

H3DF::Model & KERNEL::Command::SetImpl::GetModel()
{
	DEBUG_VALID(m_pcSession);

	SessionImpl * pcDocImpl = (SessionImpl *)GetSession().GetImpl();
	DEBUG_VALID(pcDocImpl);

	return pcDocImpl->GetModel();
}

const H3DF::Model & KERNEL::Command::SetImpl::GetModel() const
{
	DEBUG_VALID(m_pcSession);

	SessionImpl * pcDocImpl = (SessionImpl *)GetSession().GetImpl();
	DEBUG_VALID(pcDocImpl);

	return pcDocImpl->GetModel();
}

// Update하기전에 Hightlight된 것들을 모두 Unhighlight하고, SnapItem을 모두 Reset한다.
void KERNEL::Command::SetImpl::PrepareUpdate()
{
	GetSession().Canvas().GetFrontView().SuppressUpdate(true);

	SessionImpl * pcDocViewImpl = dynamic_cast<SessionImpl *>(GetSession().GetImpl());
	pcDocViewImpl->Select().UnhighlightEverything();
	pcDocViewImpl->Select().ResetSnapItems(false);
}

// Update가 완료되면, View를 Update한다.
void KERNEL::Command::SetImpl::Updated()
{
	GetSession().Canvas().GetFrontView().SuppressUpdate(false);
	GetSession().Canvas().GetFrontView().Update();
}

//== Event 관련 함수 =================================================================================

// 1.입력 Event 처리
bool KERNEL::Command::SetImpl::EventExecution(Command::EventInfo & cInEvent)
{
	if (true == m_deStep.empty()) {
		return false;
	}

	// Iterator가 유효하지 않으면 처리하지 않는다.
	if (false == m_cIterator.IsValid()) {
		SetIteratorImpl * pcIteratorImpl = (SetIteratorImpl *) m_cIterator.GetImpl();
		DEBUG_VALID(pcIteratorImpl);

		pcIteratorImpl->pcBeginIterator = m_deStep.begin();
		pcIteratorImpl->pcEndIterator = m_deStep.end();
		pcIteratorImpl->pcIterator = pcIteratorImpl->pcBeginIterator;
	}

	// Step을 하나씩 실행한다. 지금 단계의 Step을 가져와 실행하도록 한다.
	Command::Step * pcStep = m_cIterator.GetStep();
	DEBUG_VALID(pcStep);

	StepExecution(pcStep, cInEvent);

	m_cIterator.Next();

	return true;
}

// 2. Step 실행
bool KERNEL::Command::SetImpl::StepExecution(Command::Step * pcInStep, Command::EventInfo & cInEvent)
{
	if (true == m_deStep.empty()) {
		return false;
	}

	// EventInfo의 정보가 유효한지를 검사한다.
	IsValidEventInfo(cInEvent);

	// 전달받은 Event를 저장한다.
	m_vcEventInfos.push_back(cInEvent);

	//pcStep->SetEventInfo(m_vcEventInfos);

	// Step에서 요구하는 Event인지 확인 필요
	if (false == CheckEvent(pcInStep, cInEvent)) {
		return false;
	}

	// Step에서 필요한 화면을 그리도록 한다.
	pcInStep->Draw();

	m_cIterator.Next();

	return true;
}

// 2-1. Step에서 요청한 Event인지 확인한다.
bool KERNEL::Command::SetImpl::CheckEvent(Command::Step * pcInStep, Command::EventInfo & cInEvent)
{
	return true;
}

// 3. EventInfo의 정보가 유효한지를 검사한다.
bool KERNEL::Command::SetImpl::IsValidEventInfo(Command::EventInfo & cInEvent)
{
	return true;
}
