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

	static const _3DF::Type staticType = _3DF::Type::Key;
	_3DF::Type ObjectType() const { return staticType; };

	virtual void Open();
	virtual void Open() const;

	virtual void Close();
	virtual void Close() const;

	void ForcedOpen();
	void ForcedClose();

	HC_KEY KeyValue() const;
	void SetKeyValue(HC_KEY nInKey);
	void SetKeyValue(HC_KEY nInKey) const;

	bool IsOpen() const;
	bool IsForcedOpen() const;

	bool HasOwner() const;
	void SetOwerKey(HC_KEY nInKey);

	SegmentKey Owner();

	void Delete();

	Key * Copy() const;
};

CLOSE_3DF_NAMESPACE