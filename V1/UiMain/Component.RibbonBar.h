#pragma once

#include "Component.h"
#include "Window.h"

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

	protected:

		CBCGPRibbonCategory* AddCategory(LPCTSTR lpszName);

	public:

		bool Initialize(CWnd* pMainFrame, Window::EDocType eType = Window::EDocType::Model);
		// doc & view changed
		void ChangeByDocType(Window::EDocType eType);
		// for changing language, ...
		void Reload();

	protected:

		bool CreateMainCategory();

		bool Create3dCategories();

		bool Create2dCategories();

		void SetPadding(const CSize& value);

	private:

		using Categories = std::list<CBCGPRibbonCategory*>;

		Window::EDocType m_eDocType;
		CBCGPRibbonMainButton m_wndMainButton;
		Categories m_categoies3d;
		Categories m_categoies2d;
		// For Context Menu
		CBCGPRibbonCategory* m_pDummy = nullptr;
	};
}
