#pragma once

#include "Component.h"
#include "Facility.CommandIndexer.h"

//--------------------------------------------------------------------------------------------------

namespace Component
{
	class RibbonBar : public CBCGPRibbonBar
	{
	public:

		friend class MainFrame;
		friend class View;

		RibbonBar();

		~RibbonBar();

	public:

		bool Initialize(CWnd* pMainFrame);

		bool CreateMainCategory();

		bool CreateCategories();

		void Reload();

	private:

		CBCGPRibbonMainButton m_wndMainButton;
	};
}
