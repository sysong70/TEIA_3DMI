#pragma once

#include "../3DF.h"
#include "../Object.h"

#include <hc.h>

class HSelectionSet;

#include "../Selection.h"

namespace H3DF
{
	class BaseView;

	class API_3DF WindowKeyPrivate : public PrivateImpl
	{
	public:
		WindowKeyPrivate();
		virtual ~WindowKeyPrivate();

		void Copy(WindowKeyPrivate * pcInThat);

		HC_KEY GetSceneKey();
		const HC_KEY GetSceneKey() const;

		H3DF::BaseView * GetBaseView() { return (H3DF::BaseView *) m_pcBaseView; }
		const H3DF::BaseView * m_pcBaseView = nullptr;

		HSelectionSet * m_pcSelectionSet = nullptr;

		SelectionControl * m_pcSelection = nullptr;
		HighlightControl * m_pcHighlight = nullptr;
		SelectionOptionsControl * m_pcSelectionOptions = nullptr;
		SelectionOptionsKit m_cSelectionOptionsKit;

		/*
			HSelectionSet * GetSelection() { return m_pcSelection; }
			HSelectionSet * m_pcSelection = nullptr;

			HSelectionSet * GetHighlight() { return m_pcHighlight; }
			HSelectionSet * m_pcHighlight = nullptr;
		*/

		int m_nViewId = -1;

		HC_KEY * GetSelectBufferKey(int nCount);

	private:
		int m_nSelectBufferKeyCount = 64;
		HC_KEY * m_pnSelectBufferKey = nullptr;
	};
}