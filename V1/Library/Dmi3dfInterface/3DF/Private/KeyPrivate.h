#pragma once

#include "../3DF.h"
#include "../Object.h"

namespace H3DF
{
	class API_3DF KeyPrivate : public H3DF::PrivateImpl
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
}