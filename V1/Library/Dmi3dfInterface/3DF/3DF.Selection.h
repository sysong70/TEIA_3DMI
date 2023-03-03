#pragma once

#include <HBaseView.h>
#include <HUtility.h>
#include <HTools.h>
#include <HSelectionSet.h>

#include "3DF.h"

#include <vector>

OPEN_3DF_NAMESPACE

class SelectionControl : public HSelectionSet
{
public:
	SelectionControl(HBaseView * pcView, bool bReferenceSelection = false);
	~SelectionControl();

	// overloaded virtuals
	void Init() override;

	void Select(HC_KEY key, int num_include_keys, HC_KEY * include_keys, bool emit_message = true) override;

private:
	bool m_bShowFacesAsLines;

	int	m_nSelectLevel;
	
	struct vlist_s * m_pcSelection;
};

CLOSE_3DF_NAMESPACE