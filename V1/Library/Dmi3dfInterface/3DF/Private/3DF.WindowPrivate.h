#pragma once

#include "../3DF.h"
#include "../3DF.Object.h"

#include <hc.h>

class HSelectionSet;

OPEN_3DF_NAMESPACE

class BaseView;
class HighlightControl;
class SelectionControl;

class WindowKeyPrivate : public PrivateImpl
{
public:
	WindowKeyPrivate();
	virtual ~WindowKeyPrivate();

	void Copy(WindowKeyPrivate * pcInThat);

	H3DF::BaseView * GetBaseView() { return (H3DF::BaseView *) m_pcBaseView; }
	const H3DF::BaseView * m_pcBaseView = nullptr;

	HSelectionSet * m_pcSelectionSet = nullptr;

	SelectionControl * m_pcSelection = nullptr;
	HighlightControl * m_pcHighlight = nullptr;
	SelectionOptionsControl * m_pcSelectionOptions = nullptr;

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

CLOSE_3DF_NAMESPACE