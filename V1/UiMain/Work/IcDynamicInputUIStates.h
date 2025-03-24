/***************************************************************************
***
***     Copyright (C) 2020-2024 IntelliCAD Technology Consortium. All Rights Reserved.
***
***     Use of the information contained herein, in part or in whole,
***     in/as source code and/or in/as object code, in any way by anyone
***     other than authorized employees of The IntelliCAD Technology Consortium,
***     or by anyone to whom The IntelliCAD Technology Consortium  has not
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
#include "IcadControlFlow\ThreadManager\IcCriticalSection.h"
#include "IcadCommon/Interfaces/IIcDynamicInputManager.h"
#include <vector>


#define DECLARE_GET_INSTANCE( ClassName ) \
    static ClassName* getInstance() {\
        static ClassName instance;\
        return &instance;\
    }
//======================================================================================
//DynInputUIBaseState
class DynInputUIBaseState
{
public:
	virtual ~DynInputUIBaseState() { }
	virtual void changeActiveControl(IcDynamicInputUIManager* manager, bool considerZBox) = 0;
	virtual void resetValues(IcDynamicInputUIManager* manager) = 0;
	virtual void deactivate(IcDynamicInputUIManager* manager, bool bClearState = true) = 0;
	virtual bool onMouseMove(IcDynamicInputUIManager* manager) = 0;
	virtual void activate(IcDynamicInputUIManager* manager) = 0;
	virtual bool getCommand(IcDynamicInputUIManager* manager, bool &sendEnter, CString &command) = 0;

protected:
	DynInputUIBaseState() { }
};
//DynInputUIBaseState
//======================================================================================

//======================================================================================
//DynInputUINotActiveState
class DynInputUINotActiveState : public DynInputUIBaseState {
public:
	DECLARE_GET_INSTANCE(DynInputUINotActiveState)
	void changeActiveControl(IcDynamicInputUIManager* manager, bool considerZBox) override;
	void resetValues(IcDynamicInputUIManager* manager) override {};
	void deactivate(IcDynamicInputUIManager* manager, bool bClearState = true) override;
	bool onMouseMove(IcDynamicInputUIManager* manager) override;
	void activate(IcDynamicInputUIManager* manager) override;
	bool getCommand(IcDynamicInputUIManager* manager, bool &sendEnter, CString &command) override;
private:
	DynInputUINotActiveState() { }
};
//DynInputUINotActiveState
//======================================================================================

//======================================================================================
//DynInputUIOnlyPromptState
class DynInputUIOnlyPromptState : public DynInputUIBaseState {
public:
	DECLARE_GET_INSTANCE(DynInputUIOnlyPromptState)
	void changeActiveControl(IcDynamicInputUIManager* manager, bool considerZBox) override {};
	void resetValues(IcDynamicInputUIManager* manager)  override {};
	void deactivate(IcDynamicInputUIManager* manager, bool bClearState = true) override;
	bool onMouseMove(IcDynamicInputUIManager* manager) override;
	void activate(IcDynamicInputUIManager* manager)  override {};
	bool getCommand(IcDynamicInputUIManager* manager, bool &sendEnter, CString &command) override { return false; };
private:
	DynInputUIOnlyPromptState() { }
};
//DynInputUIOnlyPromptState
//======================================================================================

//======================================================================================
//DynInputUICoordinateState
class DynInputUICoordinateState : public DynInputUIBaseState {
public:
	DECLARE_GET_INSTANCE(DynInputUICoordinateState)
	void changeActiveControl(IcDynamicInputUIManager* manager, bool considerZBox) override;
	void resetValues(IcDynamicInputUIManager* manager) override;
	void deactivate(IcDynamicInputUIManager* manager, bool bClearState = true) override;
	bool onMouseMove(IcDynamicInputUIManager* manager) override;
	void activate(IcDynamicInputUIManager* manager) override;
	bool getCommand(IcDynamicInputUIManager* manager, bool &sendEnter, CString &command) override;

private:
	DynInputUICoordinateState() { }
};
//DynInputUICoordinateState
//======================================================================================

//======================================================================================
//DynInputUIGetRealState
class DynInputUIGetRealState : public DynInputUIBaseState {
public:
	DECLARE_GET_INSTANCE(DynInputUIGetRealState)
	void changeActiveControl(IcDynamicInputUIManager* manager, bool considerZBox) override;
	void resetValues(IcDynamicInputUIManager* manager) override;
	void deactivate(IcDynamicInputUIManager* manager, bool bClearState = true) override;
	bool onMouseMove(IcDynamicInputUIManager* manager) override;
	void activate(IcDynamicInputUIManager* manager)  override {};
	bool getCommand(IcDynamicInputUIManager* manager, bool &sendEnter, CString &command) override;
private:
	DynInputUIGetRealState() { }
};
//DynInputUIGetRealState
//======================================================================================

//======================================================================================
//DynInputUIGetIntState
class DynInputUIGetIntState : public DynInputUIBaseState {
public:
	DECLARE_GET_INSTANCE(DynInputUIGetIntState)
	void changeActiveControl(IcDynamicInputUIManager* manager, bool considerZBox) override;
	void resetValues(IcDynamicInputUIManager* manager) override;
	void deactivate(IcDynamicInputUIManager* manager, bool bClearState = true) override;
	bool onMouseMove(IcDynamicInputUIManager* manager) override;
	void activate(IcDynamicInputUIManager* manager)  override {};
	bool getCommand(IcDynamicInputUIManager* manager, bool &sendEnter, CString &command) override;
private:
	DynInputUIGetIntState() { }
};
//DynInputUIGetIntState
//======================================================================================

//======================================================================================
//DynInputUIGetDistState
class DynInputUIGetDistState : public DynInputUIBaseState {
public:
	DECLARE_GET_INSTANCE(DynInputUIGetDistState)
	void changeActiveControl(IcDynamicInputUIManager* manager, bool considerZBox) override;
	void resetValues(IcDynamicInputUIManager* manager) override;
	void deactivate(IcDynamicInputUIManager* manager, bool bClearState = true) override;
	bool onMouseMove(IcDynamicInputUIManager* manager) override;
	void activate(IcDynamicInputUIManager* manager)  override {};
	bool getCommand(IcDynamicInputUIManager* manager, bool &sendEnter, CString &command) override;
private:
	DynInputUIGetDistState() { }
};
//DynInputUIGetDistState
//======================================================================================

//======================================================================================
//DynInputUIGetAngleState
class DynInputUIGetAngleState : public DynInputUIBaseState {
public:
	DECLARE_GET_INSTANCE(DynInputUIGetAngleState)
	void changeActiveControl(IcDynamicInputUIManager* manager, bool considerZBox) override;
	void resetValues(IcDynamicInputUIManager* manager) override;
	void deactivate(IcDynamicInputUIManager* manager, bool bClearState = true) override;
	bool onMouseMove(IcDynamicInputUIManager* manager) override;
	void activate(IcDynamicInputUIManager* manager) override {};
	bool getCommand(IcDynamicInputUIManager* manager, bool &sendEnter, CString &command) override;
private:
	DynInputUIGetAngleState() { }
};
//DynInputUIGetAngleState
//======================================================================================

//======================================================================================
//DynInputUIGetStringState
class DynInputUIGetStringState : public DynInputUIBaseState {
public:
	DECLARE_GET_INSTANCE(DynInputUIGetStringState)
	void changeActiveControl(IcDynamicInputUIManager* manager, bool considerZBox) override;
	void resetValues(IcDynamicInputUIManager* manager) override;
	void deactivate(IcDynamicInputUIManager* manager, bool bClearState = true) override;
	bool onMouseMove(IcDynamicInputUIManager* manager) override;
	void activate(IcDynamicInputUIManager* manager) override;
	bool getCommand(IcDynamicInputUIManager* manager, bool &sendEnter, CString &command) override;
private:
	DynInputUIGetStringState() { }
};
//DynInputUIGetStringState
//======================================================================================

//======================================================================================
//DynInputUIGetStringWithSpaceState
class DynInputUIGetStringWithSpaceState : public DynInputUIBaseState {
public:
	DECLARE_GET_INSTANCE(DynInputUIGetStringWithSpaceState)
	void changeActiveControl(IcDynamicInputUIManager* manager, bool considerZBox) override;
	void resetValues(IcDynamicInputUIManager* manager) override;
	void deactivate(IcDynamicInputUIManager* manager, bool bClearState = true) override;
	bool onMouseMove(IcDynamicInputUIManager* manager) override;
	void activate(IcDynamicInputUIManager* manager) override;
	bool getCommand(IcDynamicInputUIManager* manager, bool &sendEnter, CString &command) override;
private:
	DynInputUIGetStringWithSpaceState() { }
};
//DynInputUIGetStringWithSpaceState
//======================================================================================

//======================================================================================
//DynInputUIGetOrientState
class DynInputUIGetOrientState : public DynInputUIBaseState {
public:
	DECLARE_GET_INSTANCE(DynInputUIGetOrientState)
	void changeActiveControl(IcDynamicInputUIManager* manager, bool considerZBox) override;
	void resetValues(IcDynamicInputUIManager* manager) override;
	void deactivate(IcDynamicInputUIManager* manager, bool bClearState = true) override;
	bool onMouseMove(IcDynamicInputUIManager* manager) override;
	void activate(IcDynamicInputUIManager* manager)  override {};
	bool getCommand(IcDynamicInputUIManager* manager, bool &sendEnter, CString &command) override;
private:
	DynInputUIGetOrientState() { }
};
//DynInputUIGetOrientState
//======================================================================================

//======================================================================================
//DynInputUIDimensionsState
class DynInputUIDimensionsState : public DynInputUIBaseState {
public:
	DECLARE_GET_INSTANCE(DynInputUIDimensionsState)
	void changeActiveControl(IcDynamicInputUIManager* manager, bool considerZBox) override;
	void resetValues(IcDynamicInputUIManager* manager) override;
	void deactivate(IcDynamicInputUIManager* manager, bool bClearState = true) override;
	bool onMouseMove(IcDynamicInputUIManager* manager) override;
	void activate(IcDynamicInputUIManager* manager) override;
	bool getCommand(IcDynamicInputUIManager* manager, bool &sendEnter, CString &command) override;

    void setHoverGrip(bool value) { m_bIsHoverGrip = value; }
    bool isHoverGrip() const { return m_bIsHoverGrip; }

private:
	DynInputUIDimensionsState(): m_bIsHoverGrip(false) { }

    bool m_bIsHoverGrip;
};
//DynInputUIDimensionsState
//======================================================================================
