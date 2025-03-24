/*****************************************************************************
***
***    Copyright (C) 2014 ProgeSOFT sa. All Rights Reserved.
***     All rights reserved.
***
***     Use of the information contained herein, in part or in whole,
***     in/as source code and/or in/as object code, in any way by anyone
***     other than authorized employees of The ProgeSOFT sa,
***     or by anyone to whom The ProgeSOFT sa has not
***     granted use is illegal.
***
***     Description:
***
*****************************************************************************/
#pragma once
//#include "stdafx.h"
#include "OdaCommon.h"
#include "DbDimData.h"
#include "IcDynamicInputControls.h"
#include "IcDynamicInputUIBoxContainer.h"
#include "IcadControlFlow\ThreadManager\IcCriticalSection.h"
#include <IcadCore/DrawTools/IcDrawer.h>
#include "IcadCommon/Interfaces/IIcDynamicInputManager.h"
#include <vector>
#include "IcDynamicInputUIStates.h"
#include <IcadCui/IcAliasesManager.h>

#include "IcEditorReactor.h"
#include <list>
UINT vkToChar(UINT code, UINT flags);

//**********************************************************************************************************************
//
//	Dynamic input UI manager
//
//	manages all controls used by Dynamic Input features
//
struct ItemsCommand
{
	// progesoft
	// added command counter usage
	//ItemsCommand(CString name, bool isSysVar = false) :m_name(name), m_isSysVar(isSysVar) , m_counter(0) {}
	ItemsCommand(CString name, int counter, bool isSysVar = false) :m_name(name), m_isSysVar(isSysVar) , m_counter(counter) {}
	CString m_name;
	bool m_isSysVar;
	// progesoft
	int m_counter;
};

class ICADCOMMANDGUI_EXPORT IcDynamicInputUIManager
{
private:
	DynInputUIBaseState * m_state;

	// progesoft
	class CommandCounter : public IcEditorReactor
	{
public:
		CommandCounter();
		~CommandCounter();

		void commandWillStart(const OdString& cmdStr) override;
		int getCommandCount(const OdString &cmdStr);

	protected:
		std::list<OdString> m_lastCommands;
		std::map<OdString, int> m_lastCommandsCache;

		void buildCache();
	};

public:
	IcDynamicInputUIManager();
	~IcDynamicInputUIManager();

	void								resizeControls();


	void 								initialize();
	void								setPrompt(OdString prompt, bool threadRedir = false);
	IIcDynamicInputManager::ActiveMode	setMode(IIcDynamicInputManager::ActiveMode mode);
	IIcDynamicInputManager::ActiveMode	getMode() const
	{
		return m_activeMode;
	}

	bool								validate();
	void								setDimData(OdArray<OdDbDimData*>* dimData);
	OdArray<OdDbDimData*>*				getDimData();
	bool								isDimVisible(OdDbDimData* dimData);
	// returns true if dynamic input changed point (last parameter)
	bool								onMouseMove(const CPoint pt, OdGePoint3d ucsPoint, OdGePoint3d &ucsModifiedPoint);

	bool								onLButtonDown(CPoint pt);
	bool								onRButtonDown(OdGsViewPtr spView, CPoint &dcPoint);
	bool								onKeyboardMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, bool isLispActive = false);
	// updates value in dimensions edit box after jig update
	void								updateDimensionValue();

	// called to update size of dimensions (after zoom command)
	//void								updateDimensions();

	// change clicked point to modified by constraints
	OdGePoint3d							getLastUcsPoint();
	OdGePoint3d							getLastWcsPoint() const;
	// move focus on next active edit control
	void								changeActiveControl(bool considerZBox);

	// returns command string to send to command line
	bool								getCommand(bool &sendEnter, CString &value);

	// send key to active edit box
	bool								sendKey(WPARAM wParam, LPARAM lParam, bool onlyWMCHAR = false, bool isLispActive = false);

	// true : display option list and select default one
	// false : there are no options in list
	bool								displayOptionList(int userInputControls);
	bool								hasOptions();
	void								destroyOptionList();
	bool								isDisplayOptionList();

	bool								displayCommandList(bool autocomplete = false, bool forced = false, bool displayList = true);
	void								destroyCommandList();
	bool								isDisplayCommandList();

	bool								isProcessControlEditing();

	bool								isPointModified() { return m_bModified; };

	//void								updateGraphicSurface();

	void								postMouseMove(int x, int y);
	void								sendMouseMove();

	// mark update required to update dimensions and controls (it send a mousemove in setMode)
	void								setUpdateRequired() { m_bUpdateRequired = true; };

	void								startJig(IcEdBaseJig* pJig);
	void								endJig();
	void								startGrip(IcArray<IcGripPointPtr> gripPoints, bool isHover = false);
	void								endGrip();

	bool							    setTooltipText(OdString text);
	void							    resetTooltipText();
    bool                                isTooltipTextEmpty() const { return m_tooltipText.isEmpty(); }

	void						        saveParams();
	void						        restoreParams();

	bool								hasEnteredText();
	void								displayPromptBox();
	bool								isActiveControl();
	bool								onMouseMove(OdGePoint3d&);

	IcString							getKeywords();
	CDynamicEdit*						getActiveControl() const;
	void								setActiveControl(CDynamicEdit* pControl);

    IcDynamicInputBoxContainer*         getBoxContainer() { return &m_BoxContainer; }
	OdArray<CDynamicEdit*>*				getDataControls();
	void								setLastUcsPoint(const OdGePoint3d point, bool setIsModified = true);
	OdString							getDefaultValue() { return m_defaultValue; }
	OdStringArray*						getOptionList() { return &m_optionList; }
	IcEdBaseJig*						getCurrentJig() { return m_pCurrentJig; }
	std::pair<OdDbGripDataPtr, OdDbObjectId> getAppDataMapFirstElement() { return (!m_pAppDataMap.empty()? *m_pAppDataMap.begin(): std::pair<OdDbGripDataPtr, OdDbObjectId>()); }
	void								deactivate(bool bClearState = true);
	void								activate();
	int									countDimData();
	IcDrawer*							getDrawer() { return m_Drawer; }
	void								setDrawer(IcDrawer* pDrawer) { m_Drawer = pDrawer; }
	void								disableActiveControl();
	bool								isFocused();
	bool								isForcedGetInput();
	bool								isOnlyKeywords();
	void								setOnlyKeywords(bool isOnlyKeywords);

	// progesoft
	// allow to use arrow cursor instead of crosshair
	bool								useArrowCursor();
	void								getOptionListAndDefault(OdString& text, bool threadRedir);

protected:
	CSize								calculateTotalSize();
	void								resetValues();

	// if not dynmode & 2 choose an alternative mode depending on dimdata from jig
	IIcDynamicInputManager::ActiveMode	getAlternativeMode();
	void								showCoordinateBoxes();
protected:
    IcDynamicInputBoxContainer          m_BoxContainer;

	CDynamicEdit*						m_ActiveControl;

	// use this array to get information
	OdArray<OdDbDimData*>*				m_DimData;

	CPoint								m_lastCursorPos;
	OdGePoint3d							m_lastWcsPoint;

	OdArray<CDynamicEdit*>				m_pDataControls;

	IIcDynamicInputManager::ActiveMode	m_activeMode;
	CString								m_OldValue;

	bool								m_enableControlsPosition;

	OdStringArray						m_optionList;
	OdStringArray						m_globalOptionList;

	OdString							m_defaultValue;

    OdString                            m_tooltipText;
    OdString                            m_oldPromptText;
    bool                                m_oldIsDisplayArrow;

	CriticalSection						m_criticalSection;
	bool								m_bModified;

	bool								m_bUpdateRequired;

	IcEdBaseJig*						m_pCurrentJig;
	std::map<OdDbGripDataPtr, OdDbObjectId>	m_pAppDataMap;
	OdDbDimDataPtrArray					m_OwnDimData;
	IcDrawer*							m_Drawer;

	std::vector<ItemsCommand>			allItemsForList;

	// progesoft
	// counts command started
	CommandCounter						m_commandCounter;

	bool								m_isOnlyKeywords;
	struct RestoringParams
	{
		RestoringParams() :
			Mode(IIcDynamicInputManager::ActiveMode::eModeNotActive),
			DimData(NULL) {}
		IIcDynamicInputManager::ActiveMode Mode;
		OdArray<OdDbDimData*>*			   DimData;
		OdDbDimDataPtrArray				   OwnDimData;
		std::vector<CString>               ControlStrings;
	};
	RestoringParams                     m_RestoringParams;
};
