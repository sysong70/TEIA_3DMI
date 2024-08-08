#pragma once

#include "../3DF.h"
#include "../Object.h"

namespace H3DF
{
	class API_3DF DefinitionImpl : public H3DF::Impl
	{
	public:
		HC_KEY const KeyValue() const;
		void SetKeyValue(HC_KEY nInKey);

		void Copy(DefinitionImpl * pcInThat)
		{
			m_nKey = pcInThat->KeyValue();
		}

	private:
		HC_KEY m_nKey = INVALID_KEY;
	};
}