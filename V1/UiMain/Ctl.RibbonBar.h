#pragma once

#include "Ctl.h"
#include "Wnd.h"

//--------------------------------------------------------------------------------------------------

class CtlRibbonBar : public CBCGPRibbonBar
{

public:

	using Categories = std::list<CBCGPRibbonCategory*>;

	Wnd::EDocType DocType;
	CBCGPRibbonMainButton MainButtonCtl;
	Categories Categoies3d;
	Categories Categoies2d;
	// For Context Menu
	CBCGPRibbonCategory* DummyCategory = nullptr;

public:

	CtlRibbonBar() {}

	~CtlRibbonBar() override {}

public:

	CBCGPRibbonCategory* AddCategory(LPCTSTR lpszName);

public:

	bool Initialize(CWnd* pMainFrame, Wnd::EDocType eType = Wnd::EDocType::Model);
	// doc & view changed
	void ChangeByDocType(Wnd::EDocType eType);
	// for changing language, ...
	void Reload();

public:

	bool CreateMainCategory();

	bool Create3dCategories();

	bool Create2dCategories();

	void SetPadding(const CSize& value);
};
