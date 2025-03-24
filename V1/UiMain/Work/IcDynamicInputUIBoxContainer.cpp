/*****************************************************************************
***
***    Copyright (C) 2014 ProgeSOFT sa. All Rights Reserved.
***     All rights reserved.
***
***     Use of the information contained herein, in part or in whole,
***     in/as source code and/or in/as object code, in any way by anyone
***     other than authorized employees of The ProgeSOFT sa,
***     or by anyone to whom The ProgeSOFT sa  has not
***     granted use is illegal.
***
***     Description:
***
*****************************************************************************/
#include "stdafx.h"
#include "IcDynamicInputUIBoxContainer.h"

IcDynamicInputBoxContainer::IcDynamicInputBoxContainer(
        IcDynamicInputUIManager& rUIManager,
        CPoint& rPointContainer,
        IIcDynamicInputManager::ActiveMode& rModeContainer,
        OdStringArray& rOptionList):
    m_lastCursorPos(rPointContainer),
    m_activeMode(rModeContainer),
    m_optionList(rOptionList),
    m_rUIManager(rUIManager)
{
	m_promptBox = nullptr;
	m_modeBox = nullptr;
	m_XBox = nullptr;
	m_YBox = nullptr;
	m_ZBox = nullptr;
	m_RealBox = nullptr;
	m_AngleBox = nullptr;
	m_StringBox = nullptr;
	m_optionsListControl = nullptr;
	m_commandListControl = nullptr;
	m_IntBox = nullptr;
	m_CommandBox = nullptr;

	createFont();

	WNDCLASS wc;
	ZeroMemory(&wc, sizeof(wc));
	wc.lpfnWndProc = AfxWndProc;
	wc.hInstance = AfxGetInstanceHandle();
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.style = CS_VREDRAW | CS_HREDRAW;
	wc.lpszClassName = L"IcDynamicInputClassWnd";
	RegisterClass(&wc);
}

void IcDynamicInputBoxContainer::deleteBox(CWnd* dynamicEditBox)
{
	if (dynamicEditBox)
	{
		if (::IsWindow(dynamicEditBox->GetSafeHwnd()))
		{
			CDynamicInputWnd* parent = dynamic_cast<CDynamicInputWnd*>(dynamicEditBox->GetParent());
			if (parent)
			{
                if (::IsWindow(parent->GetSafeHwnd()))
				    parent->DestroyWindow();
                else
                    dynamicEditBox->DestroyWindow();
				delete parent;
			}
			else
			{
				dynamicEditBox->DestroyWindow();
			}
		}
		delete dynamicEditBox;
	}
}

void IcDynamicInputBoxContainer::updateFont()
{
	createFont();
	if(m_promptBox)
	{
		m_promptBox->SetFont(&m_Font);
	}
	if (m_modeBox)
	{
		m_modeBox->SetFont(&m_Font);
	}
	if (m_XBox)
	{
		m_XBox->SetFont(&m_Font);
	}
	if (m_YBox)
	{
		m_YBox->SetFont(&m_Font);
	}
	if (m_ZBox)
	{
		m_ZBox->SetFont(&m_Font);
	}
	if (m_RealBox)
	{
		m_RealBox->SetFont(&m_Font);
	}
	if (m_IntBox)
	{
		m_IntBox->SetFont(&m_Font);
	}
	if (m_AngleBox)
	{
		m_AngleBox->SetFont(&m_Font);
	}
	if (m_StringBox)
	{
		m_StringBox->SetFont(&m_Font);
	}
	if (m_CommandBox)
	{
		m_CommandBox->SetFont(&m_Font);
	}
	if (m_optionsListControl)
	{
		m_optionsListControl->SetFont(&m_Font);
	}
	if (m_commandListControl)
	{
		m_commandListControl->SetFont(&m_Font);
	}

	if (OdArray<CDynamicEdit*>* dataControls = m_rUIManager.getDataControls())
	{
		for (unsigned int i =0; i< dataControls->length(); i++)
		{
			if (CDynamicEdit * dynControl = dataControls->at(i))
			{
				dynControl->SetFont(&m_Font);
			}
		}
	}
}

void IcDynamicInputBoxContainer::createFont()
{
	m_Font.DeleteObject();
	NONCLIENTMETRICS ncm;
#if(WINVER >= 0x0600)
	ncm.cbSize = sizeof(NONCLIENTMETRICS) - sizeof(ncm.iPaddedBorderWidth);
#else
	ncm.cbSize = sizeof(NONCLIENTMETRICS);
#endif
	if (SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0))
	{
		ncm.lfMenuFont.lfHeight = CDynamicEdit::getFontSize();
		m_Font.CreateFontIndirectW(&(ncm.lfMenuFont));
	}
	else
	{
		CWnd* icadFrame = icedGetIcadFrame();
		HDC pDC = GetDC(icadFrame->m_hWnd);
		int nCharSet = GetTextCharset(pDC);
		int fontSize = CDynamicEdit::getFontSize();
		BOOL res = m_Font.CreateFont(
			fontSize,
			0,						//int nWidth, 0=Reasonable Aspect Ratio
			0,						//int nEscapement,
			0,						//int nOrientation,
			0,						//int nWeight,
			0,						//BYTE bItalic,
			0,						//BYTE bUnderline,
			0,						//BYTE cStrikeOut,
			nCharSet,				//BYTE nCharSet,		 Was ANSI_CHARSET.
			OUT_RASTER_PRECIS,		//BYTE nOutPrecision,
			CLIP_DEFAULT_PRECIS,	//BYTE nClipPrecision,
			DEFAULT_QUALITY,		//BYTE nQuality,
			DEFAULT_PITCH,			//BYTE nPitchAndFamily,	 Was FIXED_PITCH (we cannot use fixed for hieroglyphical strings)
			L"Arial"	//LPCTSTR lpszFacename
		);
		ReleaseDC(icadFrame->m_hWnd, pDC);
	}
}

IcDynamicInputBoxContainer::~IcDynamicInputBoxContainer()
{
	deleteBox(m_promptBox);
	m_promptBox = nullptr;

	deleteBox(m_XBox);
	m_XBox = nullptr;

	deleteBox(m_YBox);
	m_YBox = nullptr;

	deleteBox(m_ZBox);
	m_ZBox = nullptr;

	deleteBox(m_modeBox);
	m_modeBox = nullptr;

	deleteBox(m_RealBox);
	m_RealBox = nullptr;

	deleteBox(m_IntBox);
	m_IntBox = nullptr;

	deleteBox(m_AngleBox);
	m_AngleBox = nullptr;

	deleteBox(m_StringBox);
	m_StringBox = nullptr;

	deleteBox(m_CommandBox);
	m_CommandBox = nullptr;

	deleteBox(m_optionsListControl);
	m_optionsListControl = nullptr;

	deleteBox(m_commandListControl);
	m_commandListControl = nullptr;
}

extern void SetTrasparentWindow(CWnd *pWindow, int perc);

bool IcDynamicInputBoxContainer::createEditBox(int deltax, int deltay, bool enabled, CDynamicEdit *pNewControl)
{
	bool result(false);

	DWORD style = WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | ES_AUTOHSCROLL | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	CWnd *pIcad = (CWnd*)IcadSharedGlobals::GetCMainWindow();

	RECT rect;
	rect.left = m_lastCursorPos.x + deltax;
	rect.top = m_lastCursorPos.y + deltay;
	rect.bottom = rect.top + 10;
	rect.right = rect.left + 30;

	WNDCLASS wc;
	ZeroMemory(&wc, sizeof(wc));
	wc.lpfnWndProc = AfxWndProc;
	wc.hInstance = AfxGetInstanceHandle();
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.style = CS_VREDRAW | CS_HREDRAW;
	wc.lpszClassName = L"IcDynamicInputClassWnd";
	RegisterClass(&wc);

	CDynamicInputWnd *pParent = new CDynamicInputWnd(pIcad);
	if (pParent->CreateEx(WS_EX_NOACTIVATE, L"IcDynamicInputClassWnd", L"", WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, rect, pIcad, 0))
	{
		pParent->SetWindowPos(&CWnd::wndTop, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOREDRAW | SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_NOMOVE);

		if (pNewControl->CreateEx(WS_EX_NOACTIVATE, L"Edit", L"", style, rect, pParent, 1000))
		{
			pNewControl->EnableWindow(enabled);
			pNewControl->SetReadOnly(!enabled);
			pNewControl->SetFont(&m_Font);
			if (enabled)
			{
				pNewControl->SetSel(0, -1);
			}

			pNewControl->setManager(&m_rUIManager);

			int perc = 0;

			icedGetVar(L"TOOLTIPTRANSPARENCY", perc);
			SetTrasparentWindow(pParent, perc);
			SetTrasparentWindow(pNewControl, perc);
			result = true;
		}
		else
		{
			ASSERT(FALSE);
			pParent->DestroyWindow();
			delete pParent;
		}
	}
	else
	{
		ASSERT(FALSE);
		delete pParent;
	}

	return result;
}

CDynamicOptionListBox *IcDynamicInputBoxContainer::createOptionListBox()
{
	DWORD style = WS_VISIBLE | WS_TABSTOP | WS_CHILDWINDOW | LVS_NOCOLUMNHEADER | LVS_NOSCROLL | LVS_SINGLESEL | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_NOLABELWRAP | LVS_OWNERDRAWFIXED;

	CWnd *pIcad = (CWnd*)IcadSharedGlobals::GetCMainWindow();

	int deltax = 10;
	int deltay = 10;
	RECT rect;
	rect.left = m_lastCursorPos.x + deltax;
	rect.top = m_lastCursorPos.y + deltay;
	rect.bottom = rect.top + 50 * m_optionList.size();
	rect.right = rect.left + 30;

	CDynamicListBoxInputWnd *pParent = new CDynamicListBoxInputWnd(pIcad);
	pParent->CreateEx(WS_EX_NOACTIVATE, L"IcDynamicInputClassWnd", L"", WS_POPUP | WS_CLIPSIBLINGS, rect, pIcad, 0);
	pParent->SetWindowPos(&CWnd::wndTop, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOCOPYBITS | SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_NOMOVE);
	pParent->EnableWindow(TRUE);

	rect.left = 0;
	rect.top = 0;
	rect.bottom = 50 * m_optionList.size();
	rect.right = 500;
	CDynamicOptionListBox *pNewControl = new CDynamicOptionListBox();
	BOOL res = pNewControl->CreateEx(WS_EX_NOACTIVATE, style, rect, pParent, 1000);

	LVCOLUMN columnParams;
	columnParams.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	columnParams.fmt = LVCFMT_LEFT;
	columnParams.pszText = L"Name";
	columnParams.iSubItem = -1;
	columnParams.cx = 260;
	pNewControl->InsertColumn(0, &columnParams);
	pNewControl->SetExtendedStyle(pNewControl->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	pNewControl->SetFont(&m_Font);
	pNewControl->setManager(&m_rUIManager);
	pNewControl->EnableWindow(TRUE);

	int perc = 0;

	icedGetVar(L"TOOLTIPTRANSPARENCY", perc);
	SetTrasparentWindow(pParent, perc);
	SetTrasparentWindow(pNewControl, perc);

	return pNewControl;
}

CDynamicCommandListBox* IcDynamicInputBoxContainer::createCommandListBox()
{
	DWORD style = /*WS_VISIBLE |*/ WS_TABSTOP | WS_CHILDWINDOW | LVS_NOCOLUMNHEADER | LVS_SINGLESEL | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_NOLABELWRAP | LVS_OWNERDRAWFIXED;

	CWnd *pIcad = (CWnd*)IcadSharedGlobals::GetCMainWindow();

	int deltax = 10;
	int deltay = 10;
	RECT rect;
	rect.left = m_lastCursorPos.x + deltax;
	rect.top = m_lastCursorPos.y + deltay;
	rect.bottom = rect.top + 250;
	rect.right = rect.left + 300;

	CDynamicCommandListBoxInputWnd *pParent = new CDynamicCommandListBoxInputWnd(pIcad);
	pParent->CreateEx(WS_EX_NOACTIVATE, L"IcDynamicInputClassWnd", L"", WS_POPUP | WS_CLIPSIBLINGS, rect, pIcad, 0);
	pParent->SetWindowPos(&CWnd::wndTop, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOCOPYBITS | SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_NOMOVE);
	pParent->EnableWindow(TRUE);

	rect.left = 0;
	rect.top = 0;
	rect.bottom = 100;
	rect.right = 100;
	CDynamicCommandListBox *pNewControl = new CDynamicCommandListBox();
	BOOL res = pNewControl->CreateEx(WS_EX_NOACTIVATE, style, rect, pParent, 1000);

	LVCOLUMN columnParams;
	columnParams.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	columnParams.fmt = LVCFMT_LEFT;
	columnParams.pszText = L"Name";
	columnParams.iSubItem = -1;
	columnParams.cx = 100;
	pNewControl->InsertColumn(0, &columnParams);
	pNewControl->SetExtendedStyle(pNewControl->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	pNewControl->SetFont(&m_Font);
	pNewControl->setManager(&m_rUIManager);
	pNewControl->EnableWindow(TRUE);
	pNewControl->ShowScrollBar(SB_VERT, true);
	pNewControl->ShowScrollBar(SB_HORZ, false);
	int perc = 0;
	icedGetVar(L"TOOLTIPTRANSPARENCY", perc);
	SetTrasparentWindow(pParent, perc);
	SetTrasparentWindow(pNewControl, perc);

	return pNewControl;
}


void IcDynamicInputBoxContainer::initialize()
{
    getPromptBox(true)->Hide();
    getModeBox(true)->Hide();
    getXBox(true)->Hide();
    getYBox(true)->Hide();
    getZBox(true)->Hide();
    getOptionsListBox(true)->Hide();
    getCommandListBox(true)->Hide();
    getIntBox(true)->Hide();
    getRealBox(true)->Hide();
    getAngleBox(true)->Hide();
    getStringBox(true)->Hide();
    getCommandStringBox(true)->Hide();
}

CDynamicPrompt* IcDynamicInputBoxContainer::getPromptBox(bool create)
{
	if (!m_promptBox && create)
	{
		CDynamicPrompt*control = new CDynamicPrompt();
		if (createEditBox(10, 10, false, control))
		{
			m_promptBox = control;
		}
		else
		{
			deleteBox(control);
		}
	}
	return m_promptBox;
}

CDynamicStringPromptEdit* IcDynamicInputBoxContainer::getModeBox(bool create)
{
	if (!m_modeBox && create)
	{
		CDynamicStringPromptEdit *control = new CDynamicStringPromptEdit();
		if (createEditBox(10, 10, false, control))
		{
			m_modeBox = control;
			m_modeBox->setDisplayDownArrow(false);
		}
		else
		{
			deleteBox(control);
		}
	}
	return m_modeBox;
}

CDynamicEdit* IcDynamicInputBoxContainer::getAngleBox(bool create)
{
	if (!m_AngleBox && create)
	{
		CDynamicAngleEdit *pControl = new CDynamicAngleEdit();
		if (createEditBox(10, 30, true, pControl))
		{
			m_AngleBox = pControl;
			m_AngleBox->setCanSwitchToCoordinate(true);
		}
		else
		{
			deleteBox(pControl);
		}
	}
	return m_AngleBox;
}

CDynamicEdit* IcDynamicInputBoxContainer::getStringBox(bool create)
{
	if (!m_StringBox && create)
	{
		CDynamicStringEdit *pControl = new CDynamicStringEdit();
		if (createEditBox(10, 30, true, pControl))
		{
			m_StringBox = pControl;
			m_StringBox->setCanSwitchToCoordinate(false);
		}
		else
		{
			deleteBox(pControl);
		}
	}

	if (m_StringBox)
	{
		if (m_activeMode == IIcDynamicInputManager::eModeGetStringWithSpace)
			m_StringBox->setEnterMask(CDynamicEdit::eEnterOnEnter);
		else
			m_StringBox->setEnterMask(CDynamicEdit::eEnterOnEnter | CDynamicEdit::eEnterOnSpace);
		m_StringBox->setCanSwitchToCoordinate(false);
		m_StringBox->setSkipMask(CDynamicEdit::eSkipOnTab);
	}
	return m_StringBox;
}

CDynamicEdit* IcDynamicInputBoxContainer::getCommandStringBox(bool create)
{
	if (!m_CommandBox && create)
	{
		CDynamicCommandStringEdit *pControl = new CDynamicCommandStringEdit();
		if (createEditBox(10, 30, true, pControl))
		{
			m_CommandBox = pControl;
			m_CommandBox->setCanSwitchToCoordinate(false);
		}
		else
		{
			deleteBox(pControl);
		}
	}

	if (m_CommandBox)
	{
		if(create)
			m_CommandBox->setSkipMask(0);
		m_CommandBox->setCanSwitchToCoordinate(false);
	}

	return m_CommandBox;
}

CDynamicEdit* IcDynamicInputBoxContainer::getRealBox(bool create)
{
	if (!m_RealBox && create)
	{
		CDynamicRealEdit *pControl = new CDynamicRealEdit();
		if (createEditBox(10, 30, true, pControl))
		{
			m_RealBox = pControl;
		}
		else
		{
			deleteBox(pControl);
		}
	}

	if (m_RealBox)
	{
		m_RealBox->setCanSwitchToCoordinate(m_activeMode == IIcDynamicInputManager::eModeGetDist);
	}

	return m_RealBox;
}

CDynamicEdit* IcDynamicInputBoxContainer::getIntBox(bool create)
{
	if (!m_IntBox && create)
	{
		CDynamicIntEdit *pControl = new CDynamicIntEdit();
		if (createEditBox(10, 30, true, pControl))
		{
			m_IntBox = pControl;
		}
		else
		{
			deleteBox(pControl);
		}
	}
	if (m_IntBox)
	{
		m_IntBox->setCanSwitchToCoordinate(false);
	}
	return m_IntBox;
}

CDynamicEdit* IcDynamicInputBoxContainer::getXBox(bool create)
{
	if (create)
	{
		bool boxCreated = false;
		if (!m_XBox)
		{
			CDynamicRealEdit *pControl = new CDynamicRealEdit();
			if (createEditBox(10, 30, true, pControl))
			{
				m_XBox = pControl;
				boxCreated = true;
			}
			else
			{
				deleteBox(pControl);
			}
		}


		if (m_XBox && (!m_XBox->IsWindowVisible() || boxCreated))
		{
			m_XBox->setSkipMask(CDynamicEdit::eSkipOnTab | CDynamicEdit::eSkipOnComma | CDynamicEdit::eSkipOnMinor);
			m_XBox->setCanSwitchToCoordinate(true);

			// starting from 2nd point of a command this means
			// 0 = relative coords
			// 1 = absolute coords
			int	dynPiCoords, lastPointInit;
			IIcadSysvarManager* pISysvar = getSysvarManagerInterface(getIcadInterfaces());
			int retVal = pISysvar->getVar(L"DYNPICOORDS", dynPiCoords);
			retVal = pISysvar->getVar(L"LASTPOINTINIT", lastPointInit);
			m_XBox->setIsRelative(lastPointInit == 1 && dynPiCoords == DynInputValues::KRelative);
			CDynamicStringPromptEdit* modeBox = getModeBox(true);
			if (modeBox)
			{
				modeBox->setValue(m_XBox->isRelative() ? L"@" : L"#");
			}
		}
	}
	return m_XBox;
}

CDynamicEdit* IcDynamicInputBoxContainer::getYBox(bool create)
{
	if (create)
	{
		bool boxCreated = false;
		if (!m_YBox)
		{
			CDynamicRealEdit *pControl = new CDynamicRealEdit();
			if (createEditBox(10, 60, false, pControl))
			{
				m_YBox = pControl;
				boxCreated = true;
			}
			else
			{
				deleteBox(pControl);
			}
		}

		if (m_YBox && (!m_YBox->IsWindowVisible() || boxCreated))
		{
			m_YBox->setAcceptMask(CDynamicEdit::eAcceptMinor | CDynamicEdit::eAcceptDot | CDynamicEdit::eAcceptNumbers);
			m_YBox->setSkipMask(CDynamicEdit::eSkipOnTab | CDynamicEdit::eSkipOnComma);
			m_YBox->setCanSwitchToCoordinate(false);
			// starting from 2nd point of a command this means
			// 0 = polar coords
			// 1 = cartesian coords
			int	dynPiFormat(0), lastPointInit(0);
			IIcadSysvarManager* pISysvar = getSysvarManagerInterface(getIcadInterfaces());
			pISysvar->getVar(L"DYNPIFORMAT", dynPiFormat);
			pISysvar->getVar(L"LASTPOINTINIT", lastPointInit);
			m_YBox->setIsAngular(lastPointInit == 1 && dynPiFormat == DynInputValues::KPolar);
		}
	}
	return m_YBox;
}

CDynamicEdit* IcDynamicInputBoxContainer::getZBox(bool create)
{
	if (create)
	{
		bool boxCreated = false;
		if (!m_ZBox)
		{
			CDynamicRealEdit *pControl = new CDynamicRealEdit();
			if (createEditBox(10, 90, false, pControl))
			{
				m_ZBox = pControl;
				boxCreated = true;
			}
			else
			{
				deleteBox(pControl);
			}
		}

		if (m_ZBox && (!m_ZBox->IsWindowVisible() || boxCreated))
		{
			m_ZBox->setAcceptMask(CDynamicEdit::eAcceptMinor | CDynamicEdit::eAcceptDot | CDynamicEdit::eAcceptNumbers);
			m_ZBox->setSkipMask(CDynamicEdit::eSkipOnTab);
			m_ZBox->setCanSwitchToCoordinate(false);
		}

		m_rUIManager.resizeControls();
	}
	return m_ZBox;
}

CDynamicOptionListBox *IcDynamicInputBoxContainer::getOptionsListBox(bool create)
{
	if (!m_optionsListControl && create)
	{
		//m_optionsListControl = new CDynamicOptionListBox();
		m_optionsListControl = createOptionListBox();
	}
	return m_optionsListControl;
}

CDynamicCommandListBox* IcDynamicInputBoxContainer::getCommandListBox(bool create)
{
	if (!m_commandListControl && create)
	{
		m_commandListControl = createCommandListBox();
	}
	return m_commandListControl;
}
