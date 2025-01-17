#pragma once

#include "../../3DF/3DF.h"
#include "../../3DF/Object.h"

#include "../3DF.Component.h"

namespace H3DF
{
	class ComponentImpl : public Impl
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

		CString m_strName;
		ComponentArray * m_pvSubComponents = nullptr;
		MetadataArray * m_pvMetaDatas = nullptr;
	};

	namespace ComponentUtility
	{
		//== Utility Functions =====================================================================
		API_3DF bool SetData(Component & cInComponent, CString strInName, HC_KEY nKey, HC_KEY nIncludeKey, Component::Type eInType = Component::Type::None);
		API_3DF bool SetName(Component & cInComponent, CString strInName);
		API_3DF bool AddSubComponent(Component & cInParentComponent, Component & pcInComponent);
		API_3DF CString TypeName(Component & cInComponent);
		API_3DF bool FindParentPartDefinition(Component & cInComponent, Component *& pcOutComponent);
		API_3DF Component * FindSubComponentBySegmentKey(Component & cInComponent, HC_KEY nInSegmentKey, bool bRecursive);

		API_3DF bool ClonedParentPartDefinition(Component & cInComponent);
		API_3DF bool ClonedComponent(Component & cInComponent, Component & cInOwnerComponent, bool bDeleteInclude);

		API_3DF Component * GetViewGroupComponent(Component & cInParentComp);
		API_3DF Component * GetAnnotationViewGroupComponent(Component & cInParentComp);
		API_3DF Component * GetPmiGroupComponent(Component & cInParentComp);
	};
}