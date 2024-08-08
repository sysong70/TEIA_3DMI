#include "StdAfx.h"

#include "Command.h"
#include "Command.Event.h"

using namespace KERNEL;
using namespace H3DF;

//== Event class ===============================================================================
namespace KERNEL
{
	namespace Command
	{
		class EventImpl : public Impl
		{
		public:
			void Copy(EventImpl * pcInThat)
			{
				m_pcWindowKey = pcInThat->m_pcWindowKey;
				m_cOperatorEvent = pcInThat->m_cOperatorEvent;
			}

			H3DF::Operator::Event m_cOperatorEvent;
			H3DF::WindowKey * m_pcWindowKey = nullptr;
		};
	}
}

KERNEL::Command::Event::Event()
{
	m_pcImpl = new EventImpl();
	DEBUG_VALID(m_pcImpl);
}

KERNEL::Command::Event::Event(H3DF::WindowKey & cInWindowKey)
{
	EventImpl * pcImpl = new EventImpl();
	DEBUG_VALID(pcImpl);

	pcImpl->m_pcWindowKey = &cInWindowKey;
	pcImpl->m_cOperatorEvent.SetWindow(cInWindowKey);

	m_pcImpl = pcImpl;
}

KERNEL::Command::Event::Event(Event const & cInThat)
{
	m_pcImpl = new EventImpl();
	DEBUG_VALID(m_pcImpl);

	Set(cInThat);
}

void KERNEL::Command::Event::Set(Event const & cInThat)
{
	EventImpl * pcImpl = (EventImpl *) m_pcImpl;
	EventImpl * pcInThatImpl = (EventImpl *) cInThat.m_pcImpl;

	pcImpl->Copy(pcInThatImpl);
}

KERNEL::Command::Event & KERNEL::Command::Event::operator = (Event const & cInThat)
{
	Set(cInThat);
	return *this;
}

H3DF::Operator::Event & KERNEL::Command::Event::OperatorEvent()
{
	EventImpl * pcImpl = (EventImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->m_cOperatorEvent;
}

void KERNEL::Command::Event::SetPoint(H3DF::Operator::Event::Type eInType, int x, int y, UINT nInFlags)
{
	EventImpl * pcImpl = (EventImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_cOperatorEvent.SetPoint(eInType, x, y, nInFlags);
}

void KERNEL::Command::Event::SetMouseWheelDelta(int nInDelta)
{
	EventImpl * pcImpl = (EventImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_cOperatorEvent.SetMouseWheelDelta(nInDelta);
}

bool KERNEL::Command::Event::Control() const
{ 
	EventImpl * pcImpl = (EventImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->m_cOperatorEvent.Control();
}

bool KERNEL::Command::Event::Shift() const
{
	EventImpl * pcImpl = (EventImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->m_cOperatorEvent.Shift();
}

bool KERNEL::Command::Event::Alt() const 
{ 
	EventImpl * pcImpl = (EventImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->m_cOperatorEvent.Alt();
}

bool KERNEL::Command::Event::LButton() const 
{ 
	EventImpl * pcImpl = (EventImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->m_cOperatorEvent.LButton();
}

bool KERNEL::Command::Event::MButton() const 
{
	EventImpl * pcImpl = (EventImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->m_cOperatorEvent.MButton();
}

bool KERNEL::Command::Event::RButton() const 
{
	EventImpl * pcImpl = (EventImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->m_cOperatorEvent.RButton();
}

PixelPoint const & KERNEL::Command::Event::GetMousePixelPoint() const 
{ 
	EventImpl * pcImpl = (EventImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->m_cOperatorEvent.GetMousePixelPoint();
}

WindowPoint const & KERNEL::Command::Event::GetMouseWindowPoint() const
{
	EventImpl * pcImpl = (EventImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->m_cOperatorEvent.GetMouseWindowPoint();
}

WorldPoint const & KERNEL::Command::Event::GetMouseWorldPoint() const
{
	EventImpl * pcImpl = (EventImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->m_cOperatorEvent.GetMouseWorldPoint();
}

void KERNEL::Command::Event::SetEventType(H3DF::Operator::Event::Type cInType)
{
	EventImpl * pcImpl = (EventImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_cOperatorEvent.SetEventType(cInType);
}

H3DF::Operator::Event::Type KERNEL::Command::Event::GetEventType() const
{
	EventImpl * pcImpl = (EventImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->m_cOperatorEvent.GetEventType();
}