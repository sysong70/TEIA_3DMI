#pragma once

#include "../3DF.h"
#include "../3DF.Object.h"

OPEN_3DF_NAMESPACE

class KeyPrivate : public TDF::PrivateImpl
{
public:
	KeyPrivate() { m_eType = Type::Key; }
	~KeyPrivate() {}

	HC_KEY m_nKey = INVALID_KEY;

	void Copy(KeyPrivate * pcInThat)
	{
		m_nKey = pcInThat->m_nKey;
		m_eType = pcInThat->Type();
	}
};

CLOSE_3DF_NAMESPACE