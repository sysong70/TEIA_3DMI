#include "stdafx.h"

#include "Ast.h"
#include "Ctl.StatusBar.h"
#include "Dlg.ObjectSnaps.h"

#include <Signal2d.h>
#include <Signal3d.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

namespace
{
	enum class Id
	{
		Unknown = WM_USER,
		Message,
		Coordinate,
	};
}

//**************************************************************************************************

class StatusBarButton : public CBCGPRibbonStatusBarPane
{
public:

	StatusBarButton(UINT nCmdID)
		: CBCGPRibbonStatusBarPane(nCmdID, NULL)
	{
		SetIcon(Ast::CreateIcon(nCmdID, Ctl::ImageSize()));
	}

	void DrawImage(CDC* pDC, RibbonImageType type, CRect rectImage) override
	{
		HICON hIcon = m_hIcon;
		CSize iconSize = Ctl::ImageSize();
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

	// WARNING - remove menu arrow
	virtual void OnDrawMenuArrow(CDC* pDC, const CRect& rectMenuArrow) override
	{
	}

	// CHECK
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

	//	COLORREF clrText = IsDisabled() ? (COLORREF)EColor::OldSilver : globalData.clrBarText;

	//	m_bIsHighlighted = bIsHighlighted;
	//	m_bIsPressed = bIsPressed;
	//	m_bIsDisabled = bIsDisabled;

	//	return clrText;
	//}
};



CtlStatusBar::CtlStatusBar()
{
}



CtlStatusBar::~CtlStatusBar()
{
}



bool CtlStatusBar::Initialize(CWnd* pMainFrame)
{
	const enum Id
	{
		Unknown = WM_USER,
		Message,
		Coordinate,
	};

	if (Create(pMainFrame) == FALSE) {
		RETURN_FALSE;
	}

	AddElement(new CBCGPRibbonStatusBarPane(Message, L"", TRUE), L"MessagePane");
	AddExtendedElement(new CBCGPRibbonStatusBarPane(Coordinate, L"", TRUE), L"CoordinatePane");

	StatusBarButton* sf = new StatusBarButton(HOME_3D_POP_SelectionFiter);
	AddExtendedElement(sf, L"");

	StatusBarButton* os = new StatusBarButton(HOME_3D_POP_ObjectSnap);
	os->EnablePopupDialog(RUNTIME_CLASS(DlgObjectSnaps), IDD_DMI_DROPDOWN, FALSE, FALSE, TRUE);
	AddExtendedElement(os, L"");

	// CHECK
	GetElement(0)->SetText(Ast::Local(L"Select a command|명령을 선택하세요"));
	// CHECK - how to Initialize width?
	CBCGPRibbonStatusBarPane* pCoord = (CBCGPRibbonStatusBarPane*)GetExElement(0);
	pCoord->SetText(L"0000000000.0000, 0000000000.0000");
	pCoord->Redraw();

	return true;
}



void CtlStatusBar::ReceiveSignal(Json::Object* pData)
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



void CtlStatusBar::ShowMessage(Json::Object& data)
{
	CBCGPRibbonStatusBarPane* pPane = (CBCGPRibbonStatusBarPane*)GetElement(0);
	if (pPane == nullptr) {
		DEBUG_RETURN;
	}

	pPane->SetText(data.GetString(SKW_MESSAGE));
	pPane->Redraw();
}



void CtlStatusBar::ShowCoordinate(Json::Object& data)
{
	CString coord;
	if (data.FindValue(SKW_Z) == nullptr) {
		coord.Format(L"%.4f, %.4f", data.GetReal(SKW_X), data.GetReal(SKW_Y));
	}
	else {
		coord.Format(L"%.4f, %.4f, %.4f", data.GetReal(SKW_X), data.GetReal(SKW_Y), data.GetReal(SKW_Z));
	}

	ShowCoordinate(coord);
}

void CtlStatusBar::ShowCoordinate(const CString& value)
{
	CBCGPRibbonStatusBarPane* pPane = (CBCGPRibbonStatusBarPane*)GetExElement(0);
	if (pPane == nullptr) {
		DEBUG_RETURN;
	}

	pPane->SetText(value);
	pPane->Redraw();
}

void CtlStatusBar::ShowCoordinate(double x, double y)
{
	ShowCoordinate(WStr::Format(L"%.4f, %.4f", x, y));
}

void CtlStatusBar::ShowCoordinate(double x, double y, double z)
{
	ShowCoordinate(WStr::Format(L"%.4f, %.4f, %.4f", x, y, z));
}
