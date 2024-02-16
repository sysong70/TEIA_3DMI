#include "stdafx.h"
#include "resource.h"
#include "Component.RibbonBar.h"
#include "Window.Application.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

class RibbonSplitButton : public CBCGPRibbonButton
{
public:

	RibbonSplitButton()
	{
		CommonInit();
	}

	RibbonSplitButton(int id, LPCTSTR lpszText)
		: CBCGPRibbonButton(id, lpszText)
	{
		//:WARNING - split button mode
		SetDefaultCommand(TRUE);
	}

public:

	void SelectChild(int index)
	{
		SelectChild((CBCGPRibbonButton*)m_arSubItems[index]);
	}

	void SelectChild(CBCGPRibbonButton* pChild)
	{
		auto& cmd = TheCommandIndexer.Get(GetID());
		ASSERT(cmd.Id != -1); // Not dummy
		cmd.ChildId = pChild->GetID();

		//:WARNING - bAlphaBlendIcon == TRUE
		SetIcon(pChild->GetIcon(), TRUE, FALSE, TRUE);
		//:CHECK - can not change size...
		//SetText(pChild->GetText());
		//SetToolTipText(pChild->GetToolTipText());
	}
};

//**************************************************************************************************

class RibbonChildItem : public CBCGPRibbonButton
{
	DECLARE_DYNCREATE(RibbonChildItem)

public:

	RibbonChildItem()
	{
		CommonInit();
	}

	RibbonChildItem(int id, LPCTSTR lpszText)
		: CBCGPRibbonButton(id, lpszText)
	{}

public: // CBCGPBaseRibbonElement

	void OnAfterAddToParent(CBCGPBaseRibbonElement* pParentElem) override
	{
		m_pParentElem = (RibbonSplitButton*)pParentElem;
	}

public: // CBCGPRibbonButton

	void OnClick(CPoint point) override
	{
		if (m_pParentElem == nullptr) {
			//:WARNING - this is a new dynamically created entity! why??
			RibbonChildItem* inRibbon = (RibbonChildItem*)TheApplication.GetMainFrame().GetRibbonBar().FindByID(GetID());
			DEBUG_VALID(inRibbon);
			inRibbon->m_pParentElem->SelectChild(inRibbon);
		}
		else {
			m_pParentElem->SelectChild(this);
		}

		CBCGPRibbonButton::OnClick(point);
	}

private:

	RibbonSplitButton* m_pParentElem = nullptr;
};



IMPLEMENT_DYNCREATE(RibbonChildItem, CBCGPRibbonButton)

//**************************************************************************************************

class RibbonButton : public CBCGPRibbonButton
{
public:

	RibbonButton()
	{
		CommonInit();
	}

	RibbonButton(int id, LPCTSTR lpszText)
		: CBCGPRibbonButton(id, lpszText)
	{}

public: // CBCGPBaseRibbonElement

	// For icon size of context menu

	//BOOL OnDrawMenuImage(CDC* pDC, CRect rect) override
	//{
	//	DrawImage(pDC, RibbonImageLarge, rect);
	//	return TRUE;
	//}
};

//**************************************************************************************************

#define PRESET PresetRibbonBar

namespace PresetRibbonBar
{
	CSize IconSize()
	{
		return globalUtils.ScaleByDPI(CSize(32, 32));
	}



	CBCGPRibbonPanel* CreatePanel(CBCGPRibbonCategory* pCategory, int id)
	{
		CBCGPRibbonPanel* pPanel = pCategory->AddPanel(Facility::GetTitle(id));
		pPanel->SetNonCollapsible();

		return pPanel;
	}

	// Single button

	CBCGPRibbonButton* CreateButton(int id, CString otherTitle = L"")
	{
		CString title, tooltip;
		Facility::GetResource(id, title, tooltip);
		//:CHECK
		if (otherTitle.IsEmpty() == false) {
			title = otherTitle;
		}

		//CBCGPRibbonButton* pButton = new CBCGPRibbonButton(id, title);
		CBCGPRibbonButton* pButton = new RibbonButton(id, title);
		pButton->SetIcon(Facility::CreateIcon(id, PRESET::IconSize()), TRUE, FALSE, TRUE);
		pButton->SetToolTipText(tooltip);
		pButton->SetAlwaysLargeImage();

		return pButton;
	}

	// Drop-down style button

	CBCGPRibbonButton* CreateButton(int baseId, int startId, int endId)
	{
		CBCGPRibbonButton* pParent = CreateButton(baseId);
		// not split mode
		pParent->SetDefaultCommand(FALSE);

		for (int id = startId; id <= endId; id++) {
			pParent->AddSubItem(CreateButton(id));
		}

		return pParent;
	}

	// Drop-down style button with splite mode

	CBCGPRibbonButton* CreateSplitButton(int baseId, int startId, int endId)
	{
		CString title, tooltip;
		Facility::GetResource(baseId, title, tooltip);

		RibbonSplitButton* pParent = new RibbonSplitButton(baseId, title);
		pParent->SetIcon(Facility::CreateIcon(baseId, PRESET::IconSize()), TRUE, FALSE, TRUE);
		pParent->SetToolTipText(tooltip);
		pParent->SetAlwaysLargeImage();

		for (int id = startId; id <= endId; id++) {
			Facility::GetResource(id, title, tooltip);

			RibbonChildItem* pItem = new RibbonChildItem(id, title);
			pItem->SetIcon(Facility::CreateIcon(id, PRESET::IconSize()), TRUE, FALSE, TRUE);
			pItem->SetToolTipText(tooltip);
			pItem->SetAlwaysLargeImage();

			pParent->AddSubItem(pItem);
		}

		//:WARNING - select first item
		pParent->SelectChild(0);

		return pParent;
	}


	/*
	CBCGPRibbonPaletteButton* CreatePalette(int baseId, int startId, int endId, bool splitMode = true)
	{
		// make image list
		CBCGPToolBarImages images;
		images.SetImageSize(PRESET::IconSize());

		for (int id = startId; id <= endId; id++) {
			//:WARNING - do not use local variable
			CBCGPSVGImage* pImage = new CBCGPSVGImage();
			pImage->Load(id);
			images.AddSVG(pImage);
		}

		CBCGPRibbonPaletteButton* pButton = new CBCGPRibbonPaletteButton(
			startId, // palette base id
			Facility::GetTitle(baseId), // default title
			0, 0, // image index on panel
			images // sub item images
		);

		pButton->SetIcon(Facility::CreateIcon(baseId, PRESET::IconSize()), TRUE, FALSE, TRUE);
		pButton->SetButtonMode();
		pButton->SetAlwaysLargeImage();
		pButton->SetDefaultCommand(splitMode);
		pButton->SetComboMode(); // text label on the right of icon

		// set sub items tooltip and user data
		int index = 0;
		CString title, tooltip;

		for (int id = startId; id <= endId; id++) {
			Facility::CommandIndexer::CommandInfo& item = TheCommandIndexer.Get(id);
			Facility::GetResource(id, title, tooltip);

			pButton->SetItemToolTip(index, title + L"\n" + tooltip);
			pButton->SetItemUserData(index, (DWORD_PTR)&item);
			index++;
		}

		if (splitMode) {
			// replace icon and text
			pButton->ShowSelectedImage(TRUE, TRUE);
		}
		pButton->SelectItem(0); // select first

		return pButton;
	}
	*/
}

//**************************************************************************************************

Component::RibbonBar::RibbonBar()
{
}



Component::RibbonBar::~RibbonBar()
{
}



bool Component::RibbonBar::Initialize(CWnd* pMainFrame)
{
	if (Create(pMainFrame) == FALSE) {
		RETURN_FALSE;
	}

	//EnableCustomization();
	//EnableKeyTips();
	EnableMinimizeButton();
	EnableToolTips(TRUE, TRUE);

	SetApplicationModes((UINT)Window::EAppMode::Standard);
	SetMinimizeButtonLocation(BCGPRibbonMinimizeButtonLocation_RightOfCategory);
	//SetGrayDisabledImages();
	//SetImagesLuminosity(1.1); // 0.1, 1.1
	//ShowCategory();
	//ShowContextCategories();
	ToggleMinimizeState();

	bool success = CreateMainCategory() && CreateCategories();
	RedrawWindow();

	return success;
}



bool Component::RibbonBar::CreateMainCategory()
{
#define _USE_DEFAULT_MAIN_

#ifdef _USE_DEFAULT_MAIN_

#pragma region Main button and Search
	//m_wndMainButton.SetImage(IDB_RIBBON_MAIN, TRUE);
	CString title = Facility::GetTitle(FILE_3D_CAT);
	m_wndMainButton.SetText(title);
	m_wndMainButton.SetScenicText(title);
	SetMainButton(&m_wndMainButton, CSize(45, 45));

	EnableCommandSearch(TRUE, Facility::Local(L"Enter menu name to search...|검색할 메뉴명을 입력하세요..."));
	CBCGPRibbonCommandSearchOptions so;
	so.m_Location = BCGPRibbonCommandSearchLocation_OnCaption;
	SetCommandSearchOptions(so);
#pragma endregion //:REGION

#pragma region Main Category
	CBCGPRibbonMainPanel* pMain = AddMainCategory(title, 0, 0);

	pMain->Add(PRESET::CreateButton(FILE_3D_CMD_New));
	pMain->AddSeparator();
	pMain->Add(PRESET::CreateButton(FILE_3D_CMD_Open));
	pMain->Add(PRESET::CreateButton(FILE_3D_CMD_Insert));
	pMain->AddSeparator();
	pMain->Add(PRESET::CreateButton(FILE_3D_CMD_Save));
	pMain->Add(PRESET::CreateButton(FILE_3D_CMD_SaveAs));
	pMain->Add(PRESET::CreateButton(FILE_3D_CMD_Export));
	pMain->AddSeparator();
	pMain->Add(PRESET::CreateButton(FILE_3D_CMD_Print));
	pMain->AddSeparator();
	pMain->Add(PRESET::CreateButton(FILE_3D_CMD_Options));

	pMain->AddRecentFilesList(Facility::Local(L"Recent Documents|최근 파일"), 300, TRUE); //:WARNING
#pragma endregion //:REGION

#pragma region Right Side Menu
/*
	CMenu styleMenu;
	styleMenu.CreateMenu();

	//:WARNING - check Id
	styleMenu.AppendMenuW(MF_STRING, CMD_THEME_LIGHT, Facility::Local(L"Light Theme|밝은 테마"));
	styleMenu.AppendMenuW(MF_STRING, CMD_THEME_DARK, Facility::Local(L"Dark Theme|어두운 테마"));

	CBCGPRibbonButton* pStyle = new CBCGPRibbonButton(
		-1, Facility::Local(L"Color Theme|색 테마"), -1, -1); //:WARNING
	pStyle->SetMenu(styleMenu.GetSafeHmenu());
	AddToTabs(pStyle);
*/
#pragma endregion //:REGION - Will be replaced by another menu

#else

#pragma region Main and Search
	EnableCommandSearch(
		TRUE, Facility::Local(L"Enter menu name to search...|검색할 메뉴명을 입력하세요...")); //:WARNING
#pragma endregion //:REGION

#pragma region Main Category
	CBCGPRibbonCategory* pCategory = AddCategory(GetTitle(FILE_3D_CAT), 0, 0);
	CBCGPRibbonPanel* pPanel = nullptr;

	pPanel = CreatePanel(pCategory, FILE_3D_PNL_Open);
	pPanel->Add(CreateButton(FILE_3D_CMD_New));
	pPanel->Add(CreateButton(FILE_3D_CMD_Open));
	pPanel->Add(CreateButton(FILE_3D_CMD_Insert));

	pPanel = CreatePanel(pCategory, FILE_3D_PNL_Save);
	pPanel->Add(CreateButton(FILE_3D_CMD_Save));
	pPanel->Add(CreateButton(FILE_3D_CMD_SaveAs));
	pPanel->Add(CreateButton(FILE_3D_CMD_Export));
	pPanel->Add(CreateButton(FILE_3D_CMD_Print));

	pPanel = CreatePanel(pCategory, FILE_3D_PNL_Settings);
	pPanel->Add(CreateButton(FILE_3D_CMD_Preference));

	//:WARING - add recent files...
#pragma endregion //:REGION

#endif
	return true;
}



bool Component::RibbonBar::CreateCategories()
{
	CBCGPRibbonCategory* pCategory = nullptr;
	CBCGPRibbonPanel* pPanel = nullptr;

	using namespace PRESET;

#pragma region Home Category
	pCategory = AddCategory(Facility::GetTitle(HOME_3D_CAT), 0, 0);

	pPanel = PRESET::CreatePanel(pCategory, HOME_3D_PNL_Panels);
	pPanel->Add(PRESET::CreateButton(HOME_3D_CMD_Panels_Model));
	pPanel->Add(PRESET::CreateButton(HOME_3D_CMD_Panels_View));
	pPanel->Add(PRESET::CreateButton(HOME_3D_CMD_Panels_Layer));
	pPanel->Add(PRESET::CreateButton(HOME_3D_CMD_Panels_Scene));

	pPanel = PRESET::CreatePanel(pCategory, HOME_3D_PNL_Focus);
	pPanel->Add(PRESET::CreateButton(HOME_3D_CMD_Pan));
	pPanel->Add(PRESET::CreateSplitButton(HOME_3D_LST_Zoom, HOME_3D_CMD_Zoom_Fit, HOME_3D_CMD_Zoom_Object));
	pPanel->Add(PRESET::CreateSplitButton(HOME_3D_LST_Rotate, HOME_3D_CMD_Rotate_Rotate, HOME_3D_CMD_Rotate_Turntable));

	pPanel = PRESET::CreatePanel(pCategory, HOME_3D_PNL_View);
	pPanel->Add(PRESET::CreateSplitButton(HOME_3D_LST_ViewStyle, HOME_3D_CMD_ViewStyle_Shade, HOME_3D_CMD_ViewStyle_Tessellated));
	pPanel->Add(PRESET::CreateButton(HOME_3D_LST_ViewDirection, HOME_3D_CMD_ViewDirection_Top, HOME_3D_CMD_ViewDirection_Perspective));
	pPanel->Add(PRESET::CreateButton(HOME_3D_LST_Visualize, HOME_3D_CMD_Visualize_ShowAll, HOME_3D_CMD_Visualize_Toggle));
	pPanel->Add(PRESET::CreateButton(HOME_3D_LST_VisualEffects));

	pPanel = PRESET::CreatePanel(pCategory, HOME_3D_PNL_SelectAndSnap);
	pPanel->Add(PRESET::CreateButton(HOME_3D_LST_Select, HOME_3D_CMD_Select_All, HOME_3D_CMD_Select_Axis));
	pPanel->Add(PRESET::CreateButton(HOME_3D_POP_SelectionFiter, HOME_3D_CMD_SelectionFiter_Point, HOME_3D_CMD_SelectionFiter_PMI));
	pPanel->Add(PRESET::CreateButton(HOME_3D_POP_ObjectSnap, HOME_3D_CMD_ObjectSnap_Point, HOME_3D_CMD_ObjectSnap_ExpandLine));

	pPanel = PRESET::CreatePanel(pCategory, HOME_3D_PNL_Window);
	pPanel->Add(PRESET::CreateButton(HOME_3D_CMD_Window_Cascade));
	pPanel->Add(PRESET::CreateButton(HOME_3D_CMD_Window_TileHorizontal));
	pPanel->Add(PRESET::CreateButton(HOME_3D_CMD_Window_TileVertical));
#pragma endregion //:REGION

#pragma region Measure Category
	pCategory = AddCategory(Facility::GetTitle(MEASURE_3D_CAT), 0, 0);

	pPanel = PRESET::CreatePanel(pCategory, MEASURE_3D_PNL_Basic);
	pPanel->Add(PRESET::CreateButton(MEASURE_3D_CMD_Basic_Coordinate));
	pPanel->Add(PRESET::CreateButton(MEASURE_3D_CMD_Basic_Distance));
	pPanel->Add(PRESET::CreateButton(MEASURE_3D_CMD_Basic_Length));
	pPanel->Add(PRESET::CreateButton(MEASURE_3D_CMD_Basic_Radius));
	pPanel->Add(PRESET::CreateButton(MEASURE_3D_CMD_Basic_Angle));

	pPanel = PRESET::CreatePanel(pCategory, MEASURE_3D_PNL_Physical);
	pPanel->Add(PRESET::CreateButton(MEASURE_3D_CMD_Physical_BoundBox));
	pPanel->Add(PRESET::CreateButton(MEASURE_3D_CMD_Physical_Area));
	pPanel->Add(PRESET::CreateButton(MEASURE_3D_CMD_Physical_VolumnWeight));
	pPanel->Add(PRESET::CreateButton(MEASURE_3D_CMD_Physical_CenterOfGravity));
	pPanel->Add(PRESET::CreateButton(MEASURE_3D_CMD_Physical_ProjectionArea));

	pPanel = PRESET::CreatePanel(pCategory, MEASURE_3D_PNL_Annotation);
	pPanel->Add(PRESET::CreateButton(MEASURE_3D_CMD_Annotation));

	pPanel = PRESET::CreatePanel(pCategory, MEASURE_3D_PNL_Settings);
	pPanel->Add(PRESET::CreateButton(MEASURE_3D_CMD_Settings));
#pragma endregion //:REGION

#pragma region Custom Category - remove ids in Command.Resource.h later
	pCategory = AddCategory(Facility::GetTitle(CUSTOM_3D_CAT), 0, 0);

	pPanel = PRESET::CreatePanel(pCategory, CUSTOM_3D_PNL_SYSONG);
	pPanel->Add(PRESET::CreateButton(CUSTOM_3D_CMD_SYSONG_Test1));
	pPanel->Add(PRESET::CreateButton(CUSTOM_3D_CMD_SYSONG_Test2));
	pPanel->Add(PRESET::CreateButton(CUSTOM_3D_CMD_SYSONG_Test3));
	pPanel->Add(PRESET::CreateButton(CUSTOM_3D_CMD_SYSONG_Test4));
	pPanel->Add(PRESET::CreateButton(CUSTOM_3D_CMD_SYSONG_Test5));
	pPanel->Add(PRESET::CreateButton(CUSTOM_3D_CMD_SYSONG_Test6));
	pPanel->Add(PRESET::CreateButton(CUSTOM_3D_CMD_SYSONG_Test7));
	pPanel->Add(PRESET::CreateButton(CUSTOM_3D_CMD_SYSONG_Test8));
	pPanel->Add(PRESET::CreateButton(CUSTOM_3D_CMD_SYSONG_Test9));

	pPanel = PRESET::CreatePanel(pCategory, CUSTOM_3D_PNL_KEN);
	pPanel->Add(PRESET::CreateButton(CUSTOM_3D_CMD_KEN_Test1));
	pPanel->Add(PRESET::CreateButton(CUSTOM_3D_CMD_KEN_Test2));
	pPanel->Add(PRESET::CreateButton(CUSTOM_3D_CMD_KEN_Test3));
	pPanel->Add(PRESET::CreateButton(CUSTOM_3D_CMD_KEN_Test4));
	pPanel->Add(PRESET::CreateButton(CUSTOM_3D_CMD_KEN_Test5));
	pPanel->Add(PRESET::CreateButton(CUSTOM_3D_CMD_KEN_Test6));
	pPanel->Add(PRESET::CreateButton(CUSTOM_3D_CMD_KEN_Test7));
	pPanel->Add(PRESET::CreateButton(CUSTOM_3D_CMD_KEN_Test8));
	pPanel->Add(PRESET::CreateButton(CUSTOM_3D_CMD_KEN_Test9, L"Coordinate"));
#pragma endregion //:REGION

	return true;
}



void Component::RibbonBar::Reload()
{
	RemoveAllCategories();

	CreateCategories();
	CreateMainCategory();
}

#undef PRESET
