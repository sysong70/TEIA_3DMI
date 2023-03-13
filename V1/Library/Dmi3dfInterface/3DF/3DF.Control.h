#pragma once

#include "3DF.h"
#include "3DF.Object.h"

OPEN_3DF_NAMESPACE

class API_3DF Control : public Object
{
public:
	Control() {};
	Control(HC_KEY nKey);
	virtual ~Control() {};

	virtual void Open() const;
	virtual void Close() const;

	HC_KEY KeyValue() const { return m_nKey; }

	bool HasOwner() const;
	void SetOwerKey(HC_KEY nInKey);

	SegmentKey Owner() const;

	void Delete();

protected:
	HC_KEY m_nKey = INVALID_KEY;
	HC_KEY m_nOwnerKey = INVALID_KEY;
};

CLOSE_3DF_NAMESPACE