#pragma once

#include "3DF.Segment.h"
#include "3DF.Selection.h"
#include "3DF.Highlight.h"

class HBaseView;

OPEN_3DF_NAMESPACE

class API_3DF WindowKey : public SegmentKey
{
public:
	WindowKey(HBaseView * pcBaseView);
	WindowKey(WindowKey const & cInThat);

	const HBaseView * GetBaseView() const;
	HBaseView * GetBaseView();

	HC_KEY GetSceneKey();
	const HC_KEY GetSceneKey() const;

	int ViewId();
	const int ViewId() const;
	void SetViewId(int nViewId);

	int OnMouseMove(HEventInfo & cEvent);

	// Returns a control that allows the user to manipulate and query details of the selection options on this window.
	SelectionOptionsControl GetSelectionOptionsControl();
	SelectionOptionsControl const GetSelectionOptionsControl() const;

	// Returns a control that allows the user to perform selections in this window.
	SelectionControl GetSelectionControl();
	SelectionControl const GetSelectionControl() const;

	// Returns a control that allows the user to highlight segments or geometry in this window.
	HighlightControl GetHighlightControl();
	HighlightControl const GetHighlightControl() const;

	void Initialize();

private:
	SelectionControl * m_pcSelection;
};

CLOSE_3DF_NAMESPACE