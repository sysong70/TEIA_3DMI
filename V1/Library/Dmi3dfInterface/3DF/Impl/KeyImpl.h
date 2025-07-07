#pragma once

#include "3DF.Impl.h"
#include "../Object.h"

namespace H3DF
{
	class KeyImpl : public H3DF::Impl
	{
	public:
		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<KeyImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

// 		void Copy(const KeyImpl * pcInThat) {
// 			m_nKey = pcInThat->KeyValue();
// 		}

		void Copy(const Impl * pcInThat) override {
			auto pcImpl = static_cast<const KeyImpl *>(pcInThat);
			if (nullptr == pcImpl) {
				DEBUG_RETURN;
			}

			Impl::Copy(pcInThat);

			m_nKey = pcImpl->KeyValue();
		}


		HC_KEY const KeyValue() const;
		void SetKeyValue(HC_KEY nInKey);

	private:
		HC_KEY m_nKey = INVALID_KEY;
	};
}