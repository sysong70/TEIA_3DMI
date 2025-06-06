#pragma once

#include "3DF.h"

#include "Geometry.h"

namespace H3DF
{
	class API_3DF ReferenceKey : public H3DF::GeometryKey
	{
	public:
		ReferenceKey();
		ReferenceKey(HC_KEY nInKey);
		explicit ReferenceKey(Key const & cInKey);
		ReferenceKey(ReferenceKey const & cInThat);

		~ReferenceKey();

		static const H3DF::Type staticType = H3DF::Type::ReferenceKey;
		H3DF::Type ObjectType() const { return staticType; };

		void Set(ReferenceKey const & cInThat);
		ReferenceKey & operator = (ReferenceKey const & cInThat);

		Key GetTarget() const;
	};
}