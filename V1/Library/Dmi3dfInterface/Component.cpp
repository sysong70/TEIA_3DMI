#include "StdAfx.h"

#include "Component.h"

#include "Common_Define.h"

#include "3DF/Segment.h"
#include "3DF/3DF.Utility.h"
#include "3DF/KeyPath.h"
#include "3DF/Selection.h"
#include "3DF/Impl/SelectionImpl.h"

#include <ranges>

using namespace H3DF;

//== ComponentImpl Class ===========================================================================

namespace H3DF
{
	class ComponentImpl : public Impl
	{
	public:
		enum API_3DF ComponentStatus
		{
			Normal			= 0x0001,
			End				= 0x0002,
			UiUpdate		= 0x0004,
			Invisible		= 0x0008,		// Tree에 나타나면 않되는 요소
			Hide			= 0x0010,		// 원래 Hide된 경우
			NoShow			= 0x0020,		// NoShow된 경우
		};

		void Copy(ComponentImpl * pcInThat);

		HC_KEY m_nKey = INVALID_KEY;
		Component::ComponentType m_eType = Component::ComponentType::None;
		DWORD m_nStatus = ComponentStatus::Normal;

		Component * m_pcParent = nullptr;
		std::vector<Component *> m_vpnChildren;
	};
}

void H3DF::ComponentImpl::Copy(ComponentImpl * pcInThat)
{
	m_nKey = pcInThat->m_nKey;
	m_nStatus = pcInThat->m_nStatus;
	m_pcParent = pcInThat->m_pcParent;
	m_vpnChildren = pcInThat->m_vpnChildren;
}

//== Component Class ===============================================================================
H3DF::Component::Component()
{
	m_pcImpl = new ComponentImpl();
	DEBUG_VALID(m_pcImpl);
}

H3DF::Component::Component(Component const & cInThat)
{
	m_pcImpl = new ComponentImpl();
	DEBUG_VALID(m_pcImpl);
	Set(cInThat);
}

void H3DF::Component::Set(Component const & cInThat)
{
	ComponentImpl * pcImpl = (ComponentImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	ComponentImpl * pcInThatImpl = (ComponentImpl *)cInThat.m_pcImpl;
	DEBUG_VALID(pcInThatImpl);
	pcImpl->Copy(pcInThatImpl);
}

Component & H3DF::Component::operator = (Component const & cInThat)
{
	Set(cInThat);
	return *this;
}

Component::ComponentType H3DF::Component::GetComponentType() const
{
	ComponentImpl * pcImpl = (ComponentImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->m_eType;
}

bool H3DF::Component::HasComponentType(ComponentType eInMask) const
{
	ComponentImpl * pcImpl = (ComponentImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return (pcImpl->m_eType == eInMask);
}

Key H3DF::Component::GetKey() const
{
	ComponentImpl * pcImpl = (ComponentImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->m_nKey;
}