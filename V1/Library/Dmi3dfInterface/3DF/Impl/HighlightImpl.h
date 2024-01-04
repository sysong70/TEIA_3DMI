#pragma once

#include <HBaseView.h>
#include <HUtility.h>
#include <HTools.h>
#include <HSelectionSet.h>

#include "../3DF.h"
#include "../Kit.h"
#include "../Highlight.h"
#include "../../Impl/ViewImpl.h"

#include "../Math.h"

#include <vector>

namespace H3DF
{
	class HighlightOptionsKitImpl : public Impl
	{
	public:
		void Copy(HighlightOptionsKitImpl * pcInThat) {
			m_strInStyleName = pcInThat->m_strInStyleName;
			m_strInSecondaryStyleName = pcInThat->m_strInSecondaryStyleName;
			m_nNotification = pcInThat->m_nNotification;
		}

		CStringA m_strInStyleName;
		CStringA m_strInSecondaryStyleName;
		int m_nNotification = -1;
	};



	class API_3DF HighlightControlImpl : public Impl
	{
	public:
		HighlightControlImpl(WindowKey const & cInWindow);

		void Copy(HighlightControlImpl * pcInThat) {
			m_pcWindow = pcInThat->m_pcWindow;
		}

		// 	int SelectButtonDown(Point const & cInLocation, UINT const nFlags, SelectionResults & cOutResults);
		// 	void HandleSelection(UINT const nFlags, SelectionResults & cOutResults);

		const WindowKey & Window() { return *m_pcWindow; }

		BaseView * GetBaseView();
		BaseView * GetBaseView() const;

		HSelectionSet * SelectionSet();

		//== Highlight 관련 함수 =====================================================================
		void Highlight(SelectionResults const & cInItems, HighlightOptionsKit const & cInOptions, bool bInRemoveExisting = true);
		void Highlight(SelectionItem const & cInItem, HighlightOptionsKit const & cInOptions, bool bInRemoveExisting = true);

		void Unhighlight(SelectionResults const & cInItems, HighlightOptionsKit const & cInOptions = HighlightOptionsKit());
		void Unhighlight(SelectionItem const & cInItem, HighlightOptionsKit const & cInOptions = HighlightOptionsKit());

		H3DF::SelectionResults m_cOldHighlightSelection;

	private:
		const WindowKey * m_pcWindow = nullptr;
		HSelectionSet * m_pcSelectionSet = nullptr;
	};
}