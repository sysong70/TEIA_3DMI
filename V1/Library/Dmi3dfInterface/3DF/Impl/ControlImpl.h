#pragma once

#include "../3DF.h"
#include "../Object.h"
#include "../Window.h"
#include "../Segment.h"
#include "../Impl/SegmentImpl.h"

namespace H3DF 
{
	class BaseView;

	class ControlImpl : public H3DF::Impl
	{
	public:
		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<ControlImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy(const ControlImpl * pcInThat)
		{
			m_cOverrideKey = pcInThat->m_cOverrideKey;
		}

		SegmentKey m_cOverrideKey;

		H3DF::BaseView * GetBaseView() const
		{
			SegmentKeyImpl * pcSegmentImpl = (SegmentKeyImpl *)m_cOverrideKey.GetImpl();
			DEBUG_VALID(pcSegmentImpl);

			if (nullptr == pcSegmentImpl->GetWindow()) {
				DEBUG_STOP;
				return nullptr;
			}

			return pcSegmentImpl->GetWindow()->GetBaseView();
		}
	};
}
