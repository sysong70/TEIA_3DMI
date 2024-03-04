#pragma once

#include "3DF/3DF.h"
#include "3DF/Object.h"

namespace H3DF
{
	class API_3DF CADModel : public Object
	{
	public:
		CADModel();
		CADModel(Component const & cInThat);

		void Set(CADModel const & cInThat);
		CADModel & operator = (CADModel const & cInThat);

		H3DF::Type ObjectType() const { return H3DF::Type::CADModel; };

		Component & Root();

		Component & ModelsGroupComponent();
		Component & MeasurementsGroupComponent();
		Component & MarkupsGroupComponent();
// 
// 		ComponentPath GetComponentPath(SelectionItem const & in_item,
// 			ComponentPath::PathType in_path_type = ComponentPath::PathType::Unique) const;

	};
}
