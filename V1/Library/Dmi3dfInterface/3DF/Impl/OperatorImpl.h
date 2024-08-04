#pragma once

#include "../3DF.h"
#include "../Object.h"
#include "../../Sprocket/Impl/3DF.View.Impl.h"

#include <HBaseView.h>
#include <HUtility.h>
#include <HTools.h>
#include <HSelectionSet.h>

namespace H3DF
{
	namespace Operator
	{
		class API_3DF OperatorImpl : public H3DF::Impl
		{
		public:
			OperatorImpl();
			OperatorImpl(WindowKey const & cInWindow);

			void Copy(OperatorImpl * pcInThat) {
				m_pcWindow = pcInThat->m_pcWindow;
			}

			WindowKey * GetWindow() { return (WindowKey *)m_pcWindow; }
			const WindowKey * m_pcWindow = nullptr;

			BaseView * GetBaseView();
			BaseView * GetBaseView() const;
		};
	}
}