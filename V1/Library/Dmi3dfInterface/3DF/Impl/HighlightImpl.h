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
			strncpy(m_chInStyleName, pcInThat->m_chInStyleName, STYLE_BUFFER_SIZE);
			strncpy(m_chInSecondaryStyleName, pcInThat->m_chInSecondaryStyleName, STYLE_BUFFER_SIZE);
			m_nNotification = pcInThat->m_nNotification;
		}

		char m_chInStyleName[STYLE_BUFFER_SIZE];
		char m_chInSecondaryStyleName[STYLE_BUFFER_SIZE];
		int m_nNotification = -1;
	};

	class HighlightControlImpl : public Impl
	{
	public:
		HighlightControlImpl();
		HighlightControlImpl(WindowKey const & cInWindow);

		void Copy(HighlightControlImpl * pcInThat) {
			m_pcWindow = pcInThat->m_pcWindow;
		}

		// 	int SelectButtonDown(Point const & cInLocation, UINT const nFlags, SelectionResults & cOutResults);
		// 	void HandleSelection(UINT const nFlags, SelectionResults & cOutResults);

		WindowKey * GetWindow() { return (WindowKey *) m_pcWindow; }
		const WindowKey * m_pcWindow = nullptr;

		BaseView * GetBaseView();
		BaseView * GetBaseView() const;

		HSelectionSet * GetHighlightSelection();

		//== Highlight 관련 함수 =====================================================================
		void Highlight(SelectionResults const & cInItems, HighlightOptionsKit const & cInOptions, bool bInRemoveExisting = true);
		void Highlight(SelectionItem const & cInItem, HighlightOptionsKit const & cInOptions, bool bInRemoveExisting = true);

		void Unhighlight(SelectionResults const & cInItems, HighlightOptionsKit const & cInOptions = HighlightOptionsKit());
		void Unhighlight(SelectionItem const & cInItem, HighlightOptionsKit const & cInOptions = HighlightOptionsKit());

		//== Mouse Event 처리 =======================================================================
		int NoButtonDownAndMove(int nFlags, int x, int y, SelectionResults & cOutSelections);

		bool DoDynamicHighlighting(WindowPoint cMousePoint, SelectionResults & cOutSelections);

		H3DF::SelectionResults m_cOldHighlightSelection;
	};
}