#pragma once

#include "../3DF.h"
#include "../Object.h"
#include "../Portfolio.h"

namespace H3DF
{
	class DefinitionImpl : public H3DF::Impl
	{
	public:
		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<DefinitionImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		HC_KEY const KeyValue() const;
		void SetKeyValue(HC_KEY nInKey);

		void Copy(const DefinitionImpl * pcInThat)
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