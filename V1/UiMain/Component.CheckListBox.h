#pragma once

#include "Component.h"



namespace Component
{
	class CheckListBox : public CBCGPCheckListBox
	{
	public:

		CheckListBox();
		// Get image, string, tooltip from resource
		void AddItem(UINT resourceId);

		void AddItems(std::vector<UINT> ids, CSize imageSize);

		void SetImageList(std::vector<UINT> ids, CSize imageSize);

	protected:

		void OnClickItem(int index) override;

		DECLARE_MESSAGE_MAP()
	};
}
