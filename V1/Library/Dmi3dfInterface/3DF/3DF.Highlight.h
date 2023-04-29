#pragma once

#include <HBaseView.h>
#include <HUtility.h>
#include <HTools.h>
#include <HSelectionSet.h>

#include "3DF.h"
#include "3DF.Kit.h"
#include "3DF.Control.h"
#include "3DF.Selection.h"

#include "3DF.Math.h"

#include <vector>

OPEN_3DF_NAMESPACE

class HighlightOptionsKit : public Kit
{
public:
	explicit HighlightOptionsKit();
	HighlightOptionsKit(char const * chInStyleName);
	HighlightOptionsKit(char const * chInStyleName, char const * chInSecondaryStyleName);
	HighlightOptionsKit(HighlightOptionsKit const & cInThat);

	void Set(HighlightOptionsKit const & cInThat);
	HighlightOptionsKit & operator=(HighlightOptionsKit const & cInThat);

	// Whether to inject a notification event after highlighting (or unhighlighting).
	// param: bInState Whether to inject a notification event after highlighting(or unhighlighting).
	HighlightOptionsKit & SetNotification(bool bInState);
	HighlightOptionsKit & UnsetNotification();
	bool ShowNotification(bool & bOutState) const;
};

class HighlightControl : public Control
{
public:
	explicit HighlightControl(WindowKey const & cInWindow);
	HighlightControl(HighlightControl const & cInThat);
	~HighlightControl();

	void Set(HighlightControl const & cInThat);
	HighlightControl & operator=(HighlightControl const & cInThat);

	TDF::Type ObjectType() const { return TDF::Type::HighlightControl; };

	HighlightControl & Highlight(SelectionResults const & cInItems, HighlightOptionsKit const & cInOptions, bool bInRemoveExisting = true);

	HighlightControl & Unhighlight(SelectionResults const & cInItems, HighlightOptionsKit const & cInOptions = HighlightOptionsKit());
	HighlightControl & Unhighlight(SelectionItem const & cInItem, HighlightOptionsKit const & cInOptions = HighlightOptionsKit());

private:
	// Private default constructor to prevent instantiation without a window.
	HighlightControl();
};

CLOSE_3DF_NAMESPACE