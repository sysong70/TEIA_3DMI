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
#include "IcDynamicInputControls.h"


class ICADCOMMANDGUI_EXPORT IcDynamicInputBoxContainer
{
public:
	IcDynamicInputBoxContainer( IcDynamicInputUIManager& rUIManager,
        CPoint& rPointContainer,
        IIcDynamicInputManager::ActiveMode& rModeContainer,
        OdStringArray& rOptionList);
	~IcDynamicInputBoxContainer();


	void 								initialize();

	CDynamicPrompt*						getPromptBox(bool create = false);
	CDynamicStringPromptEdit*			getModeBox(bool create = false);
	CDynamicEdit*						getXBox(bool create = false);
	CDynamicEdit*						getYBox(bool create = false);
	CDynamicEdit*						getZBox(bool create = false);
	CDynamicOptionListBox*				getOptionsListBox(bool create = false);
	CDynamicCommandListBox*				getCommandListBox(bool create = false);
	CDynamicEdit*						getIntBox(bool create = false);
	CDynamicEdit*						getRealBox(bool create = false);
	CDynamicEdit*						getAngleBox(bool create = false);
	CDynamicEdit*						getStringBox(bool create = false);
	CDynamicEdit*						getCommandStringBox(bool create = false);

	bool								createEditBox(int deltax, int deltay, bool enabled, CDynamicEdit *pNewControl);
	CDynamicOptionListBox*				createOptionListBox();
	CDynamicCommandListBox*				createCommandListBox();
	void								deleteBox(CWnd* dynamicEditBox);
	void								updateFont();
protected:
	void								createFont();
	// graphic controls
	CDynamicPrompt*						m_promptBox;

	CDynamicStringPromptEdit*			m_modeBox;

	CDynamicRealEdit*					m_XBox;
	CDynamicRealEdit*					m_YBox;
	CDynamicRealEdit*					m_ZBox;

	CDynamicRealEdit*					m_RealBox;
	CDynamicIntEdit*					m_IntBox;
	CDynamicAngleEdit*					m_AngleBox;
	CDynamicStringEdit*					m_StringBox;
	CDynamicCommandStringEdit*			m_CommandBox;

	CDynamicOptionListBox*				m_optionsListControl;
	CDynamicCommandListBox*				m_commandListControl;
	CFont								m_Font;

	CPoint&								m_lastCursorPos;
	IIcDynamicInputManager::ActiveMode&	m_activeMode;
	OdStringArray&						m_optionList;
	IcDynamicInputUIManager&            m_rUIManager;	// parent ui manager
};