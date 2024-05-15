#include "StdAfx.h"

#include "Command.InputManager.h"

#include "Impl/Command.InputManagerImpl.h"

using namespace KERNEL;
using namespace KERNEL::Command;


//== InputKit Class ===================================================================================
KERNEL::Command::InputKit::InputKit()
{
	m_pcImpl = new InputKitImpl();
	DEBUG_VALID(m_pcImpl);
}

KERNEL::Command::InputKit::InputKit(InputKit const & cInThat)
{
	m_pcImpl = new InputKitImpl();
	DEBUG_VALID(m_pcImpl);

	Set(cInThat);
}

void KERNEL::Command::InputKit::Set(InputKit const & cInThat)
{
	InputKitImpl * pcImpl = (InputKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	InputKitImpl * pcInThatImpl = (InputKitImpl *)cInThat.m_pcImpl;
	DEBUG_VALID(pcInThatImpl);

	pcImpl->Copy(pcInThatImpl);
}

InputKit & KERNEL::Command::InputKit::operator = (InputKit const & cInThat)
{
	Set(cInThat);
	return *this;
}


size_t KERNEL::Command::InputKit::CoordinateCount() const
{
	InputKitImpl * pcImpl = (InputKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->m_aPoints.size();
}

H3DF::Point KERNEL::Command::InputKit::Coordinate(size_t nIndex)
{
	InputKitImpl * pcImpl = (InputKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->m_aPoints[nIndex];
}

size_t KERNEL::Command::InputKit::StringCount() const
{
	InputKitImpl * pcImpl = (InputKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->m_aStrings.size();
}

CString KERNEL::Command::InputKit::String(size_t nIndex)
{
	InputKitImpl * pcImpl = (InputKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->m_aStrings[nIndex];
}
