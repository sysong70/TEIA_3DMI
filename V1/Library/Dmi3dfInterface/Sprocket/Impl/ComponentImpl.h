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

		void AddSubComponent(Component & cInSubCompnent);

		ComponentArray * GetAllSubcomponents(Component::Type eInType) const;
		void GetSubcomponents(Component::Type eInType, ComponentArray * pcOutSubComponents, bool bRecursive) const;
		
		Component * FindUpComponent(Component::Type eInType);
		Component * FindSubComponentBySegmentKey(HC_KEY nInSegmentKey, bool bRecursive);

		HC_KEY m_nSegmentKey = INVALID_KEY;
		HC_KEY m_nIncludeKey = INVALID_KEY;

		Component::Type m_eType = Component::Type::None;
		
		DWORD m_nStatus = Component::Status::None;
		
		Component * m_pcOwner = nullptr;

		CString * m_pstrName = nullptr;
		ComponentArray * m_pvSubComponents = nullptr;
		MetadataArray * m_pvMetaDatas = nullptr;

		//== Utility Functions =====================================================================
		static bool SetData(Component & cInComponent, CString strInName, HC_KEY nKey, HC_KEY nIncludeKey, Component::Type eInType = Component::Type::None);
		static bool SetName(Component & cInComponent, CString strInName);
		static bool AddSubComponent(Component & cInParentComponent, Component & pcInComponent);
		static CString TypeName(Component & cInComponent);
		static bool FindParentPartDefinition(Component & cInComponent, Component *& pcOutComponent);

		static bool ClonedParentPartDefinition(Component & cInComponent);
		static bool ClonedComponent(Component & cInComponent, Component & cInOwnerComponent, bool bDeleteInclude);

	};
}