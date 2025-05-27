#pragma once

#include "../3DF.h"
#include "../Object.h"
#include "../Portfolio.h"

namespace H3DF
{
	class DefinitionImpl : public H3DF::Impl
	{
	public:
		DefinitionImpl() { m_eType = H3DF::Type::Definition; }

		HC_KEY const KeyValue() const;
		void SetKeyValue(HC_KEY nInKey);

		void Set(DefinitionImpl * pcInThat)
		{
			m_nKey = pcInThat->KeyValue();
			m_cOwnerPortfolio = pcInThat->m_cOwnerPortfolio;
			m_strName = pcInThat->m_strName;
		}

		PortfolioKey m_cOwnerPortfolio;
		CStringA m_strName;
		HC_KEY m_nKey = INVALID_KEY;
	};
}