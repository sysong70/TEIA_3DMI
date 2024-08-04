#include "StdAfx.h"

#include "Command.h"
#include "Command.EventInfo.h"

using namespace KERNEL;
using namespace H3DF;

//== EventInfo class ===============================================================================
namespace KERNEL
{
	namespace Command
	{
		class EventInfoImpl : public Impl
		{
		public:
			void Copy(EventInfoImpl * pcInThat)
			{
				m_pcWindowKey = pcInThat->m_pcWindowKey;
				m_eEventType = pcInThat->m_eEventType;
				nFlags = pcInThat->nFlags;

				m_cPixelPoint = pcInThat->m_cPixelPoint;
				m_cWindowPoint = pcInThat->m_cWindowPoint;
				m_cWorldPoint = pcInThat->m_cWorldPoint;
			}

			void SetType(EventInfo::Type eInType) { m_eEventType = eInType; }

			H3DF::WindowKey * m_pcWindowKey = nullptr;
			EventInfo::Type m_eEventType = EventInfo::Type::NoEvent;
			UINT nFlags = 0;

			PixelPoint m_cPixelPoint;
			WindowPoint m_cWindowPoint;
			WorldPoint m_cWorldPoint;
		};
	}
}

KERNEL::Command::EventInfo::EventInfo()
{
	m_pcImpl = new EventInfoImpl();
	DEBUG_VALID(m_pcImpl);
}

KERNEL::Command::EventInfo::EventInfo(H3DF::WindowKey & cInWindowKey)
{
	EventInfoImpl * pcImpl = new EventInfoImpl();
	DEBUG_VALID(pcImpl);

	pcImpl->m_pcWindowKey = &cInWindowKey;

	m_pcImpl = pcImpl;
}

KERNEL::Command::EventInfo::EventInfo(EventInfo const & cInThat)
{
	m_pcImpl = new EventInfoImpl();
	DEBUG_VALID(m_pcImpl);
	Set(cInThat);
}

void KERNEL::Command::EventInfo::Set(EventInfo const & cInThat)
{
	EventInfoImpl * pcImpl = (EventInfoImpl *) m_pcImpl;
	EventInfoImpl * pcInThatImpl = (EventInfoImpl *) cInThat.m_pcImpl;

	pcImpl->Copy(pcInThatImpl);
}

KERNEL::Command::EventInfo & KERNEL::Command::EventInfo::operator = (EventInfo const & cInThat)
{
	Set(cInThat);
	return *this;
}

void KERNEL::Command::EventInfo::SetPoint(Command::EventInfo::Type eInType, int x, int y, UINT nInFlags)
{
	EventInfoImpl * pcImpl = (EventInfoImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_eEventType = eInType;
	pcImpl->nFlags = nInFlags;

	pcImpl->m_cPixelPoint.x = x;
	pcImpl->m_cPixelPoint.y = y;
	pcImpl->m_cPixelPoint.z = 0.0f;
	
	pcImpl->m_cWorldPoint = WorldPoint(*pcImpl->m_pcWindowKey, pcImpl->m_cPixelPoint);
	
	pcImpl->m_cWindowPoint = WindowPoint(*pcImpl->m_pcWindowKey, pcImpl->m_cPixelPoint);
	pcImpl->m_cWindowPoint.z = 0.0f;
	pcImpl->m_cWindowPoint.ClampPoint();
}

bool KERNEL::Command::EventInfo::Control() const
{ 
	EventInfoImpl * pcImpl = (EventInfoImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	return 0 != (pcImpl->nFlags & (UINT) EventInfo::Flag::Control);
}

bool KERNEL::Command::EventInfo::Shift() const
{
	EventInfoImpl * pcImpl = (EventInfoImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	return 0 != (pcImpl->nFlags & (UINT) EventInfo::Flag::Shift);
}
bool KERNEL::Command::EventInfo::Alt() const 
{ 
	EventInfoImpl * pcImpl = (EventInfoImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	return 0 != (pcImpl->nFlags & (UINT) EventInfo::Flag::Alt);
}

bool KERNEL::Command::EventInfo::LButton() const 
{ 
	EventInfoImpl * pcImpl = (EventInfoImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	return 0 != (pcImpl->nFlags & (UINT)EventInfo::Flag::LeftButton);
}

bool KERNEL::Command::EventInfo::MButton() const 
{
	EventInfoImpl * pcImpl = (EventInfoImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	return 0 != (pcImpl->nFlags & (UINT) EventInfo::Flag::MiddleButton);
}

bool KERNEL::Command::EventInfo::RButton() const 
{
	EventInfoImpl * pcImpl = (EventInfoImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	return 0 != (pcImpl->nFlags & (UINT) EventInfo::Flag::RightButton);
}

PixelPoint const & KERNEL::Command::EventInfo::GetMousePixelPoint() const 
{ 
	EventInfoImpl * pcImpl = (EventInfoImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->m_cPixelPoint;
}

WindowPoint const & KERNEL::Command::EventInfo::GetMouseWindowPoint() const
{
	EventInfoImpl * pcImpl = (EventInfoImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->m_cWindowPoint; 
}

WorldPoint const & KERNEL::Command::EventInfo::GetMouseWorldPoint() const
{
	EventInfoImpl * pcImpl = (EventInfoImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->m_cWorldPoint; 
}

void KERNEL::Command::EventInfo::SetEventType(EventInfo::Type cInType)
{
	EventInfoImpl * pcImpl = (EventInfoImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_eEventType = cInType;
}

KERNEL::Command::EventInfo::Type KERNEL::Command::EventInfo::GetEventType() const
{
	EventInfoImpl * pcImpl = (EventInfoImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->m_eEventType;
}
