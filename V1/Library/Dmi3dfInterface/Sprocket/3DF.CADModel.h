#pragma once

#include "3DF.Component.h"

namespace H3DF
{
	class API_3DF CADModel : public Component
	{
	public:
		CADModel();
		//CADModel(Component const & cInThat);

		void Set(CADModel const & cInThat);
		CADModel & operator = (CADModel const & cInThat);

		H3DF::Type ObjectType() const { return H3DF::Type::CADModel; };

		Component * GetComponentFromKey(HC_KEY cInKey) const;

		bool ShowSelectionResult(Component * pcInComponent, H3DF::SelectionResults & cOutResults);
/*
		Component & Root();

		Component & ModelsGroupComponent();
		Component & MeasurementsGroupComponent();
		Component & MarkupsGroupComponent();

		CString GetName() const;*/
// 
// 		ComponentPath GetComponentPath(SelectionItem const & in_item,
// 			ComponentPath::PathType in_path_type = ComponentPath::PathType::Unique) const;

	};
}
