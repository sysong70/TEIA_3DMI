#include "stdafx.h"
#include "resource.h"
#include "Component.RibbonBar.h"
#include "Window.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



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

	CBCGPRibbonButton* CreateButton(int id)
	{
		Facility::CommandIndexer::CommandInfo& item = TheCommandIndexer.Get(id);

		CString title, tooltip;
		Facility::GetResource(id, title, tooltip);

		CBCGPRibbonButton* pButton = new CBCGPRibbonButton(id, title);
		pButton->SetIcon(Facility::CreateIcon(id, PRESET::IconSize()), TRUE, FALSE, TRUE);
		pButton->SetToolTipText(tooltip);
		pButton->SetAlwaysLargeImage();
		pButton->SetData((DWORD_PTR)&item);

		return pButton;
	}

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
}



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
	EnableToolTips(FALSE, FALSE);

	SetApplicationModes((UINT)Window::EAppMode::Standard);
	SetMinimizeButtonLocation(BCGPRibbonMinimizeButtonLocation_RightOfCategory);
	//SetGrayDisabledImages();
	//SetImagesLuminosity(1.1); // 0.1, 1.1
	//ShowCategory();
	//ShowContextCategories();
	ToggleMinimizeState();

	return CreateMainCategory() && CreateCategories();
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
	pPanel->Add(PRESET::CreatePalette(HOME_3D_LST_Zoom, HOME_3D_CMD_Zoom_Fit, HOME_3D_CMD_Zoom_Object));
	//pPanel->Add(PRESET::CreatePalette(HOME_3D_LST_Rotate, HOME_3D_CMD_Rotate_Rotate, HOME_3D_CMD_Rotate_Orbit));
	pPanel->Add(PRESET::CreatePalette(HOME_3D_LST_Rotate, HOME_3D_CMD_Rotate_Rotate, HOME_3D_CMD_Rotate_Turntable));

	pPanel = PRESET::CreatePanel(pCategory, HOME_3D_PNL_View);
	pPanel->Add(PRESET::CreatePalette(HOME_3D_LST_ViewStyle, HOME_3D_CMD_ViewStyle_Shade, HOME_3D_CMD_ViewStyle_Tessellated, true));
	pPanel->Add(PRESET::CreatePalette(HOME_3D_LST_ViewDirection, HOME_3D_CMD_ViewDirection_Top, HOME_3D_CMD_ViewDirection_Perspective, true));
	pPanel->Add(PRESET::CreatePalette(HOME_3D_LST_Visualize, HOME_3D_CMD_Visualize_ShowAll, HOME_3D_CMD_Visualize_Toggle, true));
	//:CHECK
	//pPanel->Add(PRESET::CreatePalette(HOME_3D_LST_VisualEffects, HOME_3D_CMD_VisualEffects_Shadow, HOME_3D_CMD_VisualEffects_Bloom, false));
	pPanel->Add(PRESET::CreateButton(HOME_3D_LST_VisualEffects));

	pPanel = PRESET::CreatePanel(pCategory, HOME_3D_PNL_SelectAndSnap);
	pPanel->Add(PRESET::CreatePalette(HOME_3D_LST_Select, HOME_3D_CMD_Select_All, HOME_3D_CMD_Select_Axis, false));
	pPanel->Add(PRESET::CreatePalette(HOME_3D_POP_SelectionFiter, HOME_3D_CMD_SelectionFiter_Point, HOME_3D_CMD_SelectionFiter_PMI, false));
	pPanel->Add(PRESET::CreatePalette(HOME_3D_POP_ObjectSnap, HOME_3D_CMD_ObjectSnap_Point, HOME_3D_CMD_ObjectSnap_ExpandLine, false));

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

	//:TEST - remove ids in Command.Resource.h
#pragma region Custom Category
	pCategory = AddCategory(Facility::GetTitle(CUSTOM_3D_CAT), 0, 0);

	pPanel = PRESET::CreatePanel(pCategory, CUSTOM_3D_PNL_Test);
	pPanel->Add(PRESET::CreateButton(CUSTOM_3D_CMD_Test1));
	pPanel->Add(PRESET::CreateButton(CUSTOM_3D_CMD_Test2));
	pPanel->Add(PRESET::CreateButton(CUSTOM_3D_CMD_Test3));
	pPanel->Add(PRESET::CreateButton(CUSTOM_3D_CMD_Test4));
	pPanel->Add(PRESET::CreateButton(CUSTOM_3D_CMD_Test5));
	pPanel->Add(PRESET::CreateButton(CUSTOM_3D_CMD_Test6));
	pPanel->Add(PRESET::CreateButton(CUSTOM_3D_CMD_Test7));
	pPanel->Add(PRESET::CreateButton(CUSTOM_3D_CMD_Test8));
	pPanel->Add(PRESET::CreateButton(CUSTOM_3D_CMD_Test9));
#pragma endregion //:REGION

	return true;
}



void Component::RibbonBar::Reload()
{
	RemoveAllCategories();

	CreateCategories();
	CreateMainCategory();
}



Facility::CommandIndexer::CommandInfo& Component::RibbonBar::GetData(UINT id)
{
	CBCGPBaseRibbonElement* pElem = FindByID(id, FALSE);
	if (pElem != nullptr) {
		DWORD_PTR data = pElem->GetData();
		if (data != 0) {
			return *(Facility::CommandIndexer::CommandInfo*)data;
		}
	}

	DEBUG_STOP;
	return TheCommandIndexer.GetDummyData();
}

Facility::CommandIndexer::CommandInfo& Component::RibbonBar::GetData(CBCGPRibbonCategory* pCategory, UINT id)
{
	if (pCategory != nullptr) {
		CBCGPBaseRibbonElement* pElem = pCategory->FindByID(id, FALSE);
		if (pElem != nullptr) {
			DWORD_PTR data = pElem->GetData();
			if (data != 0) {
				return *(Facility::CommandIndexer::CommandInfo*)data;
			}
		}
	}

	DEBUG_STOP;
	return TheCommandIndexer.GetDummyData();
}

#undef PRESET
