#pragma once

#include "3DF.h"
#include "Object.h"

namespace H3DF
{
	class API_3DF Key : public Object
	{
	public:
		Key();
		Key(HC_KEY nInKey);
		Key(Key const & cInThat);

		H3DF::Type Type() const override;
		H3DF::Type ObjectType() const override { return H3DF::Type::Key; };

		void Set(Key const & cInThat);
		Key const & operator = (Key const & cInThat);

		bool operator == (Key const & cInThat) const;

		bool IsValidate();

		HC_KEY KeyValue() const;
		void SetKeyValue(HC_KEY nInKey);
		void SetKeyValue(HC_KEY nInKey) const;

		bool HasOwner() const;
		// return: The segment containing this key.
		SegmentKey Up() const;
		// return: The segment containing this key.
		SegmentKey Owner() const;

		void Delete();
	};

	using KeyArray = std::vector<Key, Allocator<Key>>;
}