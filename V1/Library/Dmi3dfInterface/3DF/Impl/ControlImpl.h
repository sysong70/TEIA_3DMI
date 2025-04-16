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

		void Copy(ControlImpl * pcInThat)
		{
			m_cOverrideKey = pcInThat->m_cOverrideKey;
		}
	};
}
