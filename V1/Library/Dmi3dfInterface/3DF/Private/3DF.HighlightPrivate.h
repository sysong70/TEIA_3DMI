#pragma once

#include <HBaseView.h>
#include <HUtility.h>
#include <HTools.h>
#include <HSelectionSet.h>

#include "../3DF.h"
#include "../3DF.Kit.h"
#include "../3DF.Control.h"
#include "../3DF.Selection.h"

#include "../3DF.BaseView.h"

#include "../3DF.Math.h"

#include <vector>

OPEN_3DF_NAMESPACE

class HighlightOptionsKitPrivate : public PrivateImpl
{
public:
	void Copy(HighlightOptionsKitPrivate * pcInThat) {
		strncpy(m_chInStyleName, pcInThat->m_chInStyleName, STYLE_BUFFER_SIZE);
		strncpy(m_chInSecondaryStyleName, pcInThat->m_chInSecondaryStyleName, STYLE_BUFFER_SIZE);
		m_nNotification = pcInThat->m_nNotification;
	}

	char m_chInStyleName[STYLE_BUFFER_SIZE];
	char m_chInSecondaryStyleName[STYLE_BUFFER_SIZE];
	int m_nNotification = -1;
};

class HighlightControlPrivate : public PrivateImpl
{
public:
	HighlightControlPrivate() { m_eType = TDF::Type::HighlightControl; }

	void Copy(HighlightControlPrivate * pcInThat) {
		m_pcBaseView = pcInThat->m_pcBaseView;
		m_pcWindow = pcInThat->m_pcWindow;
		m_pcSelectionSet = pcInThat->m_pcSelectionSet;
	}

	// 	int SelectButtonDown(Point const & cInLocation, UINT const nFlags, SelectionResults & cOutResults);
	// 	void HandleSelection(UINT const nFlags, SelectionResults & cOutResults);

	WindowKey * GetWindow() { return (WindowKey *)m_pcWindow; }
	const WindowKey * m_pcWindow = nullptr;

	TDF::BaseView * GetBaseView() { return (TDF::BaseView *)m_pcBaseView; }
	const TDF::BaseView * m_pcBaseView = nullptr;

	HSelectionSet * m_pcSelectionSet = nullptr;
};

CLOSE_3DF_NAMESPACE