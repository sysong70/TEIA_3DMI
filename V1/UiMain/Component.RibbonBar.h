#pragma once

#include "Component.h"
#include "Facility.CommandIndexer.h"



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

	public:

		Facility::CommandIndexer::Command& GetData(UINT id);

		Facility::CommandIndexer::Command& GetData(CBCGPRibbonCategory* pCategory, UINT id);

	private:

		CBCGPRibbonMainButton m_wndMainButton;
	};
}
