#include "StdAfx.h"

#include "Command.SetImpl.h"

#include "../Command.Step.h"

#include "../Kernel.Session.h"
#include "Kernel.SessionImpl.h"

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


