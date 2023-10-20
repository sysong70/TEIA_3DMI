#pragma once

#include "3DF.h"
#include "KeyPath.h"
#include "Kit.h"
#include "Control.h"

#include "Math.h"
#include "Point.h"

#include <vector>

#include <HBaseView.h>
#include <HUtility.h>
#include <HTools.h>
#include <HSelectionSet.h>

namespace H3DF
{
	class SelectionSet : public HSelectionSet
	{
	public:
		SelectionSet(HBaseView * pcView, bool bReferenceSelection = false);
		~SelectionSet();

		// overloaded virtuals
		void Init() override;

		void Select(HC_KEY key, int num_include_keys, HC_KEY * include_keys, bool emit_message = true) override;

	private:
		bool m_bShowFacesAsLines;

		int	m_nSelectLevel;

		struct vlist_s * m_pcSelection;
	};
}