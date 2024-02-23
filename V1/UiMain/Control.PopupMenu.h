#pragma once

#include "Control.h"

//--------------------------------------------------------------------------------------------------

namespace Control
{
	class PopupMenu : public CBCGPListBox
	{
	public:

		PopupMenu();

	public:

		void AddItem(UINT id);

		void AddItems(const ResourceIds& ids, CSize imageSize);

		void SetCheckItems(const ResourceIds& ids);

	protected:

		void OnClickItem(int index) override;

		DECLARE_MESSAGE_MAP()
	};
}
