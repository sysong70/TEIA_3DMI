#pragma once

#include <hps.h>
#include <sprk.h>

class DmiHpsView;

class DmiSelectOperator : public HPS::Operator
{
public:

	// SelectOperator constructor. The operator is not active until it is attached to a HPS::View. 
	// Even after being attached, operator must remain in scope until you're done using it.
	// 
	// param: in_mouse_trigger The mouse button that will trigger this operator.
	//		Defaults to left mouse button.
	// param: in_modifier_trigger The modifier key that, when used with the in_mouse_trigger, will trigger this operator.
	//		Passing an empty ModifierKeys object means no modifier key is necessary to trigger the operator.

	DmiSelectOperator(HPS::MouseButtons cInMouseTrigger = HPS::MouseButtons::ButtonLeft(), HPS::ModifierKeys cInModifierTrigger = HPS::ModifierKeys());

	HPS::UTF8 GetName() const override { return "DMI_SelectOperator"; }

	bool OnMouseDown(HPS::MouseState const & cInState) override;
	bool OnMouseUp(HPS::MouseState const & cInState) override;
	bool OnMouseMove(HPS::MouseState const & cInState) override;
	bool OnDaynamicHighlightMouseMove(HPS::MouseState const & cInState);

	bool OnTouchDown(HPS::TouchState const & cInState) override;

	// This method returns the results of the last selection action. If no objects were selected,
	// the SelectionResults object returned will be have a count of 0.
	HPS::SelectionResults GetActiveSelection() const { return m_cActiveSelectionResult; }
	HPS::SelectionResults GetNewDynamicActiveSelection() const { return m_cNewDynamicActiveSelection; }
	HPS::SelectionResults GetOldDynamicActiveSelection() const { return m_cOldDynamicActiveSelection; }

	// Accepts a SelectionOptionsKit that defines the parameters that will be used for selection.
	// param: in_options The SelectionOptionsKit from which the selection options will be set
	void SetSelectionOptions(HPS::SelectionOptionsKit const & cInOptions) { m_cSelectionOptions = cInOptions; }

	// Gets the HPS::SelectionOptionsKit that has been set on this operator.
	// return: The HPS::SelectionOptionsKit associated with this operator
	HPS::SelectionOptionsKit GetSelectionOptions() const { return m_cSelectionOptions; }

	bool IsSameSelect() { return m_bSameSelectSourceFlag; }

protected:
	HPS::SelectionResults m_cActiveSelectionResult;
	HPS::SelectionResults m_cNewDynamicActiveSelection;
	HPS::SelectionResults m_cOldDynamicActiveSelection;

	bool m_bSameSelectSourceFlag = false;

	bool IsSameSelectSource(HPS::SelectionResults & cSelOne, HPS::SelectionResults & cSelTwo);

private:
	bool SelectCommon(HPS::SelectionResults & cSelection, HPS::Point const & cInLocation, HPS::WindowKey & cInWindow, HPS::ModifierKeys cInModifiers = HPS::ModifierKeys());
	
	HPS::SelectionOptionsKit m_cSelectionOptions;
};