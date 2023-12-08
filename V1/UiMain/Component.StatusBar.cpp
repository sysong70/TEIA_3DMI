#include "stdafx.h"
#include "resource.h"
#include "Component.StatusBar.h"
#include "Dialog.ObjectSnaps.h"
#include "Facility.h"
#include <Signal.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



#define PRESET PresetStatusBar

namespace PresetStatusBar
{
	enum EPaneId
	{
		Unknown = WM_USER,
		Message,
		Coordinate,
	};
}



class StatusBarButton : public CBCGPRibbonStatusBarPane
{
public:

	StatusBarButton(UINT nCmdID)
		: CBCGPRibbonStatusBarPane(nCmdID, NULL)
	{
		SetIcon(Facility::CreateIcon(nCmdID, Control::ImageSize()));
	}

	void DrawImage(CDC* pDC, RibbonImageType type, CRect rectImage) override
	{
		HICON hIcon = m_hIcon;
		CSize iconSize = Control::ImageSize();
		CSize offset = globalUtils.ScaleByDPI(CSize(4, 3));

		//if (m_bIsDisabled) {
		//	CBCGPToolBarImages icon;
		//	icon.SetImageSize(iconSize);
		//	icon.AddIcon(hIcon, FALSE);

		//	CBCGPDrawState ds;
		//	icon.PrepareDrawImage(ds, iconSize, TRUE);
		//	icon.Draw(pDC, rectImage.left - offset.cx, rectImage.top - offset.cy, 0, FALSE, TRUE);
		//	icon.EndDrawImage(ds);
		//}
		//else {
		//	::DrawIconEx(pDC->GetSafeHdc(),
		//		rectImage.left - offset.cx, rectImage.top - offset.cy,
		//		hIcon, iconSize.cx, iconSize.cy, 0, NULL, DI_NORMAL);
		//}

		::DrawIconEx(pDC->GetSafeHdc(),
			rectImage.left - offset.cx, rectImage.top - offset.cy,
			hIcon, iconSize.cx, iconSize.cy, 0, NULL, DI_NORMAL);
	}

	CSize GetSize(CDC* pDC) override
	{
		return globalUtils.ScaleByDPI(CSize(28, 29));
	}

	//:WARNING - remove menu arrow
	virtual void OnDrawMenuArrow(CDC* pDC, const CRect& rectMenuArrow) override
	{
	}

	//:CHECK
	//virtual COLORREF OnFillBackground(CDC* pDC) override
	//{
	//	ASSERT_VALID(this);
	//	ASSERT_VALID(pDC);

	//	BOOL bIsHighlighted = m_bIsHighlighted;
	//	BOOL bIsPressed = m_bIsPressed;
	//	BOOL bIsDisabled = m_bIsDisabled;

	//	if (m_bIsStatic) {
	//		m_bIsDisabled = FALSE;
	//	}

	//	if (m_bIsStatic || m_bIsDisabled) {
	//		m_bIsHighlighted = FALSE;
	//		m_bIsPressed = FALSE;
	//	}

	//	CRect rect = GetRect();

	//	if (IsHighlighted()) {
	//		if (IsPressed()) {
	//			pDC->FillSolidRect(rect, globalData.clrBarDkShadow);
	//		}
	//		else {
	//			pDC->FillSolidRect(rect, globalData.clrBarShadow);
	//		}
	//	}
	//	else if (IsChecked()) {
	//		pDC->FillSolidRect(rect, globalData.clrBarShadow);
	//	}

	//	COLORREF clrText = IsDisabled() ? (COLORREF)Component::EColor::OldSilver : globalData.clrBarText;

	//	m_bIsHighlighted = bIsHighlighted;
	//	m_bIsPressed = bIsPressed;
	//	m_bIsDisabled = bIsDisabled;

	//	return clrText;
	//}
};



Component::StatusBar::StatusBar()
{
}



Component::StatusBar::~StatusBar()
{
}



bool Component::StatusBar::Initialize(CWnd* pMainFrame)
{
	if (Create(pMainFrame) == FALSE) {
		RETURN_FALSE;
	}

	AddElement(new CBCGPRibbonStatusBarPane(PRESET::Message, L"", TRUE), L"MessagePane");
	AddExtendedElement(new CBCGPRibbonStatusBarPane(PRESET::Coordinate, L"", TRUE), L"CoordinatePane");

	StatusBarButton* sf = new StatusBarButton(HOME_3D_POP_SelectionFiter);
	AddExtendedElement(sf, L"");

	StatusBarButton* os = new StatusBarButton(HOME_3D_POP_ObjectSnap);
	os->EnablePopupDialog(RUNTIME_CLASS(Dialog::ObjectSnaps), IDD_DMI_DROPDOWN, FALSE, FALSE, TRUE);
	AddExtendedElement(os, L"");

#ifdef _DEBUG
	//GetElement(0)->SetText(L"Message Pane");
	//GetExElement(0)->SetText(L"Coordinate Pane");
#endif

	return true;
}



void Component::StatusBar::ReceiveSignal(Json::Object* pData)
{
	Json::Object& data = *pData;

	Signal::StatusBar::Action action = (Signal::StatusBar::Action)data.GetInteger(SKW_ACTION);
	switch (action) {
	case Signal::StatusBar::Action::ShowMessage:    ShowMessage(data);    break;
	case Signal::StatusBar::Action::ShowCoordinate: ShowCoordinate(data); break;

	default:
		DEBUG_STOP;
		break;
	}

	REMOVE_POINTER(pData);
}



void Component::StatusBar::ShowMessage(Json::Object& data)
{
	CBCGPRibbonStatusBarPane* pPane = (CBCGPRibbonStatusBarPane*)GetElement(0);
	if (pPane == nullptr) {
		DEBUG_RETURN;
	}

	pPane->SetText(data.GetString(SKW_MESSAGE));
	pPane->Redraw();
}



void Component::StatusBar::ShowCoordinate(Json::Object& data)
{
	CBCGPRibbonStatusBarPane* pPane = (CBCGPRibbonStatusBarPane*)GetExElement(0);
	if (pPane == nullptr) {
		DEBUG_RETURN;
	}

	CString coord;
	if (data.FindValue(SKW_Z) == nullptr) {
		coord.Format(L"%.4f, %.4f", data.GetReal(SKW_X), data.GetReal(SKW_Y));
	}
	else {
		coord.Format(L"%.4f, %.4f, %.4f", data.GetReal(SKW_X), data.GetReal(SKW_Y), data.GetReal(SKW_Z));
	}

	pPane->SetText(coord);
	pPane->Redraw();
}

#undef PRESET
