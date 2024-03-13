#pragma once

#include "../../3DF/3DF.h"
#include "../../3DF/Object.h"

#include "../3DF.Component.h"

namespace H3DF
{
	class API_3DF ComponentImpl : public Impl
	{
	public:
		ComponentImpl();
		~ComponentImpl();

		void Copy(ComponentImpl * pcInThat);

		void SetName(CString strInName);

		CString TypeName();

		void AddSubComponent(Component & cInSubComponent);

		HC_KEY m_nSegmentKey = INVALID_KEY;
		HC_KEY m_nIncludeKey = INVALID_KEY;

		Component::Type m_eType = Component::Type::None;

		DWORD Status();
		DWORD AddStatus(H3DF::Component::Status eStatus);
		DWORD RemoveStatus(H3DF::Component::Status eStatus);

		Component * m_pcOwner = nullptr;
		ComponentArray * m_pvSubComponents = nullptr;

		CString * m_pstrName = nullptr;

		//== Utility Functions =====================================================================
		static bool SetData(Component & cInComponent, CString strInName, HC_KEY nKey, HC_KEY nIncludeKey, Component::Type eInType = Component::Type::None);
		static bool SetName(Component & cInComponent, CString strInName);
		static bool AddSubComponent(Component & cInParentComponent, Component & pcInComponent);
		static bool AddComponentStatus(Component & cInComponent, H3DF::Component::Status eInStatus);
		static bool RemoveComponentStatus(Component & cInComponent, H3DF::Component::Status eInStatus);

	private:
		DWORD m_nStatus = Component::Status::Normal;
	};
}