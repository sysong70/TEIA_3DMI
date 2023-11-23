#pragma once

// #include <HBaseView.h>
// #include <HUtility.h>
// #include <HTools.h>
// #include <HSelectionSet.h>

#include "3DF.h"
#include "Kit.h"
#include "Control.h"
#include "Selection.h"

#include "Math.h"

#include <vector>

namespace H3DF
{
	class API_3DF HighlightOptionsKit : public Kit
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

	class API_3DF HighlightControl : public Control
	{
	public:
		explicit HighlightControl(WindowKey const & cInWindow);
		//HighlightControl(HighlightControl const & cInThat);

		void Set(HighlightControl const & cInThat);
		HighlightControl & operator=(HighlightControl const & cInThat);

		H3DF::Type ObjectType() const { return H3DF::Type::HighlightControl; };

		HighlightControl & Highlight(SelectionResults const & cInItems, HighlightOptionsKit const & cInOptions, bool bInRemoveExisting = true);
		HighlightControl & Highlight(SelectionItem const & cInItem, HighlightOptionsKit const & cInOptions, bool bInRemoveExisting = true);

		HighlightControl & Unhighlight(SelectionResults const & cInItems, HighlightOptionsKit const & cInOptions = HighlightOptionsKit());
		HighlightControl & Unhighlight(SelectionItem const & cInItem, HighlightOptionsKit const & cInOptions = HighlightOptionsKit());

		//== Material Mapping 관련 함수 ==================================================================
		HighlightControl & SetMaterialMapping(MaterialMappingKit const & cInKit);
		MaterialMappingControl GetMaterialMappingControl();
		MaterialMappingControl const GetMaterialMappingControl() const;

		//== Line Attribute 관련 함수 ====================================================================
		//HighlightControl & SetLineAttribute(LineAttributeKit const & cInKit);

		LineAttributeControl GetLineAttributeControl();
		LineAttributeControl const GetLineAttributeControl() const;


	private:
		// Private default constructor to prevent instantiation without a window.
		HighlightControl();

		HighlightControl & Highlight_ORG(SelectionResults const & cInItems, HighlightOptionsKit const & cInOptions, bool bInRemoveExisting = true);
	};
}
