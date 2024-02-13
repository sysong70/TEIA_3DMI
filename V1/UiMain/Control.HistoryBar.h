#pragma once

#include "Control.ToolBar.h"

//--------------------------------------------------------------------------------------------------

namespace Control
{
	class HistoryBar : public ToolBar
	{
	public:

		HistoryBar();

		~HistoryBar() override;

	public:

		void PushButton(UINT id);

		void RemoveButton(UINT id);

	private:

		int m_nMaxCount = 10;
	};
}
