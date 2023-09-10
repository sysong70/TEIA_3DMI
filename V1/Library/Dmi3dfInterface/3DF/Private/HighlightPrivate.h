#pragma once

#include <HBaseView.h>
#include <HUtility.h>
#include <HTools.h>
#include <HSelectionSet.h>

#include "../3DF.h"
#include "../Kit.h"
#include "../Control.h"
#include "../3DF.Selection.h"

#include "../BaseView.h"

#include "../Math.h"

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
	HighlightControlPrivate() { m_eType = H3DF::Type::HighlightControl; }

	void Copy(HighlightControlPrivate * pcInThat) {
		m_pcBaseView = pcInThat->m_pcBaseView;
		m_pcWindow = pcInThat->m_pcWindow;
		m_pcSelectionSet = pcInThat->m_pcSelectionSet;
	}

	// 	int SelectButtonDown(Point const & cInLocation, UINT const nFlags, SelectionResults & cOutResults);
	// 	void HandleSelection(UINT const nFlags, SelectionResults & cOutResults);

	WindowKey * GetWindow() { return (WindowKey *)m_pcWindow; }
	const WindowKey * m_pcWindow = nullptr;

	H3DF::BaseView * GetBaseView() { return (H3DF::BaseView *)m_pcBaseView; }
	const H3DF::BaseView * m_pcBaseView = nullptr;

	HSelectionSet * m_pcSelectionSet = nullptr;
};

CLOSE_3DF_NAMESPACE