#include "StdAfx.h"

#include "Command.StepImpl.h"

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
	m_pfDrawFunction = pcInThat->m_pfDrawFunction;
}
