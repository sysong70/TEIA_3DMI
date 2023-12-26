#pragma once

#include "3DF.h"
#include "Object.h"

OPEN_3DF_NAMESPACE

class API_3DF Key : public Object
{
public:
	Key();
	Key(HC_KEY nInKey);
	Key(Key const & cInThat);

	void Set(Key const & cInThat);
	Key const & operator = (Key const & cInThat);

	H3DF::Type ObjectType() const { return H3DF::Type::Key; };

	bool operator == (Key const & cInThat) const;

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

CLOSE_3DF_NAMESPACE