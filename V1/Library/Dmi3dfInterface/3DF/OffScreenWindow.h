#pragma once

#include "3DF.h"
#include "Window.h"

namespace H3DF
{
	class API_3DF OffScreenWindowKey : public WindowKey {
	public:
		OffScreenWindowKey();
/*
		OffScreenWindowKey(Key const & cInKey);
		OffScreenWindowKey(OffScreenWindowKey const & cInThat);
		OffScreenWindowKey(OffScreenWindowKey && cInThat);
		OffScreenWindowKey & operator=(OffScreenWindowKey && in_that);

		~OffScreenWindowKey();

		H3DF::Type ObjectType() const override { return H3DF::Type::VisualEffectsControl; }

		void Set(OffScreenWindowKey const & cInThat);
		OffScreenWindowKey & operator = (OffScreenWindowKey const & cInThat);
*/
	};
}