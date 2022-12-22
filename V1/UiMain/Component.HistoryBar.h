#pragma once

#include "Component.ToolBar.h"



namespace Component
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
