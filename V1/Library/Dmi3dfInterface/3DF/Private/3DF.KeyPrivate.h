#pragma once

#include "../3DF.h"
#include "../3DF.Object.h"

OPEN_3DF_NAMESPACE

class KeyPrivate : public TDF::PrivateImpl
{
public:
	KeyPrivate() {}
	~KeyPrivate() {}

	HC_KEY m_nKey = INVALID_KEY;
	HC_KEY m_nOwnerKey = INVALID_KEY;

	bool m_bOpen = false;
	bool m_bForcedOpen = false; // 개발자가 직접 Open해서 Close를 제어한다. 일반적인 Open과 Close는 적용되지 않는다.

	void Copy(KeyPrivate * pcInThat)
	{
		m_nKey = pcInThat->m_nKey;
		m_nOwnerKey = pcInThat->m_nOwnerKey;

		m_bOpen = pcInThat->m_bOpen;
		m_bForcedOpen = pcInThat->m_bForcedOpen;
	}
};

CLOSE_3DF_NAMESPACE