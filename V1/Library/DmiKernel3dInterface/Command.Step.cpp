#include "StdAfx.h"

#include "Command.Step.h"

#include "Impl/Command.StepImpl.h"

using namespace KERNEL;

//== Step Class ====================================================================================
KERNEL::Command::Step::Step()
{
  	m_pcImpl = new StepImpl();
  	DEBUG_VALID(m_pcImpl);
}

KERNEL::Command::Step::InputType KERNEL::Command::Step::GetInputType() const
{
	StepImpl * pcImpl = (StepImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->m_eInputType;
}

void KERNEL::Command::Step::SetInputType(Step::InputType eInInputType)
{
	StepImpl * pcImpl = (StepImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_eInputType = eInInputType;
}

void KERNEL::Command::Step::SetMessage(CString strInMessage)
{
	StepImpl * pcImpl = (StepImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_strMessage = strInMessage;
}

void KERNEL::Command::Step::SetInformation(CString strInInformation)
{
	StepImpl * pcImpl = (StepImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_strInformation = strInInformation;
}

void KERNEL::Command::Step::SetDrawFunction(void (*pfInDrawFunction)(Step * pcInStep))
{
	StepImpl * pcImpl = (StepImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_pfDrawFunction = pfInDrawFunction;
}

void KERNEL::Command::Step::Draw()
{
	StepImpl * pcImpl = (StepImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
}

