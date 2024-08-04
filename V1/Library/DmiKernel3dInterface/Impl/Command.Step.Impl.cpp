#include "StdAfx.h"

#include "Command.Step.Impl.h"

using namespace KERNEL;
using namespace KERNEL::Command;

void KERNEL::Command::StepImpl::Copy(StepImpl * pcInThat) 
{
	m_eInputType = pcInThat->m_eInputType;
	m_strText = pcInThat->m_strText;
	m_cPoint = pcInThat->m_cPoint;
	m_fValue = pcInThat->m_fValue;
	
	m_strMessage = pcInThat->m_strMessage;
	m_strInformation = pcInThat->m_strInformation;

	m_pvcSavedEvents = pcInThat->m_pvcSavedEvents;
	m_pcEvent = pcInThat->m_pcEvent;

	m_pfDrawFunction = pcInThat->m_pfDrawFunction;
}
