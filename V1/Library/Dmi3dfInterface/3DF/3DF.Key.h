#pragma once

#include "3DF.h"

OPEN_3DF_NAMESPACE

class API_3DF Key
{
public:
	Key() {};
	Key(HC_KEY nInKey);
	virtual ~Key();

	virtual void Open();
	virtual void Close();

	void ForcedOpen();
	void ForcedClose();

	HC_KEY KeyValue() const { return m_nKey; }
	bool IsOpen() const { return m_bOpen; }
	bool IsForcedOpen() const { return m_bForcedOpen; }

	bool HasOwner() const;
	void SetOwerKey(HC_KEY nInKey);

	SegmentKey Owner();

	void Delete();

protected:
	HC_KEY m_nKey = INVALID_KEY;
	HC_KEY m_nOwnerKey = INVALID_KEY;

	bool m_bOpen = false;
	bool m_bForcedOpen = false; // 개발자가 직접 Open해서 Close를 제어한다. 일반적인 Open과 Close는 적용되지 않는다.
};

CLOSE_3DF_NAMESPACE