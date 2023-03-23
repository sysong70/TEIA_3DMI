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

	virtual ~HighlightOptionsKit();

	void Set(HighlightOptionsKit const & cInThat);
	HighlightOptionsKit & operator=(HighlightOptionsKit const & cInThat);
};

class HighlightControl : public Control
{
public:
	explicit HighlightControl(WindowKey const & cInWindow);
	HighlightControl(HighlightControl const & cInThat);
	~HighlightControl();

	void Set(HighlightControl const & cInThat);
	HighlightControl & operator=(HighlightControl const & cInThat);

	_3DF::Type ObjectType() const { return _3DF::Type::HighlightControl; };

	void DynamicHighlight(Point const & cInLocation);

	HighlightControl & Highlight(SelectionResults const & cInItems, HighlightOptionsKit const & cInOptions, bool bInRemoveExisting = true);

private:
	// Private default constructor to prevent instantiation without a window.
	HighlightControl();
};

CLOSE_3DF_NAMESPACE