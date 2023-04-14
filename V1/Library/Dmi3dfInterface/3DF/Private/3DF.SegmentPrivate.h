#pragma once

#include "../3DF.h"
#include "../3DF.Object.h"

#include "3DF.KeyPrivate.h"

OPEN_3DF_NAMESPACE

class SegmentKeyPrivate : public KeyPrivate
{
public:
	SegmentKeyPrivate() { m_eType = Type::SegmentKey; }
	~SegmentKeyPrivate() {}

	void Copy(SegmentKeyPrivate * pcInThat);

	//== Segment 관련 함수 ===========================================================================
	void Open();
	void Open() const;

	void Close();
	void Close() const;

	void ForcedOpen();
	void ForcedClose();

	bool IsOpen() const;
	bool IsForcedOpen() const;

	HC_KEY const KeyValue() const;
	void SetKeyValue(HC_KEY nInKey);

private:
	mutable bool m_bOpen = false;
	mutable bool m_bForcedOpen = false; // 개발자가 직접 Open해서 Close를 제어한다. 일반적인 Open과 Close는 적용되지 않는다.
};

CLOSE_3DF_NAMESPACE