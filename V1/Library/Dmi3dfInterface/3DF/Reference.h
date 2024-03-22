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

		void Set(ReferenceKey const & cInThat);
		ReferenceKey & operator = (ReferenceKey const & cInThat);

		H3DF::Type ObjectType() const { return H3DF::Type::ReferenceKey; };

		Key GetTarget() const;
	};
}