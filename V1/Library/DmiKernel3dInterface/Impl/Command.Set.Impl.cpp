#include "StdAfx.h"

#include "Command.Set.Impl.h"

#include "../Command.Step.h"

#include "../Kernel.Session.h"
#include "Kernel.Session.Impl.h"

#include <3DF/Visibility.h>

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
bool KERNEL::Command::SetImpl::EventExecution(Command::Event & cInEvent)
{
	if (true == m_deStep.empty()) {
		return false;
	}

	// Iterator가 유효하지 않은 경우 Iterator를 초기화한다.
	if (false == m_cStepIterator.IsValid()) {
		SetIteratorImpl * pcIteratorImpl = (SetIteratorImpl *) m_cStepIterator.GetImpl();
		DEBUG_VALID(pcIteratorImpl);

		pcIteratorImpl->pcBeginIterator = m_deStep.begin();
		pcIteratorImpl->pcEndIterator = m_deStep.end();
		pcIteratorImpl->pcIterator = pcIteratorImpl->pcBeginIterator;
	}

	// Step을 하나씩 실행한다. 지금 단계의 Step을 가져와 실행하도록 한다.
	Command::Step * pcStep = m_cStepIterator.GetStep();
	DEBUG_VALID(pcStep);

	StepExecution(pcStep, cInEvent);

	return true;
}

// 2. Step 실행
bool KERNEL::Command::SetImpl::StepExecution(Command::Step * pcInStep, Command::Event & cInEvent)
{
	// 저장되어 있는 Step이 유효한지 확인
	if (true == m_deStep.empty()) {
		return false;
	}

	DEBUG_VALID(m_pcSession);

	// Event의 정보가 유효한지를 검사한다.
	IsValidEventInfo(cInEvent);

	// 현제 저장되어 있는 Event와 현재 Event를 전달한다.
	pcInStep->SetEventInfo(m_vcEventInfos, cInEvent);

	// Step에서 요구하는 Event인지 확인 필요
	Step::EventType eEventType = CheckEvent(pcInStep, cInEvent);

	// 주어진 Event가 현재 Step을 완료하지 않고 준비 단계를 나타냄. Moouse Move등의 Event를 나타냄.
	if (Step::EventType::PreProcessing == eEventType) {

		H3DF::SegmentKey cConstructionSegment = m_pcSession->Canvas().GetFrontView().GetConstructionKey();
		cConstructionSegment.Flush(H3DF::Search::Type::Geometry);
		pcInStep->Draw(cConstructionSegment);

		return true;
	}
	// Event가 완료된 후 단계임.
	else if (Step::EventType::Complete == eEventType) {
		// 전달받은 Event를 저장한다.
		m_vcEventInfos.push_back(cInEvent);
	}

	m_cStepIterator.Next();

	return true;
}

// 2-1. Step에서 요청한 Event인지 확인한다.
Step::EventType KERNEL::Command::SetImpl::CheckEvent(Command::Step * pcInStep, Command::Event & cInEvent)
{
	Step::EventType eStepEventType = Step::EventType::None;

	// 입력된 Step Command에서 InputType을 확인해서 각 Type별로 처리한다.
	Step::InputType eInputType = pcInStep->GetInputType();
	if (Step::InputType::Coordinate == eInputType) {
		// LButtonUp이면 그 좌표를 이용해서 다음 처리를 한다. 현재 Step은 완료한 것으로 본다.
		if (H3DF::Operator::Event::Type::LButtonUp == cInEvent.GetEventType()) {
			eStepEventType = Step::EventType::Complete;
		}
		// MouseMove이면 그 좌표를 이용해서 현재 Step의 처리를 한다. 현재 Step은 완료하지 않은 것으로 본다.
		// 완료되기전 Process를 진행하도록 한다.
		else if (H3DF::Operator::Event::Type::MouseMove == cInEvent.GetEventType()) {
			eStepEventType = Step::EventType::PreProcessing;
		}
	}

	return eStepEventType;
}

// 3. Event의 정보가 유효한지를 검사한다.
bool KERNEL::Command::SetImpl::IsValidEventInfo(Command::Event & cInEvent)
{
	return true;
}
