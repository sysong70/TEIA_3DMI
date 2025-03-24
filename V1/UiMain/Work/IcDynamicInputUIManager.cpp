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
#include "Icad/Configure.h"
#include "IcDynamicInputUIManager.h"
#include "IcadCommon/Utilities/IcadPreferences.h"
#include "IcadCommon\Interfaces\IIcadIOManager.h"
#include "IcadCommon\utilities\IcadInterfaceUtils.h"
#include "IcadCommon\IcadCommonUtils.h"
#include "IcadCommon/Utilities/IcTransforms.h"

#include "IcadControlFlow\IcadEngineScheduler\IcadEngineScheduler.h"
#include "IcadControlFlow\IcadEngineScheduler\IcadControlFlowContext.h"
#include "../Dialogs/IcadCommandBar/CommandAutocompleteDef.h"

#include "DbAlignedDimension.h"
#include "Db3PointAngularDimension.h"
#include "DbArcDimension.h"
#include "Ge/GeCircArc3d.h"
#include "Ge/GeLine3d.h"
#include "DbGrip.h"

#include "IcadCommon/Interfaces/IIcadCmdQueuesInternal.h"
#include "IcadCommon/Interfaces/IIcadCmdQueueInternal.h"
#include "api\icrx\inc\IcEdCommand.h"
#include "IcDynamicInputUtils.h"
#include <algorithm>
#include "IcadCore/System/IcadCmdHistoryManager.h"
#include "IcadCore/System/IcadCmdHistory.h"
#include "IcadCore/Commands/IcEdNativeCommand.h"
#include "IcadCore/DragJigs/IIcadDragger.h"
#include "IcadCore/DragJigs/IcEdBaseJig.h"

#include "CommandGUI/Dialogs/IcadCommandBar/IcadCommandBar.h"
#include "CommandGUI/Dialogs/IcadCommandBar/IcadCommandLine.h"
#include "CommandGUI/Dialogs/IcadCommandBar/IcadCommandHistory.h"
#include "CommandGUI/Dialogs/IcadCommandBar/IcadCommandWindow.h"

#include "IcadCore/GripEdit/IcGripDataPoint.h"
#include "IcadCore/ExternalApplications/ExternalApplication.h"

#define SPACE_BETWEEN_EDIT 5
#define OFFSET_Y_OPTION_LIST 15

OdGsDCPoint wcs2Screen(OdGePoint3d point, OdGeMatrix3d mtx)
{
	point.transformBy(mtx);
	// in uncertain situation transformation returns bad values to avoid crash
	if ((point.x < double(LONG_MIN)) || (point.x > double(LONG_MAX)) || (point.y < double(LONG_MIN)) || (point.y > double(LONG_MAX)))
	{
		ASSERT(FALSE);
		return OdGsDCPoint(0, 0);
	}
	return OdGsDCPoint(OdRoundToLong(point.x), OdRoundToLong(point.y));
}

UINT vkToChar(UINT code, UINT flags)
{
	WCHAR keybuff[256] = {0};
	int ret = 0;
	WORD ch;
	BYTE kbs[256];
	GetKeyboardState(kbs);
	HKL keyboardlayout = GetKeyboardLayout(0);
	UINT scanCode = flags & 0xffff;
	ret = ToUnicodeEx(code, scanCode, (PBYTE)&kbs, (LPWSTR)&keybuff, sizeof(keybuff) / 16, 0, keyboardlayout);
	if (ret > 0)
		ch = keybuff[0];
	if (ret == 0)
		ch = 0;
	return ch;
}

#define _LOCK() CriticalSection::Lock _lock(m_criticalSection);

IcDynamicInputUIManager::IcDynamicInputUIManager() :
	m_BoxContainer(*this, m_lastCursorPos, m_activeMode, m_optionList),
	m_pCurrentJig(nullptr),
	m_Drawer(nullptr),
	m_DimData(nullptr),
	m_state(nullptr),
	m_isOnlyKeywords(false)
{
	m_enableControlsPosition = false;
	m_activeMode = IIcDynamicInputManager::eModeNone;
	m_bUpdateRequired = false;
	m_oldIsDisplayArrow = false;

	NONCLIENTMETRICS ncm;
#if (WINVER >= 0x0600)
	ncm.cbSize = sizeof(NONCLIENTMETRICS) - sizeof(ncm.iPaddedBorderWidth);
#else
	ncm.cbSize = sizeof(NONCLIENTMETRICS);
#endif


	WNDCLASS wc;
	ZeroMemory(&wc, sizeof(wc));
	wc.lpfnWndProc = AfxWndProc;
	wc.hInstance = AfxGetInstanceHandle();
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.style = CS_VREDRAW | CS_HREDRAW;
	wc.lpszClassName = L"IcDynamicInputClassWnd";
	RegisterClass(&wc);

	m_ActiveControl = nullptr;
	setMode(IIcDynamicInputManager::eModeNotActive);
	//::InitializeCriticalSection( &m_criticalSection );
}

IcDynamicInputUIManager::~IcDynamicInputUIManager()
{
	if (m_Drawer)
	{
		delete m_Drawer;
		m_Drawer = nullptr;
	}
}

void IcDynamicInputUIManager::initialize()
{
	m_BoxContainer.initialize();
}

bool IcDynamicInputUIManager::validate()
{
	if(getActiveControl())
	{
		CString controlText;
		getActiveControl()->GetWindowText(controlText);
		if(getActiveControl()->isKeyword(controlText.GetBuffer()))
		{
			return true;
		}
	}

	if (m_activeMode == IIcDynamicInputManager::eModeCoordinate)
	{
		if (m_BoxContainer.getXBox() && !m_BoxContainer.getXBox()->validate())
		{
			return false;
		}
		if (m_BoxContainer.getYBox() && !m_BoxContainer.getYBox()->validate())
		{
			return false;
		}
		if (m_BoxContainer.getZBox() && !m_BoxContainer.getZBox()->validate())
		{
			return false;
		}
	}
	return true;
}

void IcDynamicInputUIManager::resetValues()
{
	if (m_state)
		m_state->resetValues(this);
	// disable current active control
	disableActiveControl();
}

void IcDynamicInputUIManager::deactivate(bool bClearState)
{
	if (m_state)
		m_state->deactivate(this, bClearState);

	if (m_BoxContainer.getOptionsListBox() && m_BoxContainer.getOptionsListBox(false)->m_hWnd)
	{
		if (m_BoxContainer.getOptionsListBox(false)->GetParent() && m_BoxContainer.getOptionsListBox(false)->GetParent()->m_hWnd)
		{
			destroyOptionList();
		}
	}

	if (m_BoxContainer.getCommandListBox() && m_BoxContainer.getCommandListBox(false)->m_hWnd)
	{
		if (m_BoxContainer.getCommandListBox(false)->GetParent() && m_BoxContainer.getCommandListBox(false)->GetParent()->m_hWnd)
		{
			destroyCommandList();
		}
	}

	disableActiveControl();
}

void IcDynamicInputUIManager::activate()
{
	if (m_state)
		m_state->activate(this);

	if (m_bUpdateRequired)
	{
		OdGePoint3d dummypt;
		onMouseMove(dummypt);
		m_bUpdateRequired = false;
	}
}

int IcDynamicInputUIManager::countDimData()
{
	int count = 0;
	if (m_DimData)
		count = m_DimData->length();

	return count;
}

bool IcDynamicInputUIManager::isDimVisible(OdDbDimData* dimData)
{
	if (dimData->isDimInvisible())
		return false;

	int dyndivis = 0;
	icedGetVar(L"DYNDIVIS", dyndivis);

	int dyndigrip = 0;
	icedGetVar(L"DYNDIGRIP", dyndigrip);

	if (/*!m_pAppDataMap.empty() && */dyndivis == DynInputValues::KAllDynamicDimensions && dimData->isDimEditable())
	{
		if (dyndigrip & DynInputValues::KResultingDimension)
		{
			if (dimData->isDimResultantLength())
				return true;
		}

		if (dyndigrip & DynInputValues::KLengthChangeDimension)
		{
			if (dimData->isDimDeltaLength())
				return true;
		}
		if (dyndigrip & DynInputValues::KAbsoluteAngleDimension)
		{
			if (dimData->isDimResultantAngle())
				return true;
		}
		if (dyndigrip & DynInputValues::KAngleChangeDimension)
		{
			if (dimData->isDimDeltaAngle())
				return true;
		}
		if (dyndigrip & DynInputValues::KArcRadiusDimension)
		{
			if (dimData->isDimRadius())
				return true;
		}

		return false;
	}

	return true;
}

void IcDynamicInputUIManager::setDimData(OdArray<OdDbDimData*>* dimData)
{
	m_DimData = dimData;
	/*if (m_pDataControls.size()>0)
	{
		disableActiveControl();
		for (int i = 0; i != m_pDataControls.size(); i++)
		{
			m_pDataControls[i]->Hide();
			m_pDataControls[i]->setDimData(nullptr);
		}
	}*/

	if (m_DimData && m_DimData->size() > 0)
	{
		setMode(IIcDynamicInputManager::eModeDimensions);

	}
}

OdArray<OdDbDimData*>* IcDynamicInputUIManager::getDimData()
{
	return m_DimData;
}

bool IcDynamicInputUIManager::onLButtonDown(CPoint dcPoint)
{
	_LOCK();
	m_lastCursorPos = dcPoint;

	resizeControls();

	// reset values and status of controls
	resetValues();
	if (m_BoxContainer.getCommandStringBox() && m_BoxContainer.getCommandStringBox()->IsWindowVisible())
		m_BoxContainer.getCommandStringBox()->Hide();
	// destroy options list
	destroyOptionList();
	destroyCommandList();
	return true;
}

bool IcDynamicInputUIManager::onRButtonDown(OdGsViewPtr spView, CPoint& dcPoint)
{
	if (!getActiveControl() || !getActiveControl()->IsWindowVisible())
		return false;
	// validate values in edit box
	if (!validate())
		return true;

	CString value;
	bool sendEnter;
	if (getCommand(sendEnter, value))
	{
		if (value.IsEmpty())
		{
			return false;  // does nothing in icadview OnRButtonUp and proceed normally
		}
		else
		{
			IIcadIOManager* pIO = getIOManagerInterface(getIcadInterfaces());
			for (int i = 0; i != value.GetLength(); i++)
			{
				pIO->sendMessage(WM_CHAR, value[i], 0);
			}
			pIO->sendMessage(WM_CHAR, 13, 0);  // enter
			destroyOptionList();
			resetValues();
			return true;
		}
	}
	return false;
}

bool IcDynamicInputUIManager::onKeyboardMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, bool isLispActive)
{
	IIcadIOManager* pIO = getIOManagerInterface(getIcadInterfaces());
	int ui = pIO->getUserInputControls();
	IIcadSysvarManager* pISysvar = getSysvarManagerInterface(getIcadInterfaces());
	ASSERT(pISysvar != NULL);

	int dynPrompt = 0;
	int retVal = pISysvar->getVar(L"DYNPROMPT", dynPrompt);
	int dynMode = DynInputValues::KPointerAndDimInputOff;
	pISysvar->getVar(L"DYNMODE", dynMode);

	if (dynMode == DynInputValues::KDimInputOn && m_activeMode == IIcDynamicInputManager::eModeNotActive)
		return false;

	if (!getActiveControl() && m_activeMode == IIcDynamicInputManager::eModeNotActive)
	{
		CDynamicEdit* commandBox = m_BoxContainer.getCommandStringBox(true);
		if(commandBox)commandBox->Hide();
		setActiveControl(commandBox);
	}

	if (!getActiveControl() &&
		m_activeMode == IIcDynamicInputManager::eModeDimensions &&
		DynInputUIDimensionsState::getInstance()->isHoverGrip())
	{
		endGrip();
	}

	if (m_BoxContainer.getOptionsListBox() && m_BoxContainer.getOptionsListBox()->MessageHandler(message, wParam, lParam))
		return true;

	if (m_BoxContainer.getCommandListBox() && m_BoxContainer.getCommandListBox()->MessageHandler(message, wParam, lParam))
		return true;

	if (m_activeMode == IIcDynamicInputManager::eModeNotActive && dynPrompt)
	{
		bool isCtrlPressed = HIWORD(GetKeyState(VK_CONTROL)) > 0;

		if (message == WM_KEYDOWN &&
			!isCtrlPressed &&
			CIcadPreferenceManager::ArrowKeysForHistNavPreference &&
			(wParam == VK_UP || wParam == VK_DOWN))
		{
			IcString str = (wParam == VK_UP) ? icadCurrCmdHistory()->getPrevCommand() : icadCurrCmdHistory()->getNextCommand();
			if (!str.isEmpty())
			{
				CDynamicEdit* commandBox = m_BoxContainer.getCommandStringBox(true);
				commandBox->EnableWindow(true);
				commandBox->SetReadOnly(false);
				commandBox->setValue(str);
				commandBox->postPoneShow();
				getActiveControl()->setStatus(CDynamicEdit::eStateEditing);
				getActiveControl()->SetSel(str.getLength(), str.getLength());
				resizeControls();
			}
			else
			{
				if(wParam == VK_DOWN)
				{
					if(m_BoxContainer.getCommandStringBox()->IsWindowVisible())
					{
						m_BoxContainer.getCommandStringBox()->setValue(L"");
						m_BoxContainer.getCommandStringBox()->Hide();
					}
					else
					{
						if (auto pMain = IcadSharedGlobals::GetCMainWindow())
						{
							if (auto pBar = pMain->getCommandBar())
							{
								pBar->SetFocus();
							}
						}
					}


					return false;
				}
			}
			return true;
		}
	}

	if (getActiveControl())
	{
		if (wParam == VK_ESCAPE)
		{
			if ((m_activeMode != IIcDynamicInputManager::eModeDimensions) ||
				(getActiveControl()->getStatus() == CDynamicEdit::eStateUnchanged) ||
				(getActiveControl()->getStatus() == CDynamicEdit::eStateModified))
			{
				getActiveControl()->setStatus(CDynamicEdit::eStateUnchanged);
				if (m_activeMode == IIcDynamicInputManager::eModeNotActive)
				{
					if (m_BoxContainer.getCommandStringBox() && m_BoxContainer.getCommandStringBox()->IsWindowVisible())
						m_BoxContainer.getCommandStringBox()->Hide();

					if (m_BoxContainer.getCommandListBox())
					{
						destroyCommandList();
					}
				}

				return false;
			}

			// I need to reset state of active control only on KEYUP to avoid ESC handled by command line due to previous line
			if (message == WM_KEYUP)
			{
				getActiveControl()->setStatus(CDynamicEdit::eStateUnchanged);
				// to display change suddenly
				updateDimensionValue();
			}
			return true;
		}

		if ((message == WM_KEYDOWN) || (message == WM_CHAR))
		{
			wchar_t ch = (wchar_t)wParam;
			CDynamicEdit* pActiveControl = getActiveControl();

			if (!pActiveControl)
			{
				return false;
			}

			if (wParam == VK_PROCESSKEY)
				return true;

			if ((message == WM_KEYDOWN && wParam >= VK_F1 && wParam < VK_F24) || wParam == VK_SHIFT || wParam == VK_CONTROL)
				return false;
			// manage CTRL + Z
			else if (HIWORD(GetKeyState(VK_CONTROL)) > 0 && HIWORD(GetKeyState(VK_MENU)) <= 0)
				return false;
			else if (wParam == VK_DOWN && message != WM_CHAR)
			{
				return displayOptionList(ui);
			}
			// to write '@' or '#' also in text command
			else if ((pActiveControl->canSwitchToCoordinate() || m_activeMode == IIcDynamicInputManager::eModeCoordinate)
				&& ((ch == '@') || (ch == '#')))
			{
				CString oldValue;
				if (pActiveControl->canSwitchToCoordinate())
				{
					// transfers typed value from dimension control to xcoordinate box
					if (pActiveControl->getStatus() == CDynamicEdit::eStateEditing ||
						pActiveControl->IsModified())
					{
						pActiveControl->GetWindowTextW(oldValue);
					}
					setMode(IIcDynamicInputManager::eModeCoordinate);
				}

				if (m_activeMode == IIcDynamicInputManager::eModeCoordinate)
				{
					if(dynMode & DynInputValues::KPointerInputOn)
					{
						m_BoxContainer.getXBox(true)->setMode(ch);

						// when @ or # try to change mode,
						// if control can change mode then it's not displayed
						if (auto modeBox = m_BoxContainer.getModeBox())
						{
							modeBox->setValue(m_BoxContainer.getXBox()->isRelative() ? L"@" : L"#");
							modeBox->setStatus(CDynamicEdit::eStateModified);
							if (!modeBox->IsWindowVisible())
							{
								modeBox->Show();
							}
						}

						// changing to coordinate mode I need to get typed value and put in X coordinate box
						if (!oldValue.IsEmpty() && m_BoxContainer.getXBox())
						{
							m_BoxContainer.getXBox()->SetWindowTextW(oldValue);
							m_BoxContainer.getXBox()->setStatus(CDynamicEdit::eStateModified);
						}
					}
					else
					{
						return false;
					}

					// to display change suddenly

					if (dynMode > DynInputValues::KPointerAndDimInputOff)
					{
						MSG msg;
						CWnd* icadFrame = icedGetIcadDwgView();
						if (!PeekMessage(&msg, icadFrame->m_hWnd, WM_MOUSEFIRST, WM_MOUSELAST, PM_NOYIELD))
						{
							OdGePoint3d dummyPt;
							onMouseMove(dummyPt);
						}
					}
					return true;
				}
			}
			else if (pActiveControl->isGoToNextEditChar(ch))
			{
				if (wParam == VK_TAB)
				{
					if (IcadBaseDocInstance* pActiveView = IcCoreExternal::getCurrentView())
					{
						CIcadDwgView* pActiveDWGView = dynamic_cast<CIcadDwgView*>(pActiveView);
						if (pActiveDWGView)
							pActiveDWGView->processOsnapChar((TCHAR)wParam);
					}

					if (m_BoxContainer.getOptionsListBox())
					{
						destroyOptionList();
					}
				}

				CString oldValue;
				bool isSwitched = false;
				if ((ch == L',' || ch == L'<') &&
					pActiveControl->canSwitchToCoordinate() &&
					pActiveControl->validate(false))
				{
					// transfers typed value from dimension control to xcoordinate box
					if (pActiveControl->getStatus() == CDynamicEdit::eStateEditing ||
						pActiveControl->IsModified())
					{
						pActiveControl->GetWindowTextW(oldValue);
					}

					setMode(IIcDynamicInputManager::eModeCoordinate);
					isSwitched = true;

					int dynPivis = 0;
					retVal = pISysvar->getVar(L"DYNPIVIS", dynPivis);
					if(dynPivis == DynInputValues::KOnlyWhenYouTypeAtPromptForPoint)
					{
						showCoordinateBoxes();
					}

					if (!(dynMode&DynInputValues::KPointerInputOn))
					{
						if (auto pMain = getMainInternalInterface(getIcadInterfaces()))
						{
							pMain->getCommandBar()->GetPromptWindow()->CommandLineAddText(oldValue);
						}
						m_BoxContainer.getModeBox()->Hide();
						return false;
					}
				}

				if (!isSwitched || !oldValue.IsEmpty())
					changeActiveControl((wParam != VK_TAB));
				else
				{
					setActiveControl(m_BoxContainer.getXBox());
					m_BoxContainer.getYBox()->Hide();
					m_BoxContainer.getModeBox()->Hide();
					m_BoxContainer.getXBox()->setStatus(CDynamicEdit::eStateEditing);
					sendKey(wParam, lParam, (message == WM_CHAR), isLispActive);
				}

				if (ch == L'<' && !oldValue.IsEmpty())	// switch to relative mode
				{
					if (getActiveControl())
						getActiveControl()->setIsAngular(true);

					// changing to coordinate mode I need to get typed value and put in X coordinate box
					if (!oldValue.IsEmpty() && m_BoxContainer.getXBox())
					{
						m_BoxContainer.getXBox()->SetWindowTextW(oldValue);
						m_BoxContainer.getXBox()->setStatus(CDynamicEdit::eStateUnchanged);
					}
				}

				if (ch == L',')	 // switch to absolute mode
				{
					if (getActiveControl())
						getActiveControl()->setIsAngular(false);
				}
				// changing to coordinate mode I need to get typed value and put in X coordinate box
				if (!oldValue.IsEmpty() && m_BoxContainer.getXBox())
				{
					m_BoxContainer.getXBox()->SetWindowTextW(oldValue);
					m_BoxContainer.getXBox()->setStatus(CDynamicEdit::eStateModified);
				}
				//sendMouseMove();

				if (!getActiveControl() || !getActiveControl()->IsWindowVisible() || !getActiveControl()->IsWindowEnabled())
				{
					return false;
				}

				return true;
			}
			else if (pActiveControl->isEnterChar(ch))
			{
				if (getMode() == IIcDynamicInputManager::eModeCoordinate &&
					!m_BoxContainer.getYBox()->IsWindowVisible() &&
					m_BoxContainer.getXBox()->IsWindowVisible())
				{
					CString value;
					m_BoxContainer.getXBox()->GetWindowTextW(value);
					if (value.GetLength() > 0 && value[0] == '<')
					{
						m_BoxContainer.getYBox()->Show();
						value.Delete(0, 1);
						m_BoxContainer.getYBox()->SetWindowTextW(value);
						m_BoxContainer.getYBox()->setIsAngular(true);
						if (m_BoxContainer.getYBox()->validate())
						{
							m_BoxContainer.getYBox()->setStatus(CDynamicEdit::eStateModified);
						}
						else
						{
							m_BoxContainer.getYBox()->setStatus(CDynamicEdit::eStateUnchanged);
						}
						m_BoxContainer.getXBox()->setStatus(CDynamicEdit::eStateUnchanged);
						setActiveControl(m_BoxContainer.getXBox());
						resizeControls();
						sendMouseMove();
					}
					return true;
				}
				// validate values in edit box
				//if (!validate())
				//	return true;

				if (getActiveControl() &&
					getActiveControl()->m_hWnd &&
					getActiveControl()->IsWindowVisible() &&
					getActiveControl()->getStatus() != CDynamicEdit::eStateUnchanged)
				{
					CString controlText;
					getActiveControl()->GetWindowTextW(controlText);
					if (controlText.IsEmpty())
					{
						getActiveControl()->setStatus(CDynamicEdit::eStateUnchanged);
						// to display change suddenly
						updateDimensionValue();
						return true;
					}
				}

				CString command;
				bool blockMessage;	// block this keyboard message
				bool sendEnter;		//
				blockMessage = getCommand(sendEnter, command);

				// select string in active edit box so if command line reject this command it's selected to easyly correct
				if (getActiveControl())
				{
					getActiveControl()->SetSel(0, -1);
					getActiveControl()->Invalidate(TRUE);
				}

				// for Bug 110880 - [Dynamic input] Keywords are incorrectly executed in some cases.
				if (IcadSharedGlobals::GetCMainWindow()->IsShiftDown() && (::GetKeyState(VK_SHIFT) & 0x8000) <= 0 )
					IcadSharedGlobals::GetCMainWindow()->SetIsShiftDown(false);

				if (sendEnter)
				{
					// clean before send enter to avoid problems after command is sended
					destroyOptionList();
					resetValues();

					pIO->sendMessage(WM_MENUSELECT, 0, (LPARAM)L"");
				}
				else if (!command.IsEmpty())
				{
					for (int i = 0; i != command.GetLength(); i++)
					{
						pIO->sendMessage(WM_CHAR, command[i], 0);
					}
					// clean before send enter to avoid problems after command is sended
					destroyOptionList();
					resetValues();

					pIO->sendMessage(WM_CHAR, 13, 0);  // enter

				}
				else if (!blockMessage)
				{
					resetValues();
				}

				setUpdateRequired();
				return blockMessage;
			}
			else
			{
				// in autocad I can write all chars
				// on enter command line rejects invalid values
				// if ( pActiveControl->isAcceptedChar( ch ))
				//{
				return sendKey(wParam, lParam, (message == WM_CHAR), isLispActive);
				//}
				// return true;
			}
		}
	}
	else if (((message == WM_KEYDOWN) || (message == WM_CHAR)) && wParam == VK_DOWN)
	{
		return displayOptionList(ui);
	}

	return false;
}

bool IcDynamicInputUIManager::onMouseMove(const CPoint dcPoint, OdGePoint3d ucsPoint, OdGePoint3d &ucsModifiedPoint)
{
	if (IcadSharedGlobals::IsMainThreadCurrent())
	{
		if (!(getActiveControl() && getActiveControl()->getStatus() == CDynamicEdit::State::eStateEditing))
		{
			m_lastCursorPos = dcPoint;
			setLastUcsPoint(ucsPoint, false);
		}

		return onMouseMove(ucsModifiedPoint);
	}
	else
	{
		postMouseMove(dcPoint.x + 1, dcPoint.y);
		return false;
	}
}

bool IcDynamicInputUIManager::onMouseMove(OdGePoint3d& ucsModifiedPoint)
{
	_LOCK()
	bool ret = false;

	IIcadSysvarManager* pISysvar = getSysvarManagerInterface(getIcadInterfaces());
	ASSERT(pISysvar != NULL);

	int dynMode = DynInputValues::KPointerAndDimInputOff;
	int retVal = pISysvar->getVar(L"DYNMODE", dynMode);

	if (!IcadSharedGlobals::IsMainThreadCurrent())
	{
		CWnd* icadFrame = icedGetIcadDwgView();
		// only main thread can create edit controls, simulate MouseMove message
		// send to main thread mouse move message (point must be different to avoid discarge of message)
		icadFrame->PostMessage(WM_MOUSEMOVE, 0, MAKELPARAM(m_lastCursorPos.x + 1, m_lastCursorPos.y));
		return false;
	}

	// disable control reposition fired by EN_CHANGE events on edit box
	m_enableControlsPosition = false;

	// set prompt box
	int dynPrompt = 0;
	retVal = pISysvar->getVar(L"DYNPROMPT", dynPrompt);

	bool isHoverGrip = m_activeMode == IIcDynamicInputManager::eModeDimensions && !m_pAppDataMap.empty() &&
					   DynInputUIDimensionsState::getInstance()->isHoverGrip();

	if (dynPrompt && (dynMode == DynInputValues::KDimInputOn && (!m_DimData || m_activeMode != IIcDynamicInputManager::eModeDimensions)) )
	{
		dynPrompt = false;
	}

	if (dynPrompt && (m_activeMode != IIcDynamicInputManager::eModeNotActive && !isHoverGrip))
	{
		if (CDynamicPrompt* promptBox = m_BoxContainer.getPromptBox(true))
		{
			promptBox->OnTimer(CDynamicPrompt::kTimerId);
			promptBox->postPoneShow();
		}
	}

	if (!dynPrompt)
	{
		CDynamicEdit* promptBox = m_BoxContainer.getPromptBox(true);
		if (promptBox)
		{
			promptBox->Hide();
		}
	}

	IIcadIOManager* pIO = getIOManagerInterface(getIcadInterfaces());
	int userInputControls = pIO->getUserInputControls();

	// sometimes message that disable active control come after enable so active control is disabled
	if (getActiveControl() && getActiveControl()->IsWindowVisible() && !getActiveControl()->IsWindowEnabled())
	{
		getActiveControl()->EnableWindow();
		getActiveControl()->SetReadOnly(false);
	}

	if (m_state && dynMode > 0 && (dynMode & DynInputValues::KPointerInputOn))
		ret = m_state->onMouseMove(this);

	if (ret)
	{
		ucsModifiedPoint = getLastUcsPoint();
	}

	m_enableControlsPosition = true;

	// don't do it twice when dimensions are displayed
	if (!((m_activeMode == IIcDynamicInputManager::eModeDimensions) && (dynMode & DynInputValues::KDimInputOn)))
	{
		if (!getActiveControl())
			changeActiveControl(false);
		if (getActiveControl() && getActiveControl()->IsUnchanged())
			getActiveControl()->SetSel(0, -1);

		resizeControls();
	}
	else
	{
		if (getActiveControl() && getActiveControl()->IsUnchanged())
			getActiveControl()->SetSel(0, -1);
	}

	return ret;
}

void IcDynamicInputUIManager::updateDimensionValue()
{
	if (getDrawer())
	{
		getDrawer()->getMyDrawList()->invalidateListToScreen();
		getDrawer()->updateScreen();
	}

	IIcadSysvarManager* pISysvar = getSysvarManagerInterface(getIcadInterfaces());
	ASSERT(pISysvar != NULL);

	int dynMode;
	int retVal = pISysvar->getVar(L"DYNMODE", dynMode);

	IIcadIOManager* pIO = getIOManagerInterface(getIcadInterfaces());
	int userInputControls = pIO->getUserInputControls();

	if ((m_activeMode == IIcDynamicInputManager::eModeGetDist) &&
		(dynMode & DynInputValues::KDimInputOn) &&
		getActiveControl() &&
		getActiveControl()->GetWindowTextLengthW() == 0)
	{
		m_state->onMouseMove(this);
		getActiveControl()->SetSel(0, -1);
	}
	// this part is moved from mouse move to updateDimensionValue
	// because onMouseMove is called before jig update so values displyed one step behind of real mouse position
	if ((m_activeMode == IIcDynamicInputManager::eModeDimensions) && (dynMode & DynInputValues::KDimInputOn))
	{
		m_enableControlsPosition = false;
		// draw dimensions
		// check current jig if dimData doesn't return NULL update dynamic dimensions
		if (m_pDataControls.size() > 0)
		{
			_LOCK();
			int dyndivis = 0;
			icedGetVar(L"DYNDIVIS", dyndivis);
			int countDim = 0;
			for (int i = 0; i != m_pDataControls.size(); i++)
			{
				OdDbDimData* dimData = nullptr;
				if (m_pDataControls[i])
					dimData = m_pDataControls[i]->dimData();

				if (dimData)
				{
					OdDbDimensionPtr pDim = dimData->dimension();
					bool isVisible = isDimVisible(dimData);
					if (/*!m_pAppDataMap.empty() && */dimData->isDimEditable() && ((dyndivis == DynInputValues::KOnlyFirstDynamicDimension && countDim > 0) ||
						(dyndivis == DynInputValues::KOnlyFirstTwoDynamicDimensions && countDim > 1)))
					{
						isVisible = false;
					}

					if (isVisible)
					{
						countDim++;
					}

					if (!pDim.isNull() && m_pDataControls[i]->IsUnchanged())
					{
						if (isVisible)
						{
							OdString dimensionText = pDim->dimensionText();
							if (pDim->dimensionText() != L"<>" && !dimensionText.trimLeft().isEmpty())
							{
								OdString formattedMeasurement = pDim->dimensionText();
								IcString sMeasurement;
								IIcadServicesManager* pServMgr = getServicesManagerInterface(getIcadInterfaces());
								pServMgr->realToString(pDim->getMeasurement(), -1, -1, sMeasurement);
								formattedMeasurement.replace(L"<>", (LPCTSTR)sMeasurement);
								m_pDataControls[i]->setValue(formattedMeasurement); // use default measurement
							}
							else
							{
								m_pDataControls[i]->setValue(pDim->getMeasurement()); // use default measurement
							}

							pDim->setVisibility(OdDb::Visibility::kVisible);
						}
						else
						{
							pDim->setVisibility(OdDb::Visibility::kInvisible);
						}
					}

					if (!isVisible)
					{
						m_pDataControls[i]->Hide();
					}
					else
					{
						m_pDataControls[i]->setAcceptNull(!(userInputControls & IcUserInputControls::kNullResponseIsNotAccepted));
						m_pDataControls[i]->Show();
					}
				}
			}

			if (m_pDataControls[0] && countDim == 1 && !m_defaultValue.isEmpty())
			{
				// sets default also for dimension data,
				// don't set if it displays a dimension
				if (OdDbDimData* dimData = m_pDataControls[0]->dimData())
				{
					OdDbDimensionPtr pDim = dimData->dimension();
					if (pDim.isNull() && m_pDataControls[0]->IsUnchanged())
					{
						m_pDataControls[0]->SetWindowTextW(m_defaultValue);
					}
				}
			}
		}
		if (!getActiveControl())
			changeActiveControl(false);
		if (getActiveControl() && getActiveControl()->IsUnchanged())
			getActiveControl()->SetSel(0, -1);

		m_enableControlsPosition = true;
		resizeControls();
	}
}

OdGePoint3d IcDynamicInputUIManager::getLastUcsPoint()
{
	return IcWcsToUcs(m_lastWcsPoint);
}

OdGePoint3d IcDynamicInputUIManager::getLastWcsPoint() const
{
	return m_lastWcsPoint;
}

void IcDynamicInputUIManager::setLastUcsPoint(const OdGePoint3d point, bool bSetModified /*=true*/)
{
	m_bModified = false;
	m_lastWcsPoint = IcUcsToWcs(point);

	if (bSetModified)
		m_bModified = true;
}

extern void SetTrasparentWindow(CWnd* pWindow, int perc);


void IcDynamicInputUIManager::getOptionListAndDefault(OdString& text, bool threadRedir)
{
	IIcadSysvarManager* pISysvar = getSysvarManagerInterface(getIcadInterfaces());
	ASSERT(pISysvar != NULL);

	m_optionList.clear();
	m_defaultValue = L"";
	if (m_BoxContainer.getOptionsListBox())
	{
		destroyOptionList();
	}

	int dynOptionList;
	int retVal = pISysvar->getVar(k_IC_RDYNOPTIONLIST, dynOptionList);
	if (dynOptionList)
	{
		text = text.trimLeft().trimRight();
		// options must be in format [Aaaa/Bbbb/CCccc/DDdddd]
		const int start = text.find(L'[');
		const int end = text.find(L']');
		if (start > -1 && end > -1 && start < end)
		{
			const int findLeftRes = text.find(L'<');
			const int findRightRes = text.find(L'>');
			const int findLastLeftRes = text.reverseFind(L'<');
			const int findLastRightRes = text.reverseFind(L'>');
			bool hasOption = start > -1 && end > -1; //always true
			const bool isDefaultCorrect = findLeftRes > -1 && findRightRes > -1 && findLeftRes < findRightRes && findRightRes > end;
			const bool isLastDefaultCorrect = findLastLeftRes > -1 && findLastRightRes > -1 && findLastLeftRes < findLastRightRes && findLastRightRes > end;
			OdString defaultValue;
			if (isDefaultCorrect &&
				(!hasOption || (hasOption && (findRightRes < start || findLeftRes > end))))
			{
				defaultValue = text.mid(findLeftRes + 1, findRightRes - findLeftRes - 1);
			}
			else if (isLastDefaultCorrect &&
				(!hasOption || (hasOption && (findLastRightRes < start || findLastLeftRes > end))))
			{
				defaultValue = text.mid(findLastLeftRes + 1, findLastRightRes - findLastLeftRes - 1);
			}

			OdString defaultValueForOptions;
			if (isLastDefaultCorrect &&
				(!hasOption || (hasOption && (findLastRightRes != findRightRes))))
			{
				defaultValueForOptions = text.mid(findLastLeftRes + 1, findLastRightRes - findLastLeftRes - 1);
			}

			IcString options = text.mid(start + 1, end - start - 1);
			options.splitString(L"/", m_optionList, false);
			

			text = text.left(start - 1) + text.mid(end + 1, text.getLength() - end - 1);
			if (text.right(1) == L":")
				text = text.left(text.getLength() - 1);

			text = text.trimLeft("/");

			// if one option is <Aaaa> set it also to default and remove <>
			bool isFindedDefValue = false;
			if (!defaultValue.isEmpty())
			{
				IIcadIOManager* pIO = getIOManagerInterface(getIcadInterfaces());
				ASSERT(pIO);
				IcString resKeyword;;
				if(pIO->findKeyword(defaultValue, resKeyword)>0)
				{
					for (int i = 0; i != m_optionList.size(); i++)
					{
						if (m_optionList[i].find(resKeyword) > -1)
						{
							m_defaultValue = m_optionList[i];
							if (auto pOptionListBox = m_BoxContainer.getOptionsListBox())
							{
								pOptionListBox->setDefaultIndex(i);
								isFindedDefValue = true;
							}
							break;
						}
					}
				}
			}

			if (!isFindedDefValue && !defaultValueForOptions.isEmpty())
			{
				m_optionList.insertAt(0, defaultValueForOptions);
				m_defaultValue = defaultValueForOptions;
				if (auto pOptionListBox = m_BoxContainer.getOptionsListBox())
				{
					pOptionListBox->setDefaultIndex(0);
				}
			}
		}
	}

	int dynShowDefault;
	retVal = pISysvar->getVar(k_IC_RDYNSHOWDEFAULT, dynShowDefault);

	if (dynShowDefault)
	{
		// default must be in format <XXXX>
		if (text.reverseFind(L'<') > -1 && text.reverseFind(L'>') > -1 && text.reverseFind(L'<') < text.reverseFind(L'>'))
		{
			int start = text.reverseFind(L'<');
			int end = text.reverseFind(L'>');
			m_defaultValue = text.mid(start + 1, end - start - 1);
			IIcadIOManager* pIO = getIOManagerInterface(getIcadInterfaces());
			ASSERT(pIO);
			IcString resKeyword;;
			if(pIO->findKeyword(m_defaultValue.trimLeft().trimRight(), resKeyword) > 0)
			{
				m_defaultValue = resKeyword;
			}
			// text = text.left(start) + text.right(text.getLength() - end - 1);
			text = text.trimLeft().trimRight();
		}
	}

	const int lastPos = text.reverseFind(L'\n');
	if (lastPos > -1)
	{
		text = text.right(text.getLength()-lastPos);
	}
	// avoid force update if exiting
	if (IcadEngineScheduler::get().getGlobalContext()->isIcadExiting())
		return;

	int promptMenu = 0;
	retVal = pISysvar->getVar(L"SHORTCUTMENU", promptMenu);
	ASSERT(retVal == RTNORM);
	bool isShortCutMenu = (promptMenu & 8) ? true : false;
	if (isShortCutMenu)
	{
		IcString keyWords = ExternalApplication::GetActiveApplication()->GetKeywordsList() + L" " +
		                    ExternalApplication::GetActiveApplication()->GetSystemKeywords();

		int findPos = keyWords.find(L" _");
		if (findPos != -1)
		{
			keyWords = keyWords.left(findPos);
		}

		if (keyWords != L" ")
		{
			m_optionList.clear();
			m_globalOptionList.clear();
			wchar_t* cptr1 = NULL;
			wchar_t* cptr2 = NULL;
			wchar_t* cmd = NULL;
			for (cptr1 = cptr2 = (wchar_t*)(const wchar_t*)keyWords; *cptr2 != 0; cptr2++)
			{
				if (*cptr2 == L' ')
				{
					*cptr2 = 0;
					cmd = cptr1;
					if (wcschr(cptr1, L'|'))
					{
						cmd = wcschr(cptr1, L'|');
						*cmd = 0;
						++cmd;
						for (int fi1 = 0; cptr1[fi1]; fi1++)
						{
							if (cptr1[fi1] == L'_')
								cptr1[fi1] = L' ';
						}
						//]-
					}
					if ((*cptr1 != L'~' && *cptr1 != 0) || (*cptr1 == L'~' && (cptr1 + 1) == cptr2))
					{
						m_optionList.push_back(cptr1);
						m_globalOptionList.push_back(cmd);
					}

					*cptr2 = L' ';
					cptr1 = cptr2 + 1;
				}
			}
			cmd = cptr1;
			if (wcschr(cptr1, L'|'))
			{
				cmd = wcschr(cptr1, L'|');
				if (cmd)
					*cmd = 0;
				++cmd;
				for (int fi1 = 0; cptr1[fi1]; fi1++)
				{
					if (cptr1[fi1] == L'_')
						cptr1[fi1] = L' ';
				}
			}
			if ((*cptr1 != L'~' && *cptr1 != 0) || (*cptr1 == L'~' && (cptr1 + 1) == cptr2))
			{
				m_optionList.push_back(cptr1);
				m_globalOptionList.push_back(cmd);
			}
			// Add the "Cancel" menu item.
			if (!(ExternalApplication::GetActiveApplication()->GetInputControlBits() & IcUserInputControls::kNoCancelMenuItem))
			{
				m_optionList.push_back(L"~");
				m_globalOptionList.push_back(L"");
				m_optionList.push_back(ResourceString(IDC_ICADWNDACTION_CANCEL_4, "Cancel"));
				m_globalOptionList.push_back(L"");
			}
		}
	}

	if (!threadRedir)
	{
		OdGePoint3d dummyPt;
		onMouseMove(dummyPt);
	}
}


void IcDynamicInputUIManager::setPrompt(OdString text, bool threadRedir)
{
	if (IcadEngineScheduler::get().getGlobalContext()->isIcadExiting())
		return;

	if (text.isEmpty())
	{
		text = IcCoreExternal::icGetCmdLTextPrompt();
	}

	if (m_BoxContainer.getPromptBox())
	{
		m_BoxContainer.getPromptBox()->setValue((LPCTSTR)text);
	}
}

bool IcDynamicInputUIManager::displayOptionList(int userInputControls)
{
	IIcadSysvarManager* pISysvar = getSysvarManagerInterface(getIcadInterfaces());
	ASSERT(pISysvar != NULL);

	int promptMenu = 0;
	int retVal = pISysvar->getVar(L"SHORTCUTMENU", promptMenu);
	ASSERT(retVal == RTNORM);
	bool isShortCutMenu = (promptMenu & 8) ? true : false;

	if (m_BoxContainer.getOptionsListBox(false) && m_BoxContainer.getOptionsListBox(false)->IsWindowVisible())
	{
		return false;
	}

	if (m_optionList.isEmpty())
	{
		return false;
	}

	if (CDynamicOptionListBox* optionList = m_BoxContainer.getOptionsListBox(true))
	{
		optionList->ResetContent();
		if (isShortCutMenu)
		{
			if (m_optionList.size() == m_globalOptionList.size())
			{
				for (int i = 0; i != m_optionList.size(); i++)
				{
					if (m_optionList[i].compare(L"`") == 0)
					{
						optionList->AddString(ResourceString(IDC_ICADPROMPTMENU_DONE_5, "Done"), (LPCTSTR)m_globalOptionList[i]);
					}
					else
					{
						optionList->AddString((LPCTSTR)m_optionList[i], (LPCTSTR)m_globalOptionList[i]);
					}
				}
				CSize s = optionList->getSize();
				optionList->MoveWindow(0, 0, s.cx, s.cy);
				optionList->SetExtendedStyle( optionList->GetExtendedStyle() & (~LVS_EX_GRIDLINES));
			}
			else
			{
				ASSERT(0);
			}
		}
		else
		{
			for (int i = 0; i != m_optionList.size(); i++)
			{
				optionList->AddString((LPCTSTR)m_optionList[i]);
				if (!m_defaultValue.isEmpty() && m_optionList[i].find(m_defaultValue) > -1)
				{
					optionList->setDefaultIndex(i);
				}
			}
			CSize s = optionList->getSize();
			optionList->MoveWindow(0, 0, s.cx, s.cy);
			optionList->SetExtendedStyle(optionList->GetExtendedStyle() | LVS_EX_GRIDLINES);
		}



		bool optionMandatory = (m_activeMode != IIcDynamicInputManager::eModeGetStringWithSpace) &&
								!(userInputControls & IcUserInputControls::kAcceptOtherInputString) &&
								optionList->GetItemCount() > 1;
		optionList->setMandatoryOption(optionMandatory);
		
		optionList->Show();
		resizeControls();
		return true;
	}

	return false;
}

bool IcDynamicInputUIManager::hasOptions()
{
	return (m_optionList.size() > 0);
}

void IcDynamicInputUIManager::destroyOptionList()
{
	if (m_BoxContainer.getOptionsListBox())
		m_BoxContainer.getOptionsListBox()->Hide();
	setUpdateRequired();
}

bool IcDynamicInputUIManager::isDisplayOptionList()
{
	if (m_BoxContainer.getOptionsListBox(false) &&
		m_BoxContainer.getOptionsListBox(false)->m_hWnd &&
		m_BoxContainer.getOptionsListBox()->IsWindowVisible())
	{
		return true;
	}
	return false;
}

// progesoft
class CommandComparer
{
public:
	CommandComparer(CString typedText) : m_typedText(typedText) {};
	bool operator ()(const ItemsCommand& c1, const ItemsCommand& c2)
	{
		CString alias1(c1.m_name), alias2(c2.m_name);
		if (c1.m_name.Find(L"(") >= 0)
			alias1 = c1.m_name.Left(c1.m_name.Find(L"(")).Trim();
		if (alias1 == m_typedText)
			return (alias1 != alias2); // std library in case of true check if exchanged operator return false
		else
		{
			if (c2.m_name.Find(L"(") >= 0)
				alias2 = c2.m_name.Left(c2.m_name.Find(L"(")).Trim();
			if (alias2 == m_typedText)
				return false;
			else if (c1.m_counter > c2.m_counter)
				return true;
			else if (c1.m_counter < c2.m_counter)
				return false;
			else if (c1.m_isSysVar && !c2.m_isSysVar)
				return false;
			else if (!c1.m_isSysVar && c2.m_isSysVar)
				return true;
			//else if (c1.m_name == c2.m_name)
			//	return c1.m_isSysVar;
			else
			{
				return c1.m_name.Compare(c2.m_name) < 0;
			}
		}
	};
protected:
	CString m_typedText;
};

struct sameName {
	sameName(LPCTSTR name) : m_name(name) {}

	bool operator()(ItemsCommand itm) {
		return itm.m_name.CompareNoCase(m_name) == 0;
	}

	LPCTSTR m_name;
};

bool IcDynamicInputUIManager::displayCommandList(bool autocomplete, bool forced, bool displayList)
{
	CDynamicCommandListBox* commandList = m_BoxContainer.getCommandListBox(true);
	if (commandList)
	{
		// progesoft
		// start timer
		int autoCompleteDelay = 300;
		auto m_pISysvar = getSysvarManagerInterface(getIcadInterfaces());
		int retVal = m_pISysvar->getVar(L"AUTOCOMPLETETO", autoCompleteDelay);
		if (displayList)  // set timer only when display is needed
		{
			if (!commandList->IsWindowVisible() && !forced && autoCompleteDelay > 0)
			{
				commandList->SetTimer(CDynamicCommandListBox::kFillList, autoCompleteDelay, NULL);
				return true;
			}
			m_BoxContainer.getCommandStringBox()->SetFocus();
		}

		// progesoft
		// use autocompletefilter variable to setup what add in command list
		IIcadSysvarManager* pISysvar = getSysvarManagerInterface(getIcadInterfaces());
		int autocompleteFilter = 0;
		retVal = pISysvar->getVar(L"AUTOCOMPLETEFILTER", autocompleteFilter);

		IIcadBaseDocInternal* pCurDoc = IcCoreExternal::getCurrentDoc();
		if (pCurDoc)
			pCurDoc->startTimer(L"filter command list");

		std::unordered_map<std::wstring, IcAlias*> aliasesMap;
		bool globalName = false;
		// bool lispExp = false;
		CString stValue;
		m_BoxContainer.getCommandStringBox()->GetWindowText(stValue);

		CString stPartial; // progesoft
		stPartial = stValue;

		for (int i = 0; i < stValue.GetLength(); i++)
		{
			if (iswpunct(stValue[i]) != 0
				&& stValue[i] != L'_'
				&& stValue[i] != L'-'
				&& stValue[i] != L'+'
				&& stValue[i] != L'('
				&& stValue[i] != L')'
				&& stValue[i] != L':'
				)
			{
				destroyCommandList();
				if (pCurDoc)
					pCurDoc->stopTimer(L"filter command list");
				return true;
			}
		}

		wchar_t firstChar = stValue[0];
		switch (firstChar)
		{
			case L'_':
				globalName = true;
				// stValue.Delete(0,1);
				break;
		}

		if (stValue.IsEmpty() || (globalName && stValue.GetLength() == 1))
		{
			destroyCommandList();
			if (pCurDoc)
				pCurDoc->stopTimer(L"filter command list");
			return true;
		}

		stValue += "*";

		/*DWORD cmdTypes = value;
		cmdTypes |= EIcadCommands;*/

		// int limit = 0;
		// int retVal = m_pISysvar->getVar(L"AUTOCOMPLETELIMIT", limit);*/

		commandList->ResetContent();
		allItemsForList.clear();
		if (autocompleteFilter & CommandAutoComplete::EIcadAliases)
		{
			IcDynamicInputUtils::FindAliasesByMask(stValue, aliasesMap);
		}

		if (autocompleteFilter & CommandAutoComplete::EIcadSysvars)
		{
			OdRxDictionaryPtr pVarDict = odrxSysRegistry()->getAt(L"ODDB_SYSVARDICT");
			OdRxDictionaryPtr pIcVarDict = odrxSysRegistry()->getAt(L"ICAD_SYSVARTYPESDICT");

			std::vector<IcString> allSysvarsForList;
			if (autocompleteFilter & CommandAutoComplete::EIcadSysvars)
			{
				IcDynamicInputUtils::FindSysvarsByMask(pVarDict, pIcVarDict, OdString(stValue), allSysvarsForList);
				for (auto sysVar = allSysvarsForList.begin(); sysVar != allSysvarsForList.end(); ++sysVar)
				{
					allItemsForList.push_back(ItemsCommand((LPCTSTR)(*sysVar),0, true));
				}
			}
		}
		for (OdRxIteratorPtr groupIter = ::odedRegCmds()->newGroupIterator(); !groupIter->done(); groupIter->next())
		{
			OdRxDictionaryPtr group = groupIter->object();
			if (group.isNull())
				continue;

			for (OdRxIteratorPtr iter = group->newIterator(); !iter->done(); iter->next())
			{
				OdEdCommandPtr cmd = iter->object();

			if (cmd->flags() & (IcEdCommand::kToCallFromAPIOnly | IcEdCommand::kNoAutoComplete))
					continue;

				if (!IcDynamicInputUtils::IsCommandAvailable(cmd))
					continue;

				IcString groupName = cmd->groupName();
				IcString cmdName;
				if (globalName)
				{
					cmdName = cmd->globalName();
				}
				else
				{
					cmdName = cmd->localName();
					if (cmdName.find(L"C:") == 0)
					{
						cmdName.deleteChars(0, 2);
						groupName = ICAD_COMMAND_GROUP;
					}
				}

				if (/*!(value & CommandAutoComplete::ELispAll) &&*/
					(groupName == ICAD_LISP_DEFUN || groupName == ICAD_LISP_ATOM))
				{
					continue;
				}

				if (!(autocompleteFilter & CommandAutoComplete::EIrxCommands) &&
					(groupName != ICAD_COMMAND_GROUP))
				{
					continue;
				}

				if (::odutWcMatchNoCase(cmdName, OdString(stValue)))
				{
					int pos = 0;

					auto it = aliasesMap.end();
					if (!globalName)
					{
						it = aliasesMap.find((LPCTSTR)cmdName);
					}

					if (it != aliasesMap.end())
					{
						CString newName;
						newName.Format(L"%s (%s)", it->second->m_name.c_str(), it->second->m_localizedCommand.c_str());
						allItemsForList.push_back(ItemsCommand((LPCTSTR)newName, m_commandCounter.getCommandCount(IcString(newName))));
						// cmdName = newName;
					}

					// progesoft
					// added command counter usage
					std::vector<ItemsCommand>::iterator iter = std::find_if(allItemsForList.begin(), allItemsForList.end(), sameName((LPCTSTR)cmdName));
					if (iter == allItemsForList.end())
					{
						if (it != aliasesMap.end())
							allItemsForList.push_back(ItemsCommand( (LPCTSTR)cmdName, m_commandCounter.getCommandCount( OdString(it->second->m_localizedCommand.c_str()))));
						else
							allItemsForList.push_back(
									ItemsCommand((LPCTSTR)cmdName,
													m_commandCounter.getCommandCount(cmdName)));
					}
					else
						allItemsForList.push_back(ItemsCommand(
								(LPCTSTR)cmdName, m_commandCounter.getCommandCount(cmdName)));

					if (it != aliasesMap.end())
					{
						aliasesMap.erase(it);
					}
				}
			}
		}

		for (auto it = aliasesMap.begin(); it != aliasesMap.end(); it++)
		{
			CString newName;
			newName.Format(L"%s (%s)", it->second->m_name.c_str(),it->second->m_localizedCommand.c_str());
			// progesoft
			// added command counter usage
			allItemsForList.push_back(ItemsCommand(newName, m_commandCounter.getCommandCount(OdString(it->second->m_localizedCommand.c_str()))));
		}
		// progesoft
		// custom comparer
		CommandComparer comparer(stPartial.MakeUpper());
		std::sort(allItemsForList.begin(), allItemsForList.end(), comparer);

		// progesoft [-
		// add also commands that not starts with partial but only contains
		if (stPartial.GetLength() >= 3 &&
			((!displayList && allItemsForList.size() == 0) || displayList))
		{
			stValue = L"*" + stPartial + L"*";

			size_t prevSize = allItemsForList.size();

			if (autocompleteFilter & CommandAutoComplete::EIcadSysvars)
			{
				OdRxDictionaryPtr pVarDict = odrxSysRegistry()->getAt(L"ODDB_SYSVARDICT");
				OdRxDictionaryPtr pIcVarDict = odrxSysRegistry()->getAt(L"ICAD_SYSVARTYPESDICT");

				std::vector<IcString> allSysvarsForList;

				IcDynamicInputUtils::FindSysvarsByMask(pVarDict, pIcVarDict, OdString(stValue), allSysvarsForList);

				for (auto sysVar = allSysvarsForList.begin(); sysVar != allSysvarsForList.end(); ++sysVar)
					{
						allItemsForList.push_back(ItemsCommand((LPCTSTR)(*sysVar), 0, true));
					}
			}
			for (OdRxIteratorPtr groupIter = ::odedRegCmds()->newGroupIterator(); !groupIter->done(); groupIter->next())
			{
				OdRxDictionaryPtr group = groupIter->object();
				if (group.isNull())
					continue;

				for (OdRxIteratorPtr iter = group->newIterator(); !iter->done(); iter->next())
				{
					OdEdCommandPtr cmd = iter->object();

					if (cmd->flags() & (IcEdCommand::kToCallFromAPIOnly | IcEdCommand::kNoAutoComplete))
						continue;

					if (!IcDynamicInputUtils::IsCommandAvailable(cmd))
						continue;

					IcString groupName = cmd->groupName();
					IcString cmdName;
					if (globalName)
					{
						cmdName = cmd->globalName();
					}
					else
					{
						cmdName = cmd->localName();
						if (cmdName.find(L"C:") == 0)
						{
							cmdName.deleteChars(0, 2);
							groupName = ICAD_COMMAND_GROUP;
						}
					}

					if (groupName == ICAD_LISP_DEFUN || groupName == ICAD_LISP_ATOM)
					{
						continue;
					}

					if (::odutWcMatchNoCase(cmdName, OdString(stValue)))
					{
						int pos = 0;

						auto it = aliasesMap.end();
						if (!globalName)
						{
							it = aliasesMap.find((LPCTSTR)cmdName);
						}

						if (it != aliasesMap.end())
						{
							if (!(autocompleteFilter & CommandAutoComplete::EIcadAliases))
								continue;
							CString newName;
							newName.Format(L"%s (%s)", it->second->m_name.c_str(), it->second->m_command.c_str());
							cmdName = newName;
						}

						// progesoft
						// added command counter usage
						// allItemsForList.push_back(ItemsCommand((LPCTSTR)cmdName));
						std::vector<ItemsCommand>::iterator iter = std::find_if(allItemsForList.begin(), allItemsForList.end(), sameName((LPCTSTR)cmdName));
						if (iter == allItemsForList.end())
						{
							if (it != aliasesMap.end())
							allItemsForList.push_back(ItemsCommand((LPCTSTR)cmdName, m_commandCounter.getCommandCount(OdString(it->second->m_localizedCommand.c_str()))));
						else
							allItemsForList.push_back(ItemsCommand((LPCTSTR)cmdName, m_commandCounter.getCommandCount(cmdName)));
						}
						if (it != aliasesMap.end())
						{
							aliasesMap.erase(it);
						}
					}
				}
			}

			std::vector<ItemsCommand>::iterator last = allItemsForList.begin();
			if (allItemsForList.size() > prevSize)
				std::advance(last, prevSize++);

			std::sort(last, allItemsForList.end(), comparer);
		}

		if (displayList)
		{
			CList<CString> tmpLst;
			for (int index = 0; index < (int)allItemsForList.size(); index++)
			{
				if (tmpLst.Find(allItemsForList[index].m_name) == NULL)
				{
					int insertIndex = commandList->AddString(allItemsForList[index].m_name);
					commandList->SetItemData(insertIndex, (DWORD_PTR)&allItemsForList[index]);
				}
				tmpLst.AddTail(allItemsForList[index].m_name);
			}
		}
		// progesoft -]

		CDynamicCommandStringEdit* commandStringEdit = ((CDynamicCommandStringEdit*)m_BoxContainer.getCommandStringBox());
		int startSel, endSel;
		CString text;
		if (commandStringEdit)
		{
			commandStringEdit->GetSel(startSel, endSel);
			commandStringEdit->GetWindowTextW(text);
		}

		bool canBeAutocompleted = startSel >= text.GetLength();
		if (allItemsForList.size() > 0 && autocomplete && canBeAutocompleted)
		{
			CString firstCommand = allItemsForList[0].m_name;
			int pos = firstCommand.Find(L' ');
			if (pos > 0)
			{
				firstCommand = firstCommand.Left(pos);
			}
			if (commandStringEdit && firstCommand.Find(text.MakeUpper())==0)
				commandStringEdit->AutocompleteTextHighlighted(firstCommand);
			commandList->SetCurSel(0);
		}

		m_BoxContainer.getCommandStringBox()->postPoneShow();
		if (displayList)
		{
			commandList->postPoneShow();
		}

		// progesoft
		// after timer force show of list
		if (forced && displayList)
		{
			m_BoxContainer.getCommandStringBox()->postPoneShow();
			commandList->postPoneShow();
		}

		if (pCurDoc)
			pCurDoc->stopTimer(L"filter command list");
		return true;
	}

	return false;
}

void IcDynamicInputUIManager::destroyCommandList()
{
	if (m_BoxContainer.getCommandListBox())
		m_BoxContainer.getCommandListBox()->Hide();

	// progesoft
	m_BoxContainer.getCommandListBox()->ResetContent();

	setUpdateRequired();
}

bool IcDynamicInputUIManager::isDisplayCommandList()
{
	if (m_BoxContainer.getCommandListBox(false) &&
		m_BoxContainer.getCommandListBox(false)->m_hWnd &&
		m_BoxContainer.getCommandListBox()->IsWindowVisible())
	{
		return true;
	}
	return false;
}

bool IcDynamicInputUIManager::isProcessControlEditing()
{
	if (getActiveControl() && (getActiveControl()->getStatus() == CDynamicEdit::eStateEditing || getActiveControl()->getStatus() == CDynamicEdit::eStateError))
	{
		return true;
	}
	return false;
}

// progesoft
bool IcDynamicInputUIManager::useArrowCursor()
{
	int dyncursor = 0;
	icedGetVar(L"DYNCURSORMODE", dyncursor);
	if (dyncursor == 0)
	{
		return true;
	}
	if ( getActiveControl() && (getActiveControl()->requiresArrowCursor()))
	{
		return true;
	}
	return false;
}

IIcDynamicInputManager::ActiveMode IcDynamicInputUIManager::setMode(IIcDynamicInputManager::ActiveMode mode)
{
	_LOCK();

	IIcadSysvarManager* pISysvar = getSysvarManagerInterface(getIcadInterfaces());

	CString oldValue;
	IIcDynamicInputManager::ActiveMode oldMode = m_activeMode;

	if (m_activeMode != mode)
	{
		int dynMode;
		int retVal = pISysvar->getVar(L"DYNMODE", dynMode);

		if (mode == IIcDynamicInputManager::eModeDimensions &&
			!(dynMode & DynInputValues::KDimInputOn)) // dimension mode is disabled by dynmode variable
			mode = getAlternativeMode();

		deactivate();

		m_activeMode = mode;

		if (m_activeMode == IIcDynamicInputManager::eModeNotActive)
			m_state = DynInputUINotActiveState::getInstance();
		else if (m_activeMode == IIcDynamicInputManager::eModeOnlyPrompt)
			m_state = DynInputUIOnlyPromptState::getInstance();
		else if (m_activeMode == IIcDynamicInputManager::eModeCoordinate)
			m_state = DynInputUICoordinateState::getInstance();
		else if (m_activeMode == IIcDynamicInputManager::eModeGetReal)
			m_state = DynInputUIGetRealState::getInstance();
		else if (m_activeMode == IIcDynamicInputManager::eModeGetInt)
			m_state = DynInputUIGetIntState::getInstance();
		else if (m_activeMode == IIcDynamicInputManager::eModeGetDist)
			m_state = DynInputUIGetDistState::getInstance();
		else if (m_activeMode == IIcDynamicInputManager::eModeGetAngle)
			m_state = DynInputUIGetAngleState::getInstance();
		else if (m_activeMode == IIcDynamicInputManager::eModeGetString)
			m_state = DynInputUIGetStringState::getInstance();
		else if (m_activeMode == IIcDynamicInputManager::eModeGetStringWithSpace)
			m_state = DynInputUIGetStringWithSpaceState::getInstance();
		else if (m_activeMode == IIcDynamicInputManager::eModeGetOrient)
			m_state = DynInputUIGetOrientState::getInstance();
		else if (m_activeMode == IIcDynamicInputManager::eModeDimensions)
			m_state = DynInputUIDimensionsState::getInstance();

		if (dynMode > DynInputValues::KPointerAndDimInputOff)
			activate();
	}
	return oldMode;
}

static void AdjustViewRectToMonitor(CPoint cursorPoint, CRect& viewRect)
{
	if (!viewRect.IsRectEmpty())
	{
		if (auto hMonitor = MonitorFromPoint(cursorPoint, MONITOR_DEFAULTTONULL))
		{
			MONITORINFO mi;
			mi.cbSize = sizeof(mi);
			if (GetMonitorInfo(hMonitor, &mi))
			{
				CRect monitorRect;
				monitorRect = mi.rcWork;
//				VERIFY(viewRect.IntersectRect(viewRect, monitorRect));
			}
		}
	}
}

void IcDynamicInputUIManager::resizeControls()
{
	if (!m_enableControlsPosition)
		return;

	if (!IcadSharedGlobals::IsMainThreadCurrent())
	{
		return;
	}

	LARGE_INTEGER pcElapsed;
	LARGE_INTEGER pc1, pc2, pc3, pc4, pc5, pc6;
	LARGE_INTEGER pcStart, pcFreq;

	QueryPerformanceCounter(&pcStart);
	QueryPerformanceFrequency(&pcFreq);

	CPoint cursorPt = m_lastCursorPos;
	int deltax = SPACE_BETWEEN_EDIT;
	int deltay = 15;

	IIcadSysvarManager* pISysvar = getSysvarManagerInterface(getIcadInterfaces());
	ASSERT(pISysvar != NULL);

	int dynMode;
	int retVal = pISysvar->getVar(L"DYNMODE", dynMode);
	if (dynMode <= DynInputValues::KPointerAndDimInputOff)
		return;

	int dynPrompt;
	retVal = pISysvar->getVar(L"DYNPROMPT", dynPrompt);
	POINT pt;
	pt.x = cursorPt.x;
	pt.y = cursorPt.y;
	if (!icDocManagerPtr()->activeDocInstance())
		return;
	CWnd* currentView = icedGetIcadDwgView();
	CWnd* icadFrame = icedGetIcadFrame();
	currentView->ClientToScreen(&pt);

	HWND hWnd = GetActiveWindow();
	CSize totalSize = calculateTotalSize();

	QueryPerformanceCounter(&pc1);

	CRect viewRect;
	currentView->GetWindowRect(viewRect);

	AdjustViewRectToMonitor(pt, viewRect);

	if (pt.x + totalSize.cx + SPACE_BETWEEN_EDIT > viewRect.right)
		pt.x = viewRect.right - totalSize.cx - SPACE_BETWEEN_EDIT;
	if (pt.x < viewRect.left)
		pt.x = viewRect.left;
	if (pt.y + totalSize.cy + deltay > viewRect.bottom - GetSystemMetrics(SM_CXVSCROLL)) // subtract also layout tab
		pt.y = viewRect.bottom - totalSize.cy - GetSystemMetrics(SM_CXVSCROLL) - deltay;
	if (pt.y < viewRect.top)
		pt.y = viewRect.top;

	// icadFrame->ScreenToClient(&pt);

	OdArray<CWnd*> controlsToRedraw;

	HDWP hDwp = BeginDeferWindowPos(1);
	CSize size;
	if (dynMode > DynInputValues::KPointerAndDimInputOff && dynPrompt &&
		m_BoxContainer.getPromptBox() && (m_BoxContainer.getPromptBox()->IsWindowVisible() || m_BoxContainer.getPromptBox()->ShowIsRequired()))
	{
		size = m_BoxContainer.getPromptBox()->getSize();
		hDwp = ::DeferWindowPos(hDwp, m_BoxContainer.getPromptBox()->GetParent()->m_hWnd, NULL, pt.x + deltax, pt.y + deltay, size.cx, size.cy, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW);
		controlsToRedraw.append(m_BoxContainer.getPromptBox());
		deltax += size.cx + SPACE_BETWEEN_EDIT;
	}

	QueryPerformanceCounter(&pc2);

	bool bResizeCommandList = false;  // progesoft

	if (dynMode & DynInputValues::KPointerInputOn)
	{
		if (m_BoxContainer.getModeBox() &&
			(m_BoxContainer.getModeBox()->IsWindowVisible() || m_BoxContainer.getModeBox()->ShowIsRequired()) &&
			(m_BoxContainer.getXBox()->IsWindowVisible() || m_BoxContainer.getXBox()->ShowIsRequired()))
		{
			size = m_BoxContainer.getModeBox()->getSize();
			controlsToRedraw.append(m_BoxContainer.getModeBox());
			hDwp = ::DeferWindowPos(hDwp, m_BoxContainer.getModeBox()->GetParent()->m_hWnd, NULL, pt.x + deltax, pt.y + deltay, size.cx, size.cy, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW | SWP_NOCOPYBITS);
			deltax += size.cx + SPACE_BETWEEN_EDIT;
		}
		if (m_BoxContainer.getXBox() && (m_BoxContainer.getXBox()->IsWindowVisible() || m_BoxContainer.getXBox()->ShowIsRequired()))
		{
			size = m_BoxContainer.getXBox()->getSize();
			controlsToRedraw.append(m_BoxContainer.getXBox());
			hDwp = ::DeferWindowPos(hDwp, m_BoxContainer.getXBox()->GetParent()->m_hWnd, NULL, pt.x + deltax, pt.y + deltay, size.cx, size.cy, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW | SWP_NOCOPYBITS);
			deltax += size.cx + SPACE_BETWEEN_EDIT;
		}

		if (m_BoxContainer.getYBox() && (m_BoxContainer.getYBox()->IsWindowVisible() || m_BoxContainer.getYBox()->ShowIsRequired()))
		{
			size = m_BoxContainer.getYBox()->getSize();
			controlsToRedraw.append(m_BoxContainer.getYBox());
			hDwp = ::DeferWindowPos(hDwp, m_BoxContainer.getYBox()->GetParent()->m_hWnd, NULL, pt.x + deltax, pt.y + deltay, size.cx, size.cy, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW | SWP_NOCOPYBITS);
			deltax += size.cx + SPACE_BETWEEN_EDIT;
		}
		if (m_BoxContainer.getZBox() && (m_BoxContainer.getZBox()->IsWindowVisible() || m_BoxContainer.getZBox()->ShowIsRequired()))
		{
			size = m_BoxContainer.getZBox()->getSize();
			controlsToRedraw.append(m_BoxContainer.getZBox());
			hDwp = ::DeferWindowPos(hDwp, m_BoxContainer.getZBox()->GetParent()->m_hWnd, NULL, pt.x + deltax, pt.y + deltay, size.cx, size.cy, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW);
			deltax += size.cx + SPACE_BETWEEN_EDIT;
		}

		if (dynPrompt && m_BoxContainer.getCommandStringBox() &&
			(m_BoxContainer.getCommandStringBox()->IsWindowVisible() || m_BoxContainer.getCommandStringBox()->ShowIsRequired()))
		{
			bResizeCommandList = true;	// progesoft
			size = m_BoxContainer.getCommandStringBox()->getSize();
			controlsToRedraw.append(m_BoxContainer.getCommandStringBox());
			hDwp = ::DeferWindowPos(hDwp, m_BoxContainer.getCommandStringBox()->GetParent()->m_hWnd, NULL, pt.x + deltax, pt.y + deltay, size.cx, size.cy, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW | SWP_NOCOPYBITS);
		}
		if (m_BoxContainer.getStringBox() && (m_BoxContainer.getStringBox()->IsWindowVisible() || m_BoxContainer.getStringBox()->ShowIsRequired()))
		{
			size = m_BoxContainer.getStringBox()->getSize();
			controlsToRedraw.append(m_BoxContainer.getStringBox());
			hDwp = ::DeferWindowPos(hDwp, m_BoxContainer.getStringBox()->GetParent()->m_hWnd, NULL, pt.x + deltax, pt.y + deltay, size.cx, size.cy, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW);
		}
		if (m_BoxContainer.getAngleBox() && (m_BoxContainer.getAngleBox()->IsWindowVisible() || m_BoxContainer.getAngleBox()->ShowIsRequired()))
		{
			size = m_BoxContainer.getAngleBox()->getSize();
			controlsToRedraw.append(m_BoxContainer.getAngleBox());
			hDwp = ::DeferWindowPos(hDwp, m_BoxContainer.getAngleBox()->GetParent()->m_hWnd, NULL, pt.x + deltax, pt.y + deltay, size.cx, size.cy, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW);
		}
		if (m_BoxContainer.getRealBox() && (m_BoxContainer.getRealBox()->IsWindowVisible() || m_BoxContainer.getRealBox()->ShowIsRequired()))
		{
			size = m_BoxContainer.getRealBox()->getSize();
			controlsToRedraw.append(m_BoxContainer.getRealBox());
			hDwp = ::DeferWindowPos(hDwp, m_BoxContainer.getRealBox()->GetParent()->m_hWnd, NULL, pt.x + deltax, pt.y + deltay, size.cx, size.cy, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW);
		}
		if (m_BoxContainer.getIntBox() && (m_BoxContainer.getIntBox()->IsWindowVisible() || m_BoxContainer.getIntBox()->ShowIsRequired()))
		{
			size = m_BoxContainer.getIntBox()->getSize();
			controlsToRedraw.append(m_BoxContainer.getIntBox());
			hDwp = ::DeferWindowPos(hDwp, m_BoxContainer.getIntBox()->GetParent()->m_hWnd, NULL, pt.x + deltax, pt.y + deltay, size.cx, size.cy, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW);
		}
	}

	if (dynMode > DynInputValues::KPointerAndDimInputOff && m_optionList.size() > DynInputValues::KPointerAndDimInputOff)
	{
		if (m_BoxContainer.getOptionsListBox() && m_BoxContainer.getOptionsListBox()->m_hWnd && m_BoxContainer.getOptionsListBox()->IsWindowVisible())
		{
			CSize sizeList = m_BoxContainer.getOptionsListBox()->getSize();
			hDwp = ::DeferWindowPos(hDwp, m_BoxContainer.getOptionsListBox()->GetParent()->m_hWnd, NULL, pt.x + SPACE_BETWEEN_EDIT, pt.y + OFFSET_Y_OPTION_LIST + size.cy, sizeList.cx, sizeList.cy, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW);
			controlsToRedraw.append(m_BoxContainer.getOptionsListBox());
		}
	}

	if (m_BoxContainer.getCommandListBox() &&
		m_BoxContainer.getCommandListBox()->m_hWnd && (m_BoxContainer.getCommandListBox()->IsWindowVisible() || m_BoxContainer.getCommandListBox()->ShowIsRequired()))
	{
		CSize sizeList = m_BoxContainer.getCommandListBox()->getSize();
		hDwp = ::DeferWindowPos(hDwp, m_BoxContainer.getCommandListBox()->GetParent()->m_hWnd, NULL, pt.x + SPACE_BETWEEN_EDIT, pt.y + OFFSET_Y_OPTION_LIST + size.cy, sizeList.cx, sizeList.cy, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW);
		controlsToRedraw.append(m_BoxContainer.getCommandListBox());
	}

	bool arcPointOut = false;
	QueryPerformanceCounter(&pc3);
	if (dynMode & DynInputValues::KDimInputOn)
	{
		if (m_activeMode == IIcDynamicInputManager::eModeDimensions && m_pDataControls.size())
		{
			// lock to avoid crash during resizeControls starting an esnap commands
			_LOCK();
			OdGsViewPtr pGsView = icGetActiveView();
			for (unsigned int i = 0; i != m_pDataControls.size(); i++)
			{
				CDynamicEdit* pEdit = m_pDataControls[i];
				if (!pEdit)
					continue;

				if ((!pEdit->m_hWnd || !IsWindowVisible(pEdit->m_hWnd)) && !pEdit->ShowIsRequired())
					continue;

				OdGeMatrix3d wcs2Scr = IcWcsToScreenTransform(pGsView);

				OdDbDimData* pDimData = pEdit->dimData();
				OdDbDimensionPtr pDim;
				if (pDimData)
				{
					pDim = pDimData->dimension();
				}

				if (!pDim.isNull() && !pDimData->isDimInvisible())
				{
					// move editbox to dimension position
					CSize size = pEdit->getSize();
					OdGePoint3d ptText = pDim->textPosition();
					if (pDimData->isDimDeltaAngle() || pDimData->isDimResultantAngle())
					{
						OdDb3PointAngularDimensionPtr pRadial = OdDb3PointAngularDimension::cast(pDim);
						OdGePoint3d arcPoint = pRadial->arcPoint();
						OdGePoint3d centerPt = pRadial->centerPoint();
						OdGePoint3d xline1 = pRadial->xLine2Point();
						OdGePoint3d xline2 = pRadial->xLine1Point();
						double radius = centerPt.distanceTo(arcPoint);
						OdGeVector3d xline1Vec = (xline1 - centerPt);
						if (!xline1Vec.isZeroLength())
						{
							xline1Vec.normalize();
						}
						OdGeVector3d xline2Vec = (xline2 - centerPt);
						if (!xline2Vec.isZeroLength())
						{
							xline2Vec.normalize();
						}
						OdGeVector3d normal = xline1Vec + xline2Vec;
						if (normal.isZeroLength())
						{
							normal = xline1 - centerPt;
						}
						if (!normal.isZeroLength())
						{
							normal.normalize();
						}
						ptText = centerPt + radius * normal;
					}
					OdGsDCPoint dcPoint = wcs2Screen(ptText, wcs2Scr);
					if (!icDocManagerPtr()->activeDocInstance())
						return;
					CWnd* currentView = icedGetIcadDwgView();
					CPoint pt; pt.x = dcPoint.x; pt.y = dcPoint.y;
					currentView->ClientToScreen(&pt);
					pt.x -= size.cx / 2;
					pt.y -= size.cy / 2;
					// reduce viewrect area
					CRect usableArea(viewRect);
					usableArea.DeflateRect(0, 0, size.cx, size.cy);
					if (!usableArea.PtInRect(pt))
					{
						// dimension point is outside of screen ,
						// move it to middle point of segment of dimension in screen
						arcPointOut = true;
						if (pDimData->isDimDeltaAngle() || pDimData->isDimResultantAngle())
						{
							// it's an arc
							OdDb3PointAngularDimensionPtr pRadial = OdDb3PointAngularDimension::cast(pDim);
							OdGePoint3d extLinePt1, extLinePt2;
							OdGePoint3d tl(usableArea.left, usableArea.top, 0.0), tr(usableArea.right, usableArea.top, 0.0), bl(usableArea.left, usableArea.bottom, 0.0), br(usableArea.right, usableArea.bottom, 0.0);
							ptText = pRadial->textPosition();

							OdGePoint3d centerPt = pRadial->centerPoint();
							if (IcWcsToUcs(centerPt).y < IcWcsToUcs(ptText).y)
								extLinePt1 = pRadial->xLine2Point();
							else
								extLinePt1 = pRadial->xLine1Point();
							extLinePt2 = centerPt + IcUcsToWcs(OdGeVector3d(centerPt.distanceTo(extLinePt1), 0, 0));
							if (extLinePt2.isEqualTo(extLinePt1))
							{
								// if extLinePt1 == extLinePt2 invert ext point used
								if (IcWcsToUcs(centerPt).y > IcWcsToUcs(ptText).y)
									extLinePt1 = pRadial->xLine2Point();
								else
									extLinePt1 = pRadial->xLine1Point();
								extLinePt2 = centerPt + IcUcsToWcs(OdGeVector3d(centerPt.distanceTo(extLinePt1), 0, 0));
							}
							OdGsDCPoint dcXLinePt1 = wcs2Screen(extLinePt1, wcs2Scr);
							OdGsDCPoint dcXLinePt2 = wcs2Screen(extLinePt2, wcs2Scr);
							OdGsDCPoint dcPtText = wcs2Screen(ptText, wcs2Scr);
							POINT pt1, pt2, pixPtText;
							pt1.x = (long)dcXLinePt1.x; pt1.y = (long)dcXLinePt1.y;
							pt2.x = (long)dcXLinePt2.x; pt2.y = (long)dcXLinePt2.y;
							pixPtText.x = (long)dcPtText.x; pixPtText.y = (long)dcPtText.y;
							currentView->ClientToScreen(&pt1);
							currentView->ClientToScreen(&pt2);
							currentView->ClientToScreen(&pixPtText);
							extLinePt1.set(pt1.x, pt1.y, 0.0);
							extLinePt2.set(pt2.x, pt2.y, 0.0);
							ptText.set(pixPtText.x, pixPtText.y, 0.0);

							OdGeCircArc3d* pArc = new OdGeCircArc3d(extLinePt1, ptText, extLinePt2);
							OdGeLine3d borders[4];
							borders[0].set(tl, tr), borders[1].set(bl, br), borders[2].set(tl, bl), borders[3].set(tr, br);
							int numInt;
							OdGePoint3d inters[2];
							OdArray<OdGeCircArc3d*> arcs;
							arcs.append(pArc);
							for (int j = 0; j != 4; j++)
							{
								for (unsigned int l = 0; l < arcs.size(); l++)
								{
									int realIntPt = 0;
									arcs[l]->intersectWith(borders[j], numInt, inters[0], inters[1]);
									for (int i = 0; i != numInt; i++)
									{
										OdGeCurve3d *pCurve1 = NULL, *pCurve2 = NULL;
										double param = arcs[l]->paramOf(inters[i]);
										arcs[l]->getSplitCurves(param, pCurve1, pCurve2);
										if (pCurve1 && !IS_EQUAL(pCurve1->length(), arcs[l]->length(), 10e-7))
										{
											arcs.append((OdGeCircArc3d*)pCurve1);
											realIntPt++;
										}
										if (pCurve2 && !IS_EQUAL(pCurve2->length(), arcs[l]->length(), 10e-7))
										{
											arcs.append((OdGeCircArc3d*)pCurve2);
											realIntPt++;
										}
									}
									if (realIntPt > 0)
									{
										arcs.removeAt(l);
										l--;
									}
								}
							}
							OdGePoint3d midPt;
							double maxLen = -1;
							for (int i = 0; i != arcs.size(); i++)
							{
								OdGePoint3dArray ptsSample;
								OdGeDoubleArray params;
								double arcLen = arcs[i]->length();
								arcs[i]->getSamplePoints(3, ptsSample, params);

								if (ptsSample.size() == 3)
								{
									// OdGePoint3d sArc = ptsSample[0];
									OdGePoint3d midArc = ptsSample[1];
									// OdGePoint3d eArc = ptsSample[2];
									POINT ptPix;
									ptPix.x = (long)midArc.x;
									ptPix.y = (long)midArc.y;
									if (usableArea.PtInRect(ptPix) && OdLess(maxLen, arcLen))
									{
										midPt = midArc;
										maxLen = arcLen;
									}
								}
							}
							for (int i = 0; i != arcs.size(); i++)
							{
								delete arcs[i];
							}
							// ASSERT(maxLen>0);
							if (maxLen > 0)
							{
								pt.x = (long)midPt.x - size.cx / 2;
								pt.y = (long)midPt.y - size.cy / 2;
							}
						}
						else
						{
							// it's a line
							OdGePoint3d extLinePt1, extLinePt2, textPt, middlePt, startPt, endPt;
							OdDbAlignedDimensionPtr pAligned = OdDbAlignedDimension::cast(pDim);
							OdDbArcDimensionPtr pArc = OdDbArcDimension::cast(pDim);
							if (!pAligned.isNull())
							{
								startPt = extLinePt1 = pAligned->xLine1Point();
								endPt = extLinePt2 = pAligned->xLine2Point();
								middlePt.set((extLinePt1.x + extLinePt2.x) / 2, (extLinePt1.y + extLinePt2.y) / 2, (extLinePt1.z + extLinePt2.z) / 2);
								textPt = pAligned->textPosition();
							}
							else if (!pArc.isNull())
							{
								startPt = extLinePt1 = pArc->xLine1Point();
								endPt = extLinePt2 = pArc->xLine2Point();
								middlePt.set((extLinePt1.x + extLinePt2.x) / 2, (extLinePt1.y + extLinePt2.y) / 2, (extLinePt1.z + extLinePt2.z) / 2);
								textPt = pArc->textPosition();
							}
							OdGeVector3d direction = textPt - middlePt;
							extLinePt1 += direction;
							extLinePt2 += direction;
							OdGsDCPoint dcXLinePt1 = wcs2Screen(extLinePt1, wcs2Scr);
							OdGsDCPoint dcXLinePt2 = wcs2Screen(extLinePt2, wcs2Scr);
							OdGsDCPoint dcStartPt = wcs2Screen(startPt, wcs2Scr);
							OdGsDCPoint dcEndPt = wcs2Screen(endPt, wcs2Scr);

							CPoint pt1; pt1.x = dcXLinePt1.x; pt1.y = dcXLinePt1.y;
							currentView->ClientToScreen(&pt1);
							CPoint pt2; pt2.x = dcXLinePt2.x; pt2.y = dcXLinePt2.y;
							currentView->ClientToScreen(&pt2);
							CPoint ptStartPoint; ptStartPoint.x = dcStartPt.x; ptStartPoint.y = dcStartPt.y;
							currentView->ClientToScreen(&ptStartPoint);
							CPoint ptEndPoint; ptEndPoint.x = dcEndPt.x; ptEndPoint.y = dcEndPt.y;
							currentView->ClientToScreen(&ptEndPoint);
							CPoint ptInside, ptOutside;
							// have at least on point inside window area
							if (usableArea.PtInRect(pt1) || usableArea.PtInRect(pt2))
							{
								if (usableArea.PtInRect(pt1))
								{
									ptInside = pt1; ptOutside = pt2;
								}
								else
								{
									ptInside = pt2; ptOutside = pt1;
								}
								// calculate mid between inside point and outside one
								double DX = ptInside.x - ptOutside.x;
								double DY = ptInside.y - ptOutside.y;
								if (FABS(DX) < 10e-7)
								{
									pt.x = ptInside.x;
									pt.y = (ptOutside.y < usableArea.top) ? (ptInside.y + usableArea.top) / 2 : (ptInside.y + usableArea.bottom) / 2;
								}
								else if (FABS(DY) < 10e-7)
								{
									pt.x = (ptOutside.x < usableArea.left) ? (ptInside.x + usableArea.left) / 2 : (ptInside.x + usableArea.right) / 2;
									pt.y = ptInside.y;
								}
								else
								{
									double angle = atan2((double)(ptInside.y - ptOutside.y), (double)(ptInside.x - ptOutside.x));
									if (pt.x > usableArea.right)
									{
										double dx = ptInside.x - usableArea.right;
										double dy = (DY / DX) * dx;
										ptOutside.x = usableArea.right;
										ptOutside.y = ptInside.y - (long)dy;
									}
									else if (pt.x < usableArea.left)
									{
										double dx = ptInside.x - usableArea.left;
										double dy = (DY / DX) * dx;
										ptOutside.x = usableArea.left;
										ptOutside.y = ptInside.y - (long)dy;
									}
									else if (pt.y > usableArea.bottom)
									{
										pt.y = usableArea.bottom - size.cy;
										double dy = ptInside.y - usableArea.bottom;
										double dx = (DX / DY) * dy;
										ptOutside.x = ptInside.x - (long)dx;
										ptOutside.y = usableArea.bottom;
									}
									else if (pt.y < usableArea.top)
									{
										pt.y = usableArea.top - size.cy;
										double dy = ptInside.y - usableArea.top;
										double dx = (DX / DY) * dy;
										ptOutside.x = ptInside.x - (long)dx;
										ptOutside.y = usableArea.top;
									}
									pt.x = (ptOutside.x + ptInside.x - size.cx) / 2;
									pt.y = (ptOutside.y + ptInside.y - size.cy) / 2;
								}
							}
							else
							{
								// both points are outside
								// autocad uses start point or end point if start is out
								if (usableArea.PtInRect(ptStartPoint))
									pt = ptStartPoint;
								else
								{
									//adjust this point over end point to avoid overlap of prompt edit
									pt = ptEndPoint;
									CSize size = pEdit->getSize();
									pt.y -= (long)(size.cy * 1.5);
								}
							}
						}
					}

					CRect cursorArea(m_lastCursorPos.x - 10, m_lastCursorPos.y - 10, m_lastCursorPos.x + 10, m_lastCursorPos.y + 10);
					CRect controlArea(pt.x, pt.y, pt.x + size.cx, pt.y + size.cy);
					CRect intersRect;
					currentView->ClientToScreen(&cursorArea);
					if (intersRect.IntersectRect(cursorArea, controlArea))
					{
						pt.y = cursorArea.bottom;
					}

					if (pt.x + size.cx > viewRect.right)
						pt.x = viewRect.right - size.cx;
					if (pt.y + size.cy > usableArea.bottom)
						pt.y = usableArea.bottom - size.cy;
					if (pt.x < viewRect.left)
						pt.x = viewRect.left;
					if (pt.y < viewRect.top)
						pt.y = viewRect.top;

					// icadFrame->ScreenToClient( &pt );

					if (pEdit->GetParent())
						hDwp = ::DeferWindowPos(hDwp, pEdit->GetParent()->m_hWnd, NULL, pt.x, pt.y, size.cx, size.cy, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW);
					controlsToRedraw.append(pEdit);
				}
				else
				{
					// don't have dimension, place at right of promptbox
					CSize size = pEdit->getSize();

					// in case of multiple edit controls I need to calculate total size and see if it goes out of screen to move inside
					// for example selection

					if (pEdit->GetParent())
						hDwp = ::DeferWindowPos(hDwp, pEdit->GetParent()->m_hWnd, NULL, pt.x + deltax, pt.y + deltay, size.cx, size.cy, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW);
					controlsToRedraw.append(pEdit);
					// if (pEdit->m_hWnd)
					//	::SetWindowPos( pEdit->m_hWnd, NULL, 0, 0, size.cx , size.cy , SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW   );
					deltax += size.cx + SPACE_BETWEEN_EDIT;
				}
			}
		}
	}
	QueryPerformanceCounter(&pc4);

	BOOL dwpRes = EndDeferWindowPos(hDwp);

	QueryPerformanceCounter(&pc5);

	for (unsigned int i = 0; i != controlsToRedraw.size(); i++)
	{
		CWnd* pEdit = controlsToRedraw[i];
		if (pEdit->IsKindOf(RUNTIME_CLASS(CDynamicEdit)))
		{
			CDynamicEdit* pDynEdit = (CDynamicEdit*)pEdit;
			pDynEdit->ShowIfRequired();
		}
		if (pEdit->IsKindOf(RUNTIME_CLASS(CDynamicCommandListBox)))
		{
			CDynamicCommandListBox* pDynEdit = (CDynamicCommandListBox*)pEdit;
			pDynEdit->ShowIfRequired();
		}
	}

	for (unsigned int i = 0; i != controlsToRedraw.size(); i++)
	{
		CWnd* pEdit = controlsToRedraw[i];

		if (pEdit && pEdit->IsWindowVisible())
			pEdit->RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_INTERNALPAINT | RDW_UPDATENOW | RDW_ALLCHILDREN);
	}
	QueryPerformanceCounter(&pc6);

	QueryPerformanceCounter(&pcElapsed);
	pcElapsed.QuadPart -= pcStart.QuadPart;

	pc6.QuadPart -= pc5.QuadPart;
	pc5.QuadPart -= pc4.QuadPart;
	pc4.QuadPart -= pc3.QuadPart;
	pc3.QuadPart -= pc2.QuadPart;
	pc2.QuadPart -= pc1.QuadPart;
	pc1.QuadPart -= pcStart.QuadPart;

	double t1 = (((double)pc1.QuadPart) / ((double)pcFreq.QuadPart)) * 1000;
	double t2 = (((double)pc2.QuadPart) / ((double)pcFreq.QuadPart)) * 1000;
	double t3 = (((double)pc3.QuadPart) / ((double)pcFreq.QuadPart)) * 1000;
	double t4 = (((double)pc4.QuadPart) / ((double)pcFreq.QuadPart)) * 1000;
	double t5 = (((double)pc5.QuadPart) / ((double)pcFreq.QuadPart)) * 1000;
	double t6 = (((double)pc6.QuadPart) / ((double)pcFreq.QuadPart)) * 1000;

	double cumulativeTime = (((double)pcElapsed.QuadPart) / ((double)pcFreq.QuadPart)) * 1000;
}

CSize IcDynamicInputUIManager::calculateTotalSize()
{
	CSize totalSize(0, 0);
	int dynMode;

	IIcadSysvarManager* pISysvar = getSysvarManagerInterface(getIcadInterfaces());
	ASSERT(pISysvar != NULL);

	int retVal = pISysvar->getVar(L"DYNMODE", dynMode);
	if (dynMode <= DynInputValues::KPointerAndDimInputOff)
		return totalSize;

	int dynPrompt;
	retVal = pISysvar->getVar(L"DYNPROMPT", dynPrompt);

	if (dynMode > DynInputValues::KPointerAndDimInputOff)
	{
		if (dynPrompt && m_BoxContainer.getPromptBox())
		{
			CSize size = m_BoxContainer.getPromptBox()->getSize();
			totalSize = size;
		}
		if (m_optionList.size() > 0)
		{
			if (m_BoxContainer.getOptionsListBox() && m_BoxContainer.getOptionsListBox()->m_hWnd && m_BoxContainer.getOptionsListBox()->IsWindowVisible())
			{
				CSize size = m_BoxContainer.getOptionsListBox()->getSize();
				totalSize.cy += size.cy + OFFSET_Y_OPTION_LIST;
				totalSize.cx = max(size.cx, totalSize.cx);
			}
		}

		if (m_BoxContainer.getCommandListBox() && m_BoxContainer.getCommandListBox()->m_hWnd &&
			(m_BoxContainer.getCommandListBox()->IsWindowVisible() || m_BoxContainer.getCommandListBox()->ShowIsRequired()))
		{
			CSize size = m_BoxContainer.getCommandListBox()->getSize();
			totalSize.cy += size.cy + OFFSET_Y_OPTION_LIST;
			totalSize.cx = max(size.cx, totalSize.cx);
		}

		if (dynMode & DynInputValues::KPointerInputOn)
		{
			if (m_BoxContainer.getModeBox() && m_BoxContainer.getModeBox()->IsWindowVisible())
			{
				CSize size = m_BoxContainer.getModeBox()->getSize();
				totalSize.cx += size.cx + SPACE_BETWEEN_EDIT;
			}
			if (m_BoxContainer.getXBox() && m_BoxContainer.getXBox()->IsWindowVisible())
			{
				CSize size = m_BoxContainer.getXBox()->getSize();
				totalSize.cx += size.cx + SPACE_BETWEEN_EDIT;
			}
			if (m_BoxContainer.getYBox() && m_BoxContainer.getYBox()->IsWindowVisible())
			{
				CSize size = m_BoxContainer.getYBox()->getSize();
				totalSize.cx += size.cx + SPACE_BETWEEN_EDIT;

			}
			if (m_BoxContainer.getZBox() && m_BoxContainer.getZBox()->IsWindowVisible())
			{
				CSize size = m_BoxContainer.getZBox()->getSize();
				totalSize.cx += size.cx + SPACE_BETWEEN_EDIT;
			}
			if (dynPrompt && m_BoxContainer.getCommandStringBox() && m_BoxContainer.getCommandStringBox()->IsWindowVisible())
			{
				CSize size = m_BoxContainer.getCommandStringBox()->getSize();
				totalSize.cx += size.cx + SPACE_BETWEEN_EDIT;
			}
			if (m_BoxContainer.getStringBox() && m_BoxContainer.getStringBox()->IsWindowVisible())
			{
				CSize size = m_BoxContainer.getStringBox()->getSize();
				totalSize.cx += size.cx + SPACE_BETWEEN_EDIT;
			}
			if (m_BoxContainer.getAngleBox() && m_BoxContainer.getAngleBox()->IsWindowVisible())
			{
				CSize size = m_BoxContainer.getAngleBox()->getSize();
				totalSize.cx += size.cx + SPACE_BETWEEN_EDIT;
			}
			if (m_BoxContainer.getRealBox() && m_BoxContainer.getRealBox()->IsWindowVisible())
			{
				CSize size = m_BoxContainer.getRealBox()->getSize();
				totalSize.cx += size.cx + SPACE_BETWEEN_EDIT;
			}
			if (m_BoxContainer.getIntBox() && m_BoxContainer.getIntBox()->IsWindowVisible())
			{
				CSize size = m_BoxContainer.getIntBox()->getSize();
				totalSize.cx += size.cx + SPACE_BETWEEN_EDIT;
			}
		}
		if (dynMode & DynInputValues::KDimInputOn)
		{
			// add also size of edit controls without dimension
			for (unsigned int i = 0; i != m_pDataControls.size(); i++)
			{
				CDynamicEdit* pEdit = m_pDataControls[i];
				if (!pEdit || !pEdit->m_hWnd || !IsWindowVisible(pEdit->m_hWnd))
					continue;
				if (m_DimData && m_DimData->size() > i)
				{
					OdDbDimData* pDimData = pEdit->dimData();
					if (!pDimData ||
						(pDimData && (pDimData->dimension().isNull() || pDimData->isDimInvisible())))
					{
						CSize size = pEdit->getSize();
						totalSize.cx += size.cx + SPACE_BETWEEN_EDIT;
					}
				}
			}
		}
	}
	return totalSize;
}

CDynamicEdit* IcDynamicInputUIManager::getActiveControl() const
{
	return m_ActiveControl;
}


void IcDynamicInputUIManager::setActiveControl(CDynamicEdit* pControl)
{
	m_ActiveControl = pControl;

	// Bug 110491 - [Dynamic Input] In some cases controls get two symbols in input
	CIcadDwgView* pView = dynamic_cast<CIcadDwgView*>(IcCoreExternal::getCurrentView());
	if (pView && pControl && pControl->IsWindowVisible() && ::GetFocus() == 0)
		pView->SetFocus();
}

OdArray<CDynamicEdit*>* IcDynamicInputUIManager::getDataControls()
{
	return &m_pDataControls;
}

void IcDynamicInputUIManager::changeActiveControl(bool considerZBox)
{
	if (getDrawer())
	{
		getDrawer()->getMyDrawList()->invalidateListToScreen();
		getDrawer()->updateScreen();
	}

	// disable current active control and lock current value
	if (m_ActiveControl)
	{
		if (!m_ActiveControl->validate(false))
			return;

		OdDbDimData* pDimData = m_ActiveControl->dimData();

		if (m_ActiveControl->getStatus() == CDynamicEdit::eStateEditing)
		{
			if (m_ActiveControl->isEmpty())
			{
				m_ActiveControl->setStatus(CDynamicEdit::eStateUnchanged);
				if (pDimData)
					pDimData->setConstrain(false);
			}
			else
			{
				m_ActiveControl->setStatus(CDynamicEdit::eStateModified);  // is changed
			}

			double dValue = 0.0;
			if (pDimData &&
				!m_ActiveControl->isEmpty() &&
				m_ActiveControl->getValue(dValue))
			{

				bool isAppDataValid = m_pAppDataMap.size() > 0;

				if (m_pCurrentJig)
				{
					if (m_pCurrentJig->setDimValue(pDimData, dValue) == Icad::eOk)
					{
						if (!pDimData->isConstrained())
							m_ActiveControl->setStatus(CDynamicEdit::eStateUnchanged);

						if (IIcadViewInternal* pView = getViewInternalInterface(getIcadInterfaces()))
						{
							if (IIcadDragger* dragger = pView->getCurrentDragger())
							{
								dragger->dragOne(*(OdGePoint3d*)NULL, pView->getDrawingSurface());
								updateDimensionValue();
							}
						}
					}
				}
				else if (isAppDataValid && m_DimData->size() > 1)
				{
					IIcadViewInternal* pView = getViewInternalInterface(getIcadInterfaces());
					ASSERT(pView != NULL);
					if (pView == NULL)
						return;

					IcGripPointManagerPtr pGripPointManager = pView->getGripPointManager();
					ASSERT(!pGripPointManager.isNull());
					if (pGripPointManager.isNull())
						return;

					pGripPointManager->DimValueSetter(pDimData, dValue);
				}
			}

			resizeControls();
		}
		m_ActiveControl->EnableWindow(false);
		m_ActiveControl->SetReadOnly(true);
	}

	if (m_state)
		m_state->changeActiveControl(this, considerZBox);

	if (m_ActiveControl)
	{
		m_ActiveControl->EnableWindow();
		m_ActiveControl->SetReadOnly(false);
		m_ActiveControl->SetSel(0, -1);
	}

	/*
	if(isActiveControl && m_ActiveControl)*/
	// sendMouseMove();


	/*
		if (IIcadViewInternal* pView = getViewInternalInterface(getIcadInterfaces()))
		{
			if (IIcadDragger * dragger = pView->getCurrentDragger())
			{
				dragger->dragOne(*(OdGePoint3d*)NULL, pView->getDrawingSurface());
				updateDimensionValue();
			}
		}*/

}

void IcDynamicInputUIManager::disableActiveControl()
{
	m_ActiveControl = nullptr;
}

bool IcDynamicInputUIManager::isForcedGetInput()
{
	int dynMode;
	int retVal = safeGetSysvarManagerInterface(getIcadInterfaces()).getVar(L"DYNMODE", dynMode);
	if (!(dynMode & DynInputValues::KDimInputOn))
		return false;

	bool isDefaultString = false;
	auto activeControl = getActiveControl();
	if (!(activeControl && activeControl->m_hWnd && activeControl->IsWindowVisible()))
		return false;

	if (activeControl && !m_defaultValue.isEmpty())
	{
		CString controlValue;
		activeControl->GetWindowTextW(controlValue);
		isDefaultString = m_defaultValue == OdString(controlValue);
	}

	return m_activeMode == IIcDynamicInputManager::eModeDimensions || isDefaultString;
}

bool IcDynamicInputUIManager::isFocused()
{
	if (CWnd::GetFocus() == nullptr)
		return false;

	return (CWnd::GetFocus() == m_ActiveControl ||
			CWnd::GetFocus() == m_BoxContainer.getOptionsListBox(false) ||
			CWnd::GetFocus() == m_BoxContainer.getCommandListBox(false));
}

bool IcDynamicInputUIManager::isOnlyKeywords()
{
	return m_isOnlyKeywords;
}

void IcDynamicInputUIManager::setOnlyKeywords(bool isOnlyKeywords)
{
	m_isOnlyKeywords = isOnlyKeywords;
}

IcString IcDynamicInputUIManager::getKeywords()
{
	IcString keywords;
	if (m_pCurrentJig)
	{
		keywords = m_pCurrentJig->keywordList();
	}

	if (keywords.isEmpty())
	{
		IIcadIOManager* pIO = getIOManagerInterface(getIcadInterfaces());
		keywords = pIO->getKeywords();
	}

	return keywords;
}
// returns
//	true if stop keyboard message
//	false if forward keyboard message
// if value is empty
bool IcDynamicInputUIManager::getCommand(bool& sendEnter, CString& command)
{
	sendEnter = false;
	command = L"";

	if (!m_ActiveControl)
		return true;  // stop keyboard message but don't send anything to command line

	if (m_ActiveControl->getStatus() == CDynamicEdit::eStateUnchanged)
	{
		sendEnter = false;
		if (CDynamicEdit* modBox = getBoxContainer()->getModeBox())
		{
			if (modBox->IsModified())
			{
				command = modBox->getText();
				return true;
			}
		}
		return false;
	}

	if (!m_ActiveControl->validCommand())
		return true;  // stop keyboard message but don't send anything to command line

	bool ret = false;
	if (m_state)
		ret = m_state->getCommand(this, sendEnter, command);

	if (m_ActiveControl->getStatus() == CDynamicEdit::eStateEditing)
	{
		m_ActiveControl->setStatus(CDynamicEdit::eStateModified);  // is changed
	}

	return ret;
}

bool IcDynamicInputUIManager::sendKey(WPARAM wParam, LPARAM lParam, bool onlyWMCHAR, bool isLispActive)
{
	if ((wParam == VK_CONTROL) || (wParam == VK_MENU) || (wParam == VK_SHIFT))
		return false;

	if (getActiveControl())
	{
		IIcadSysvarManager* pISysvar = getSysvarManagerInterface(getIcadInterfaces());
		ASSERT(pISysvar != NULL);
		int dynPrompt = 0;
		int retVal = pISysvar->getVar(L"DYNPROMPT", dynPrompt);
		int dynMode = 0;
		retVal = pISysvar->getVar(L"DYNMODE", dynMode);

		bool activeControlEnabled = getActiveControl()->IsWindowEnabled();
		bool activeControlWillBeVisible = false;
		if (onlyWMCHAR &&
			(!activeControlEnabled || (m_activeMode == IIcDynamicInputManager::eModeNotActive && dynPrompt && dynMode > 0))
			&& !isLispActive)
		{
			getActiveControl()->EnableWindow(true);
			getActiveControl()->SetReadOnly(false);
			activeControlWillBeVisible = true;
		}

		if(!activeControlWillBeVisible && !getActiveControl()->IsWindowVisible())
		{
			int dynPivis = 0;
			retVal = pISysvar->getVar(L"DYNPIVIS", dynPivis);
			if(dynPivis == DynInputValues::KOnlyWhenYouTypeAtPromptForPoint)
			{
				activeControlWillBeVisible = true;
			}
		}

		if (onlyWMCHAR && (getActiveControl()->IsWindowVisible() || activeControlWillBeVisible))
		{
			getActiveControl()->SendMessage(WM_CHAR, (WPARAM)wParam, lParam);

			if (wParam != VK_ESCAPE) // to avoid VK_DOWN, VK_UP etc
			{
				getActiveControl()->setStatus(CDynamicEdit::eStateEditing);
				if ((activeControlWillBeVisible || !activeControlEnabled) && m_activeMode == IIcDynamicInputManager::eModeCoordinate)
				{
					showCoordinateBoxes();
				}

				if (m_activeMode == IIcDynamicInputManager::eModeNotActive && dynPrompt)
				{
					CDynamicEdit* commandBox = m_BoxContainer.getCommandStringBox(true);
					commandBox->postPoneShow();

					UINT nChar = (UINT)wParam;
					if (nChar != L'`' &&
						nChar != L',' &&
						nChar != L'.' &&
						nChar != L'@' &&
						nChar != L'#' &&
						nChar != L'~' &&
						nChar != L'[' &&
						nChar != L']')
						displayCommandList(wParam != VK_DELETE && wParam != VK_BACK);
					else
						destroyCommandList();
				}
			}
			resizeControls();
			return true;
		}
	}
	return false;
}


void IcDynamicInputUIManager::postMouseMove(int x, int y)
{
	// only main thread can create edit controls, simulate MouseMove message
	CWnd* currentView = icedGetIcadDwgView();
	// change x coordinate by one pixel to avoid discard of this message
	currentView->PostMessage(WM_MOUSEMOVE, 0, MAKELPARAM(x, y));
}

void IcDynamicInputUIManager::startJig(IcEdBaseJig* pJig)
{
	m_pCurrentJig = pJig;
	if (m_pCurrentJig)
	{
		m_pAppDataMap.clear();
		IIcadSysvarManager* pISysvar = getSysvarManagerInterface(getIcadInterfaces());
		ASSERT(pISysvar != NULL);

		int dynMode;
		int retVal = pISysvar->getVar(L"DYNMODE", dynMode);
		IIcadGlobalVariablesInternal* pGlobalvars = getIcadGlobalVariablesInternal(getIcadInterfaces());
		if (dynMode & DynInputValues::KDimInputOn &&
			!(pGlobalvars && pGlobalvars->isLispActive())) // dimension mode is disabled by dynmode variable
		{
			// this creates dimensions entity dragged by jig
			double zoomScale = 1.0;
			IIcadViewInternal* pView = getViewInternalInterface(getIcadInterfaces());
			IcGsDrawingSurfaceBasePtr spDrawingSurface = pView->getDrawingSurface();
			if (!spDrawingSurface.isNull())
			{
				OdGsViewPtr spActiveView = spDrawingSurface->activeView();
				if (!spActiveView.isNull())
				{
					zoomScale = spActiveView->projectionMatrix().inverse().scale();
				}
			}

			OdArray<OdDbDimData*>* dimData = m_pCurrentJig->dimData(zoomScale);
			setDimData(dimData);
			if (m_pCurrentJig->getUseGraphicDB() && getDrawer())
			{
				getDrawer()->switchToTransientGiContext(true);
			}
		}
		else
		{
			setMode(getAlternativeMode());
		}
	}
}

void IcDynamicInputUIManager::endJig()
{
	if (m_pCurrentJig && m_pCurrentJig->getUseGraphicDB() && getDrawer())
	{
		getDrawer()->switchToTransientGiContext(false);
	}
	setMode(IIcDynamicInputManager::eModeNotActive);
	setDimData(nullptr);
	m_pCurrentJig = nullptr;
	m_oldPromptText.empty();
}

void IcDynamicInputUIManager::startGrip(IcArray<IcGripPointPtr> gripPoints, bool isHover)
{
	setDimData(nullptr);

	m_pCurrentJig = NULL;
	// this creates dimensions entity dragged by pAppData
	double zoomScale = 1.0;
	IIcadViewInternal* pView = getViewInternalInterface(getIcadInterfaces());
	if (IcGsDrawingSurfaceBasePtr spDrawingSurface = pView->getDrawingSurface(); !spDrawingSurface.isNull())
	{
		if (OdGsViewPtr spActiveView = spDrawingSurface->activeView(); !spActiveView.isNull())
		{
			zoomScale = spActiveView->projectionMatrix().inverse().scale();
		}
	}

	if (!m_OwnDimData.isEmpty())
	{
		m_OwnDimData.clear();
	}

	DynInputUIDimensionsState::getInstance()->setHoverGrip(isHover);

	IIcadSysvarManager* pISysvar = getSysvarManagerInterface(getIcadInterfaces());
	ASSERT(pISysvar != NULL);

	int dynMode;
	int retVal = pISysvar->getVar(L"DYNMODE", dynMode);
	if (dynMode & DynInputValues::KDimInputOn)
	{
		for (IcGripPointPtr spGripPoint : gripPoints)
		{
			IcGripDataPointPtr spGripDataPoint = IcGripDataPoint::cast(spGripPoint);
			if (!spGripDataPoint.isNull())
			{
				OdDbDimDataPtrArray dimData;
				OdDbGripDataPtr spGripData = spGripDataPoint->getGripData();
				OdDbStub* entityId = spGripDataPoint->entityId();

				if (spGripData->hotGripDimensionFunc() && !isHover)
				{
					spGripData->hotGripDimensionFunc()(spGripData, entityId, zoomScale, dimData);
				}
				else if (spGripData->hoverDimensionFunc() && isHover)
				{
					spGripData->hoverDimensionFunc()(spGripData, entityId, zoomScale, dimData);
				}

				if (!dimData.isEmpty())
				{
					for (auto& dData : dimData)
					{
						m_OwnDimData.push_back(dData);
					}
					m_pAppDataMap[spGripData] = entityId;
				}
			}
		}

		if (!m_OwnDimData.isEmpty())
		{
			setDimData((OdArray<OdDbDimData*>*)&m_OwnDimData);
		}
		else
		{
			if (!isHover)
			{
				setMode(IIcDynamicInputManager::eModeCoordinate);
			}
		}
		
	}
	else
	{
		if (!isHover)
		{
			setMode(getAlternativeMode());
		}
	}
}

void IcDynamicInputUIManager::endGrip()
{
	if(DynInputUIDimensionsState::getInstance()->isHoverGrip())
	{
		setMode(IIcDynamicInputManager::eModeNotActive);
	}

	setDimData(nullptr);
	m_OwnDimData.clear();
	m_pAppDataMap.clear();
	DynInputUIDimensionsState::getInstance()->setHoverGrip(false);

	m_oldPromptText.empty();
}

bool IcDynamicInputUIManager::setTooltipText(OdString text)
{
	if (m_activeMode == IIcDynamicInputManager::eModeNotActive ||
		!(m_BoxContainer.getPromptBox() && m_BoxContainer.getPromptBox()->IsWindowVisible()))
		return false;

	bool wasTextEmpty = m_tooltipText.isEmpty();
	if (!text.isEmpty())
	{
		if (m_tooltipText != text)
		{
			m_tooltipText = text;

			if (m_BoxContainer.getPromptBox() && wasTextEmpty)
			{
				CString oldText;
				m_BoxContainer.getPromptBox()->GetWindowTextW(oldText);
				m_oldPromptText = oldText;
				m_oldIsDisplayArrow = m_BoxContainer.getPromptBox()->getDisplayDownArrow();
				m_BoxContainer.getPromptBox()->setDisplayDownArrow(false);
				m_BoxContainer.getPromptBox()->SetWindowTextW(m_tooltipText);
			}
		}
		return true;
	}
	return false;
}

void IcDynamicInputUIManager::resetTooltipText()
{
	m_tooltipText.empty();

	if (!m_oldPromptText.isEmpty() && m_BoxContainer.getPromptBox())
	{
		m_BoxContainer.getPromptBox()->SetWindowTextW(m_oldPromptText);
		m_oldPromptText.empty();
		m_BoxContainer.getPromptBox()->setDisplayDownArrow(m_oldIsDisplayArrow);
	}
}

void IcDynamicInputUIManager::saveParams()
{
	m_RestoringParams.Mode = m_activeMode;

	/*	m_RestoringParams.DimData = m_DimData;
		m_RestoringParams.OwnDimData = m_OwnDimData;

		m_RestoringParams.ControlStrings.clear();
		for (int i = 0; i != m_pDataControls.size(); i++)
		{
			if (m_pDataControls[i])
			{
				m_RestoringParams.ControlStrings.push_back(m_pDataControls[i]->getText());
			}
		}*/
}

void IcDynamicInputUIManager::restoreParams()
{
	setMode(m_RestoringParams.Mode);
	/*setDimData(m_RestoringParams.DimData);
	m_OwnDimData = m_RestoringParams.OwnDimData;
	m_RestoringParams.OwnDimData.clear();
	m_RestoringParams.DimData = NULL;

	int arrayIndex = 0;
	if (m_RestoringParams.ControlStrings.size() > 0)
	{
		for (int i = 0; i != m_pDataControls.size(); i++)
		{
			if (m_pDataControls[i])
			{
				m_pDataControls[i]->SetWindowTextW(m_RestoringParams.ControlStrings[arrayIndex]);
				arrayIndex++;
				if (arrayIndex >= (int)m_RestoringParams.ControlStrings.size())
				{
					break;
				}
			}
		}

		m_RestoringParams.ControlStrings.clear();
	}*/
}

bool IcDynamicInputUIManager::hasEnteredText()
{
	if (!getActiveControl() || !getActiveControl()->IsWindowVisible())
		return false;
	// validate values in edit box
	return (!getActiveControl()->IsUnchanged());
}

void IcDynamicInputUIManager::displayPromptBox()
{
	if (m_BoxContainer.getPromptBox())
		m_BoxContainer.getPromptBox()->Show();
}

bool IcDynamicInputUIManager::isActiveControl()
{
	if (getActiveControl() && !getActiveControl()->IsWindowVisible())
		return false;
	return true;
}

void IcDynamicInputUIManager::sendMouseMove()
{
	// only main thread can create edit controls, simulate MouseMove message
	CWnd* currentView = icedGetIcadDwgView();

	// avoid to send another mousemove if one is already in queque
	MSG msg;
	bool gotMouseMove = false;
	while (::PeekMessage(&msg, currentView->m_hWnd, WM_MOUSEMOVE, WM_MOUSEMOVE, PM_NOREMOVE | PM_QS_INPUT))
	{
		if (msg.message == WM_MOUSEMOVE)
		{
			gotMouseMove = true;
			break;
		}
	}
	// change x coordinate by one pixel to avoid discard of this message
	if (!gotMouseMove)
		currentView->SendMessage(WM_MOUSEMOVE, 0, MAKELPARAM(m_lastCursorPos.x - 1, m_lastCursorPos.y));

	CWnd* icadFrame = icedGetIcadDwgView();
	if (!PeekMessage(&msg, icadFrame->m_hWnd, WM_MOUSEFIRST, WM_MOUSELAST, PM_NOYIELD))
	{
		OdGePoint3d dummyPt;
		onMouseMove(dummyPt);
	}
}

// if not dynmode & 2 choose an alternative mode depending on dimdata from jig
IIcDynamicInputManager::ActiveMode IcDynamicInputUIManager::getAlternativeMode()
{
	IIcDynamicInputManager::ActiveMode alternativeMode = IIcDynamicInputManager::eModeCoordinate;
	if (m_DimData && m_DimData->size() == 1)
	{
		OdDbDimData* dimData = m_DimData->getAt(0);
		if (dimData->isDimDeltaAngle() || dimData->isDimResultantAngle())
			alternativeMode = IIcDynamicInputManager::eModeGetAngle;
		else if (dimData->isDimDeltaLength() || dimData->isDimResultantLength() || dimData->isDimRadius())
			alternativeMode = IIcDynamicInputManager::eModeGetDist;
		else if (dimData->isCustomString())
			alternativeMode = IIcDynamicInputManager::eModeGetStringWithSpace;
		else if (dimData->isCustomDimValue())
			alternativeMode = IIcDynamicInputManager::eModeGetReal;
	}
	return alternativeMode;
}

void IcDynamicInputUIManager::showCoordinateBoxes()
{
	IIcadSysvarManager* pISysvar = getSysvarManagerInterface(getIcadInterfaces());
	ASSERT(pISysvar != NULL);

	CDynamicEdit* xBox = m_BoxContainer.getXBox(true);
	CDynamicEdit* yBox = m_BoxContainer.getYBox(true);
	if (xBox && yBox)
	{
		int lastPointInit = 0;
		int retVal = pISysvar->getVar(L"LASTPOINTINIT", lastPointInit);
		ASSERT(retVal == RTNORM);
		int dynPICoords;
		retVal = pISysvar->getVar(L"DYNPICOORDS", dynPICoords);
		ASSERT(retVal == RTNORM);

		xBox->postPoneShow();
		yBox->postPoneShow();
		if (lastPointInit)
		{
			int dynPiDispModeBox = 0;
			retVal = pISysvar->getVar(L"DYNPIDISPMODEBOX", dynPiDispModeBox);
			bool showMode = ((dynPiDispModeBox == 1)
				|| (getBoxContainer()->getXBox(false)->isRelative() != (dynPICoords == DynInputValues::KRelative)));
			if (showMode)
			{
				if (CDynamicStringPromptEdit* modeBox = getBoxContainer()->getModeBox(true))
					modeBox->postPoneShow();
			}
		}

		xBox->setIsRelative(lastPointInit == 1 && dynPICoords == DynInputValues::KRelative);
		getBoxContainer()->getModeBox()->setValue(xBox->isRelative() ? L"@" : L"#");

		int dynPIFormat = 0;
		retVal = pISysvar->getVar(L"DYNPIFORMAT", dynPIFormat);
		ASSERT(retVal == RTNORM);
		yBox->setIsAngular(lastPointInit == 1 && dynPIFormat == DynInputValues::KPolar);

		int dynZBox = 0;
		retVal = pISysvar->getVar(L"DYNZBOX", dynZBox);
		ASSERT(retVal == RTNORM);
		if ((lastPointInit == 0 || dynPIFormat == DynInputValues::KCartesian) &&
			dynZBox == DynInputValues::KShowZFieldAlways)
		{
			if (CDynamicEdit* zBox = getBoxContainer()->getZBox(true))
			{
				zBox->postPoneShow();
			}
		}
	}
}

// progesoft
// to count command usage
IcDynamicInputUIManager::CommandCounter::CommandCounter()
{
	OdStringArray commandsArray;
	IcString lastCommands;
	if (icedGetCfg(L"LAST_USED_COMMANDS", (wchar_t*)lastCommands.getBuffer(10000), 10000) == RTNORM)
	{
		lastCommands.releaseBuffer();
		lastCommands.splitString(L";", commandsArray);
		for (int i = 0; i != commandsArray.size(); i++)
		{
			m_lastCommands.push_back(commandsArray[i]);
			commandWillStart(commandsArray[i]);
		}
	}
	icEditor()->addReactor(this);

	buildCache();
}

IcDynamicInputUIManager::CommandCounter::~CommandCounter()
{
	IcString lastCommands;
	std::list<OdString>::const_iterator iter = m_lastCommands.begin();
	for (; iter != m_lastCommands.end(); iter++) { lastCommands += (*iter) + L";"; }
	icedSetCfg(L"LAST_USED_COMMANDS", (LPCTSTR)lastCommands);

	icEditor()->removeReactor(this);
}
#define MAX_COMMANDS_NO 100
void IcDynamicInputUIManager::CommandCounter::commandWillStart(const OdString& cmdStr)
{
	if (cmdStr == L"_NETAUTOSTARTAPP" || cmdStr == L"_DRAWINGRECOVERY")
		return;

	while (m_lastCommands.size() > MAX_COMMANDS_NO) { m_lastCommands.pop_back(); }

	// search locale and international command name
	OdEdCommandPtr pCmd = ::odedRegCmds()->lookupCmd(cmdStr);

	OdString localeName = pCmd.isNull() ? cmdStr : pCmd->localName();
	OdString intlName = pCmd.isNull() ? cmdStr : pCmd->globalName();
	if (intlName.left(1) != L"_")
		intlName = L"_" + intlName;
	m_lastCommands.push_front(intlName);

	buildCache();
}

void IcDynamicInputUIManager::CommandCounter::buildCache()
{
	m_lastCommandsCache.clear();
	for each (OdString st in m_lastCommands)
	{
		std::map<OdString, int>::iterator iter;
		if ((iter = m_lastCommandsCache.find(st)) != m_lastCommandsCache.end())
		{
			iter->second++;
		}
		else
		{
			m_lastCommandsCache.insert(std::pair(st, 1));
		}
	}
}

int IcDynamicInputUIManager::CommandCounter::getCommandCount(const OdString& cmdStr)
{
	int ret = 0;

	OdString intCmdStr = cmdStr.left(0) == L"_" ? cmdStr : L"_" + cmdStr;
	intCmdStr.makeUpper();

	std::map<OdString, int>::iterator iter;
	if ((iter = m_lastCommandsCache.find(intCmdStr)) != m_lastCommandsCache.end())
	{
		return iter->second;
	}
	else
	{
		return 0;
	}
	return ret;
}
