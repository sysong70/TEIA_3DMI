#pragma once

#include "../3DF.h"
#include "../Object.h"

namespace H3DF
{
	class API_3DF KeyImpl : public H3DF::Impl
	{
	public:
		KeyImpl() { m_eType = Type::Key; }
		virtual ~KeyImpl();

		HC_KEY const KeyValue() const;
		void SetKeyValue(HC_KEY nInKey);

		void Copy(KeyImpl * pcInThat)
		{
			m_nKey = pcInThat->KeyValue();
			m_eType = pcInThat->Type();
		}

	private:
		HC_KEY m_nKey = INVALID_KEY;
	};
}