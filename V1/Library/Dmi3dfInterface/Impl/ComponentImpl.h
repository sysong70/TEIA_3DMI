#pragma once

#include "../3DF/3DF.h"
#include "../3DF/Object.h"

#include "../Component.h"

namespace H3DF
{
	class API_3DF ComponentImpl : public Impl
	{
	public:
		enum API_3DF ComponentStatus
		{
			Normal = 0x0001,
			End = 0x0002,
			UiUpdate = 0x0004,
			Invisible = 0x0008,		// Tree에 나타나면 않되는 요소
			Hide = 0x0010,		// 원래 Hide된 경우
			NoShow = 0x0020,		// NoShow된 경우
		};

		ComponentImpl();
		~ComponentImpl();

		void Copy(ComponentImpl * pcInThat);

		HC_KEY m_nKey = INVALID_KEY;
		Component::ComponentType m_eType = Component::ComponentType::None;
		DWORD m_nStatus = ComponentStatus::Normal;

		Component * m_pcOwner = nullptr;
		ComponentArray * m_vpnSubcomponents = nullptr;
	};
}