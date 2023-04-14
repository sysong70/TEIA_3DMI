#pragma once

#include "3DF.h"
#include "3DF.Object.h"

OPEN_3DF_NAMESPACE

class API_3DF Key : public Object
{
public:
	Key(HC_KEY nInKey = INVALID_KEY);
	Key(Key const & cInThat);
	virtual ~Key();

	void Set(Key const & cInThat);
	Key const & operator=(Key const & cInThat);

	static const TDF::Type staticType = TDF::Type::Key;
	TDF::Type ObjectType() const { return staticType; };

	HC_KEY KeyValue() const;
	void SetKeyValue(HC_KEY nInKey);
	void SetKeyValue(HC_KEY nInKey) const;


	void Delete();
};

CLOSE_3DF_NAMESPACE