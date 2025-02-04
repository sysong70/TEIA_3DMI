#pragma once

// #include <HBaseView.h>
// #include <HUtility.h>
// #include <HTools.h>
// #include <HSelectionSet.h>

#include "../3DF.h"
#include "../Selection.h"

#include "../Impl/ControlImpl.h"

#include "Math.h"

#include <vector>

class HSelectionSet;

namespace H3DF
{
	class WindowKey;
	class BaseView;

	class HighlightOptionsKitImpl : public Impl
	{
	public:
		HighlightOptionsKitImpl() { m_eType = H3DF::Type::HighlightOptionsKit; }

		void Copy(HighlightOptionsKitImpl * pcInThat) {
			m_strInStyleName = pcInThat->m_strInStyleName;
			m_strInSecondaryStyleName = pcInThat->m_strInSecondaryStyleName;
			m_nNotification = pcInThat->m_nNotification;
		}

		CStringA m_strInStyleName;
		CStringA m_strInSecondaryStyleName;
		int m_nNotification = -1;
	};

	class HighlightControlImpl : public ControlImpl
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

		H3DF::SelectionResults m_cOldHighlightSelection;

	private:
		const WindowKey * m_pcWindow = nullptr;
		HSelectionSet * m_pcSelectionSet = nullptr;
	};
}
