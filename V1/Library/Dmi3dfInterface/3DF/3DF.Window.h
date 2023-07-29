#pragma once

#include "3DF.Segment.h"
#include "3DF.Selection.h"
#include "3DF.Highlight.h"

OPEN_3DF_NAMESPACE

class BaseView;

class API_3DF WindowKey : public SegmentKey
{
public:
	WindowKey(TDF::BaseView * pcBaseView);
	WindowKey(WindowKey const & cInThat);
	~WindowKey();

	const TDF::BaseView * GetBaseView() const;
	TDF::BaseView * GetBaseView();

	HC_KEY GetSceneKey();
	const HC_KEY GetSceneKey() const;

	void Update();

	int ViewId();
	const int ViewId() const;
	void SetViewId(int nViewId);

	int OnMouseMove(HEventInfo & cEvent);

	// Returns a control that allows the user to manipulate and query details of the selection options on this window.
	SelectionOptionsControl & GetSelectionOptionsControl();
	SelectionOptionsControl const & GetSelectionOptionsControl() const;

	// Returns a control that allows the user to perform selections in this window.
	SelectionControl & GetSelectionControl();
	SelectionControl const & GetSelectionControl() const;

	// Returns a control that allows the user to highlight segments or geometry in this window.
	HighlightControl & GetHighlightControl();
	HighlightControl const & GetHighlightControl() const;

	void Initialize();
};

CLOSE_3DF_NAMESPACE