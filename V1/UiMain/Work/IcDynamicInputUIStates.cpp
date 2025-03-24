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
#include <stdafx.h>
#include "Icad/Configure.h"
#include "IcDynamicInputUIManager.h"
#include "IcDynamicInputUIStates.h"
#include <IcadCommon/Interfaces/IIcadServicesManager.h>
#include <IcadCommon/Interfaces/IIcadIOManager.h>
#include <IcadCommon/utilities/IcadInterfaceUtils.h>
#include <IcadCommon/IcadCommonUtils.h>
#include <IcadCore/Ortho/IcOrthoUtils.h>
#include <IcadCore/DragJigs/IIcadDragger.h>
#include <IcadCore/DragJigs/IcEdBaseJig.h>
#include <IcadCore/GripEdit/IcGripPoint.h>
#include <IcadCore/Osnap/IcExternalOsnapUtils.h>


void realToString(CString originalString, double rValue, IcString &retString)
{
	IcString szNumber;
	IIcadServicesManager* pServMgr = getServicesManagerInterface(getIcadInterfaces());
	int ret = pServMgr->realToString(rValue, -1, -1, szNumber);
	// if LUNITS == 4 and converted number contains spaces then use the original string
	// eg : 15/2" = 0'-7 1/2" but it contains spaces so use original one
	int lunits;
	icedGetVar(L"LUNITS", lunits);
	if (szNumber.find(OdChar(L' ')) && ret == RTNORM && lunits == 4)
	{
		szNumber = (LPCTSTR)originalString;
	}
	retString = (LPCTSTR)szNumber;
}

//======================================================================================
//DynInputUINotActiveState
void DynInputUINotActiveState::changeActiveControl(IcDynamicInputUIManager* manager, bool considerZBox)
{
	manager->setActiveControl(manager->getBoxContainer()->getCommandStringBox());
}

void DynInputUINotActiveState::deactivate(IcDynamicInputUIManager* manager, bool bClearState)
{
	if (manager->getBoxContainer()->getXBox() && manager->getBoxContainer()->getXBox()->IsWindowVisible())
		manager->getBoxContainer()->getXBox()->Hide(bClearState);
	if (manager->getBoxContainer()->getYBox() && manager->getBoxContainer()->getYBox()->IsWindowVisible())
		manager->getBoxContainer()->getYBox()->Hide(bClearState);
	if (manager->getBoxContainer()->getCommandStringBox() && manager->getBoxContainer()->getCommandStringBox()->IsWindowVisible())
		manager->getBoxContainer()->getCommandStringBox()->Hide(bClearState);
}

bool DynInputUINotActiveState::onMouseMove(IcDynamicInputUIManager* manager)
{
	IIcadSysvarManager* pISysvar = getSysvarManagerInterface(getIcadInterfaces());
	ASSERT(pISysvar != NULL);
	int dynPiVis = 0;
	int retVal = pISysvar->getVar(L"DYNPIVIS", dynPiVis);

	if (dynPiVis != DynInputValues::KAlways)
	{
		if (manager->getBoxContainer()->getXBox() && manager->getBoxContainer()->getXBox()->IsWindowVisible())
			manager->getBoxContainer()->getXBox()->Hide();
		if (manager->getBoxContainer()->getYBox() && manager->getBoxContainer()->getYBox()->IsWindowVisible())
			manager->getBoxContainer()->getYBox()->Hide();
	}

	if (manager->getBoxContainer()->getZBox() && manager->getBoxContainer()->getZBox()->IsWindowVisible())
		manager->getBoxContainer()->getZBox()->Hide();
	if (manager->getBoxContainer()->getRealBox() && manager->getBoxContainer()->getRealBox()->IsWindowVisible())
		manager->getBoxContainer()->getRealBox()->Hide();
	if (manager->getBoxContainer()->getIntBox() && manager->getBoxContainer()->getIntBox()->IsWindowVisible())
		manager->getBoxContainer()->getIntBox()->Hide();
	if (manager->getBoxContainer()->getStringBox() && manager->getBoxContainer()->getStringBox()->IsWindowVisible())
		manager->getBoxContainer()->getStringBox()->Hide();
	if (manager->getBoxContainer()->getAngleBox() && manager->getBoxContainer()->getAngleBox()->IsWindowVisible())
		manager->getBoxContainer()->getAngleBox()->Hide();
	if (manager->getBoxContainer()->getPromptBox() && manager->getBoxContainer()->getPromptBox()->IsWindowVisible())
		manager->getBoxContainer()->getPromptBox()->Hide();
	if (manager->getBoxContainer()->getModeBox() && manager->getBoxContainer()->getModeBox()->IsWindowVisible())
		manager->getBoxContainer()->getModeBox()->Hide();

	if (dynPiVis == DynInputValues::KAlways)
	{
		CDynamicEdit *xBox = manager->getBoxContainer()->getXBox(true);
		CDynamicEdit *yBox = manager->getBoxContainer()->getYBox(true);

		OdGePoint3d currentAbsoluteUcsPosition = manager->getLastUcsPoint();

		if (xBox)
		{
			xBox->setValue(currentAbsoluteUcsPosition.x);
			xBox->EnableWindow(false);
			xBox->SetReadOnly(true);
			xBox->postPoneShow();
		}

		if (yBox)
		{
			yBox->setIsAngular(false);
			yBox->setValue(currentAbsoluteUcsPosition.y);
			yBox->postPoneShow();
		}
	}

	return false;
}

void DynInputUINotActiveState::activate(IcDynamicInputUIManager* manager)
{
	IIcadSysvarManager* pISysvar = getSysvarManagerInterface(getIcadInterfaces());
	ASSERT(pISysvar != NULL);
	int dynPiVis = 0;
	int retVal = pISysvar->getVar(L"DYNPIVIS", dynPiVis);
	int dynMode = 0;
	retVal = pISysvar->getVar(L"DYNMODE", dynMode);

	if (dynPiVis == DynInputValues::KAlways)
	{
		if (manager->getBoxContainer()->getXBox(false))
		{
			manager->getBoxContainer()->getXBox()->setIsRelative(false);
			manager->getBoxContainer()->getXBox()->EnableWindow(false);
			manager->getBoxContainer()->getXBox()->SetReadOnly(true);
		}

		if (manager->getBoxContainer()->getYBox(false))
		{
			manager->getBoxContainer()->getYBox()->setIsAngular(false);
			manager->getBoxContainer()->getYBox()->EnableWindow(false);
			manager->getBoxContainer()->getYBox()->SetReadOnly(true);
		}
	}

	/*CDynamicEdit *commandBox = manager->getCommandStringBox(true);
	if (commandBox)
	{
		commandBox->Hide();
		manager->setActiveControl(commandBox);
	}*/
	manager->setUpdateRequired();
}

bool DynInputUINotActiveState::getCommand(IcDynamicInputUIManager* manager, bool &sendEnter, CString &command)
{
	// progesoft
	// if command list is not yet visible calculate first command of list
	// eg "DATA" typed fast to start "DATAEXTRACTION"
	CDynamicCommandListBox* commandListBox = manager->getBoxContainer()->getCommandListBox();
	if (commandListBox && !commandListBox->IsWindowVisible())
	{
		commandListBox->KillTimer(CDynamicCommandListBox::kFillList);
		manager->displayCommandList(true, false, false);
	}

	CDynamicEdit *commandBox = manager->getBoxContainer()->getCommandStringBox(true);
	if (commandBox)
	{
		commandBox->GetWindowTextW(command);
		commandBox->Hide();
	}

	// progesoft [-
	// when user type a partial of command
	// and there is no command starting with such partial
	// command string box doesn't contain valid command
	// use selected command in command list
	// example type "MON" and press enter
	/*if (commandListBox && commandListBox->IsWindowVisible())
	{
		if (commandListBox->GetItemCount() > 0)
		{
			int selIndex = commandListBox->GetCurSel();
			if (selIndex > -1)
			{
				CString selectedCommand = commandListBox->GetItemText(selIndex, 0);
				// for aliases remove parentesys part
				if (selectedCommand.Find(L" ") > -1)
					selectedCommand = selectedCommand.Left(selectedCommand.Find(L" "));
				if (selectedCommand.CompareNoCase(command))
					command = selectedCommand;
			}
		}
	}*/
	// progesoft -]

	if (command.IsEmpty())
		sendEnter = true;
	manager->destroyCommandList();
	return true;
}

//DynInputUINotActiveState
//======================================================================================

//======================================================================================
//DynInputUIOnlyPromptState
void  DynInputUIOnlyPromptState::deactivate(IcDynamicInputUIManager* manager, bool bClearState)
{
	if (manager->getBoxContainer()->getPromptBox(false))
		manager->getBoxContainer()->getPromptBox(false)->Hide(bClearState);
}

bool  DynInputUIOnlyPromptState::onMouseMove(IcDynamicInputUIManager* manager)
{
	if (manager->getBoxContainer()->getXBox() && manager->getBoxContainer()->getXBox()->IsWindowVisible())
		manager->getBoxContainer()->getXBox()->Hide();
	if (manager->getBoxContainer()->getYBox() && manager->getBoxContainer()->getYBox()->IsWindowVisible())
		manager->getBoxContainer()->getYBox()->Hide();
	if (manager->getBoxContainer()->getZBox() && manager->getBoxContainer()->getZBox()->IsWindowVisible())
		manager->getBoxContainer()->getZBox()->Hide();
	if (manager->getBoxContainer()->getRealBox() && manager->getBoxContainer()->getRealBox()->IsWindowVisible())
		manager->getBoxContainer()->getRealBox()->Hide();
	if (manager->getBoxContainer()->getIntBox() && manager->getBoxContainer()->getIntBox()->IsWindowVisible())
		manager->getBoxContainer()->getIntBox()->Hide();
	if (manager->getBoxContainer()->getStringBox() && manager->getBoxContainer()->getStringBox()->IsWindowVisible())
		manager->getBoxContainer()->getStringBox()->Hide();
	if (manager->getBoxContainer()->getAngleBox() && manager->getBoxContainer()->getAngleBox()->IsWindowVisible())
		manager->getBoxContainer()->getAngleBox()->Hide();
	if (manager->getBoxContainer()->getModeBox() && manager->getBoxContainer()->getModeBox()->IsWindowVisible())
		manager->getBoxContainer()->getModeBox()->Hide();

	return false;
}
//DynInputUIOnlyPromptState
//======================================================================================

//======================================================================================
//DynInputUICoordinateState
void  DynInputUICoordinateState::changeActiveControl(IcDynamicInputUIManager* manager, bool considerZBox)
{
	CDynamicEdit* activeControls = manager->getActiveControl();
	if (!activeControls)
	{
		manager->setActiveControl(manager->getBoxContainer()->getXBox());
	}
	else if (activeControls == manager->getBoxContainer()->getXBox())
	{
		manager->setActiveControl(manager->getBoxContainer()->getYBox());
	}
	else if (activeControls == manager->getBoxContainer()->getYBox() &&
				(considerZBox || manager->getBoxContainer()->getZBox(true)->IsWindowVisible()))
	{
		if (CDynamicEdit* zBox = manager->getBoxContainer()->getZBox(true))
		{
			manager->getBoxContainer()->getYBox(true)->setIsAngular(false);
			manager->setActiveControl(zBox);
			zBox->postPoneShow();
			manager->resizeControls();
		}
	}
	else if (activeControls == manager->getBoxContainer()->getYBox() && !considerZBox)
	{
		manager->setActiveControl(manager->getBoxContainer()->getXBox());
	}
	else if (activeControls == manager->getBoxContainer()->getZBox())
	{
		manager->setActiveControl(manager->getBoxContainer()->getXBox());
	}
	else
	{
		manager->setActiveControl(manager->getBoxContainer()->getXBox());
	}

	if (activeControls)
		activeControls->setCanSwitchToCoordinate(false); // cannot switch from coordinate to coordinate
}

void  DynInputUICoordinateState::resetValues(IcDynamicInputUIManager* manager)
{
	if (manager->getBoxContainer()->getXBox(false))
	{
		manager->getBoxContainer()->getXBox(false)->setStatus(CDynamicEdit::eStateUnchanged);
		//getXBox(false)->SetWindowText(L"");
	}
	if (manager->getBoxContainer()->getYBox(false))
	{
		manager->getBoxContainer()->getYBox(false)->setStatus(CDynamicEdit::eStateUnchanged);
		//getYBox(false)->SetWindowTextW(L"");
	}
	if (manager->getBoxContainer()->getZBox(false))
	{
		manager->getBoxContainer()->getZBox(false)->setStatus(CDynamicEdit::eStateUnchanged);
		//getZBox(false)->SetWindowTextW(L"");
	}
}

void  DynInputUICoordinateState::deactivate(IcDynamicInputUIManager* manager, bool bClearState)
{
	if (manager->getBoxContainer()->getPromptBox(false))
		manager->getBoxContainer()->getPromptBox(false)->Hide(bClearState);
	if (manager->getBoxContainer()->getModeBox(false))
		manager->getBoxContainer()->getModeBox(false)->Hide(bClearState);
	if (manager->getBoxContainer()->getXBox(false))
		manager->getBoxContainer()->getXBox(false)->Hide(bClearState);
	if (manager->getBoxContainer()->getYBox(false))
		manager->getBoxContainer()->getYBox(false)->Hide(bClearState);
	if (manager->getBoxContainer()->getZBox(false))
		manager->getBoxContainer()->getZBox(false)->Hide(bClearState);
}

bool  DynInputUICoordinateState::onMouseMove(IcDynamicInputUIManager* manager)
{
	bool ret = false;
	IIcadSysvarManager* pISysvar = getSysvarManagerInterface(getIcadInterfaces());
	ASSERT(pISysvar != NULL);

	// draw X Y boxes
	CDynamicEdit *xBox = manager->getBoxContainer()->getXBox();
	CDynamicEdit *yBox = manager->getBoxContainer()->getYBox();

	int dynPiCoords = 0;
	int retVal = pISysvar->getVar(L"DYNPICOORDS", dynPiCoords);

	int lastPointInit = 0;
	retVal = pISysvar->getVar(L"LASTPOINTINIT", lastPointInit);

	int dynPiVis = 0;
	retVal = pISysvar->getVar(L"DYNPIVIS", dynPiVis);

	OdGePoint3d lastPoint;
	if (xBox && xBox->isRelative())
		retVal = pISysvar->getVar(L"LASTPOINT", lastPoint);

	OdGePoint3d currentAbsoluteUcsPosition = manager->getLastUcsPoint();

	OdGeVector3d directionUcs = currentAbsoluteUcsPosition.asVector() - lastPoint.asVector();
	directionUcs.z = 0.0;
	double distance = directionUcs.length();
	double angle = 0.0;
	if (!directionUcs.convert2d().isZeroLength())
		angle = Oda2PI - directionUcs.convert2d().angleToCCW(OdGeVector2d::kXAxis);

	double dValue = 0.0;

	if (xBox)
	{
		if (xBox->IsUnchanged())
		{
			if (yBox->isAngular())
			{
				if(yBox->IsUnchanged())
				{
					xBox->setValue(distance);
				}
				else
				{
					double val;
					yBox->getValue(val);
					xBox->setValue(std::abs(cos(val - angle) * distance));
				}
			}
			else
				xBox->setValue(directionUcs.x);
		}
		else
		{
			// constaints x value to specified value
			if (xBox->getStatus() != CDynamicEdit::eStateError && !xBox->isEsnap() && xBox->getValue(dValue))
			{
				if (yBox->isAngular())
				{
					if (!yBox->IsUnchanged())
						yBox->getValue(angle);
					xBox->getValue(distance);
					manager->setLastUcsPoint(OdGePoint3d(lastPoint.x + cos(angle) * distance, lastPoint.y + sin(angle) * distance, lastPoint.z));
				}
				else
				{
					if (xBox->isRelative())
						currentAbsoluteUcsPosition.x = lastPoint.x + dValue;
					else
						currentAbsoluteUcsPosition.x = dValue;
					manager->setLastUcsPoint(currentAbsoluteUcsPosition);
				}
				ret = true;
			}
		}

		xBox->setAcceptNull(true);
	}

	if (yBox)
	{
		if (yBox->IsUnchanged())
		{
			if (yBox->isAngular())
				yBox->setValue(angle);
			else
				yBox->setValue(directionUcs.y);
		}
		else
		{
			// constaints y value to specified value
			if (yBox->getStatus() != CDynamicEdit::eStateError && yBox->getValue(dValue))
			{
				if (yBox->isAngular())
				{
					//yBox->getValue(angle);
					if (!xBox->IsUnchanged())
						xBox->getValue(distance);

					double val;
					yBox->getValue(val);
					double d = cos(val - angle) * distance;
					manager->setLastUcsPoint(OdGePoint3d(lastPoint.x + cos(val) * d, lastPoint.y + sin(val) * d, lastPoint.z));
				}
				else
				{
					if (xBox->isRelative())
						currentAbsoluteUcsPosition.y = lastPoint.y + dValue;
					else
						currentAbsoluteUcsPosition.y = dValue;
					manager->setLastUcsPoint(currentAbsoluteUcsPosition);
				}
				ret = true;
			}
		}
	}

	CDynamicEdit *zBox = manager->getBoxContainer()->getZBox(false);
	if (zBox)
	{
		if (zBox->IsUnchanged())
		{
			zBox->setValue(directionUcs.z);
		}
		else
		{
			// constaints z value to specified value
			if (zBox->getStatus() != CDynamicEdit::eStateError && zBox->getValue(dValue))
			{
				if (zBox->isAngular())
				{
					zBox->getValue(angle);
					// todo
					/*if (!xBox->IsUnchanged())
					xBox->getValue(distance);
					setLastUcsPoint( OdGePoint3d( lastPoint.x + cos(angle) * distance , lastPoint.y + sin(angle) * distance, lastPoint.z));*/
				}
				else
				{
					if (xBox->isRelative())
						currentAbsoluteUcsPosition.z = lastPoint.z + dValue;
					else
						currentAbsoluteUcsPosition.z = dValue;
					manager->setLastUcsPoint(currentAbsoluteUcsPosition);
				}
			}
		}
	}

	return ret;
}

void  DynInputUICoordinateState::activate(IcDynamicInputUIManager* manager)
{
	IIcadSysvarManager* pISysvar = getSysvarManagerInterface(getIcadInterfaces());
	ASSERT(pISysvar != NULL);

	CDynamicEdit *xBox = manager->getBoxContainer()->getXBox(true);
	CDynamicEdit *yBox = manager->getBoxContainer()->getYBox(true);

	if(xBox && yBox)
	{
		int dynPiVis = 0;
		int retVal = pISysvar->getVar(L"DYNPIVIS", dynPiVis);
		ASSERT(retVal == RTNORM);
		int lastPointInit = 0;
		retVal = pISysvar->getVar(L"LASTPOINTINIT", lastPointInit);
		ASSERT(retVal == RTNORM);
		int dynPICoords;
		retVal = pISysvar->getVar(L"DYNPICOORDS", dynPICoords); // default coords system for second point , 0 = relative, 1= absolute
		ASSERT(retVal == RTNORM);
		if (dynPiVis != DynInputValues::KOnlyWhenYouTypeAtPromptForPoint)
		{
			xBox->postPoneShow();
			yBox->postPoneShow();
			if (lastPointInit)
			{
				int dynPiDispModeBox = 0;
				retVal = pISysvar->getVar(L"DYNPIDISPMODEBOX", dynPiDispModeBox);
				bool showMode = ((dynPiDispModeBox == 1) // force display
					|| (manager->getBoxContainer()->getXBox(false)->isRelative() != (dynPICoords == DynInputValues::KRelative))); // actual isRelative mode is different from dynPiCoords variable ;
				if (showMode)
				{
					CDynamicStringPromptEdit* modeBox = manager->getBoxContainer()->getModeBox(true);
					if (modeBox)
						modeBox->postPoneShow();
				}
			}
		}
		else
		{
			xBox->Hide();
			yBox->Hide();
			CDynamicStringPromptEdit* modeBox = manager->getBoxContainer()->getModeBox(true);
			if (modeBox)
				modeBox->Hide();
		}

		xBox->setIsRelative(lastPointInit == 1 && dynPICoords == DynInputValues::KRelative);
		manager->getBoxContainer()->getModeBox()->setValue(xBox->isRelative() ? L"@" : L"#");

		// initialize isRelative and isAngular to variable value when LASTPOINTINIT is 1
		// restore default value
		int dynPIFormat = 0;
		retVal = pISysvar->getVar(L"DYNPIFORMAT", dynPIFormat);
		ASSERT(retVal == RTNORM);
		// dont create it now due to thread problem, first time this initialization is made by creation of control
		yBox->setIsAngular(lastPointInit == 1 && dynPIFormat == DynInputValues::KPolar);

		int dynZBox = 0;
		retVal = pISysvar->getVar(L"DYNZBOX", dynZBox);
		ASSERT(retVal == RTNORM);
		if((lastPointInit == 0 || dynPIFormat == DynInputValues::KCartesian) &&
			dynZBox == DynInputValues::KShowZFieldAlways &&
			dynPiVis != DynInputValues::KOnlyWhenYouTypeAtPromptForPoint)
		{
			if(CDynamicEdit* zBox = manager->getBoxContainer()->getZBox(true))
			{
				zBox->postPoneShow();
			}
		}
		manager->setUpdateRequired();
	}

}

bool  DynInputUICoordinateState::getCommand(IcDynamicInputUIManager* manager, bool &sendEnter, CString &command)
{
	CDynamicEdit *xBox = manager->getBoxContainer()->getXBox();
	CDynamicEdit *yBox = manager->getBoxContainer()->getYBox();
	CDynamicEdit *zBox = manager->getBoxContainer()->getZBox();
	if (xBox->IsUnchanged() && yBox->IsUnchanged() &&
		(!zBox || zBox->IsUnchanged()))
	{
		// edit boxes are not modified
		// return or space go to command line directly
		return false;
	}
	else if (xBox->isEsnap() || (!xBox->IsUnchanged() && yBox->IsUnchanged()))
	{
		xBox->GetWindowTextW(command);
		return true;
	}
	else if (!yBox->IsUnchanged())
	{
		CString xVal, yVal, separator = L",";
		IIcadSysvarManager* pISysvar = getSysvarManagerInterface(getIcadInterfaces());

		bool isAngular = yBox->isAngular();
		bool isRelative = xBox->isRelative();

		int lastPointInit;
		int retVal = pISysvar->getVar(L"LASTPOINTINIT", lastPointInit);

		double dVal;
		IcString szNumber;

		xBox->GetWindowTextW(xVal);
		xVal.Trim(L"be");

		yBox->GetWindowTextW(yVal);

		if (isAngular)
			separator = L"<";

		if (isRelative)
		{
			if(isAngular && yBox->IsModified())
			{
				OdGePoint3d lastPoint;
				retVal = pISysvar->getVar(L"LASTPOINT", lastPoint);
				OdGePoint3d currentAbsoluteUcsPosition = manager->getLastUcsPoint();

				OdGeVector3d directionUcs = currentAbsoluteUcsPosition.asVector() - lastPoint.asVector();
				double angle = 0.0;
				if (!directionUcs.convert2d().isZeroLength())
					angle = Oda2PI - directionUcs.convert2d().angleToCCW(OdGeVector2d::kXAxis);

				double val;
				yBox->getValue(val);
				double distance;
				xBox->getValue(distance);
				distance *= cos(val - angle);
				xVal.Format(L"%lf", distance);
			}

			command = L'@' + xVal + separator + yVal;
		}
		else
		{
			command = xVal + separator + yVal; // icad doesn't accept # but it works always in absolute mode by default
		}

		if (zBox && zBox->IsWindowVisible())
		{
			IIcadServicesManager* pServMgr = getServicesManagerInterface(getIcadInterfaces());
			CString zVal;
			zBox->getValue(dVal);
			if (separator == L"<")
			{
				pServMgr->angleToString(dVal, -1, -1, szNumber);
				zVal = (LPCTSTR)szNumber;
			}
			else
			{
				zBox->GetWindowTextW(zVal);
			}

			command += separator + zVal;
		}
		return true;
	}

	return true;
}
//DynInputUICoordinateState
//======================================================================================

//======================================================================================
//DynInputUIGetRealState
void  DynInputUIGetRealState::changeActiveControl(IcDynamicInputUIManager* manager, bool considerZBox)
{
	CDynamicEdit* activeControls = manager->getActiveControl();
	manager->setActiveControl(manager->getBoxContainer()->getRealBox());
	if (activeControls)
		activeControls->setCanSwitchToCoordinate(false);
}

void  DynInputUIGetRealState::resetValues(IcDynamicInputUIManager* manager)
{
	if (manager->getBoxContainer()->getRealBox())
	{
		manager->getBoxContainer()->getRealBox()->setStatus(CDynamicEdit::eStateUnchanged);
		manager->getBoxContainer()->getRealBox()->SetWindowTextW(L"");
	}
}

void  DynInputUIGetRealState::deactivate(IcDynamicInputUIManager* manager, bool bClearState)
{
	if (manager->getBoxContainer()->getRealBox(false))
		manager->getBoxContainer()->getRealBox(false)->Hide(bClearState);
}

bool  DynInputUIGetRealState::onMouseMove(IcDynamicInputUIManager* manager)
{
	int dynShowDefault = 0;
	IIcadSysvarManager* pISysvar = getSysvarManagerInterface(getIcadInterfaces());
	ASSERT(pISysvar != NULL);
	int retVal = pISysvar->getVar(L"DYNSHOWDEFAULT", dynShowDefault);
	// this doesn't set any value by mouse position
	CDynamicEdit *realBox = manager->getBoxContainer()->getRealBox(true);
	realBox->setAcceptNull(false);

	if (dynShowDefault && realBox->IsUnchanged())
		realBox->SetWindowTextW((LPCTSTR)manager->getDefaultValue());

	realBox->postPoneShow();

	return false;
}

bool  DynInputUIGetRealState::getCommand(IcDynamicInputUIManager* manager, bool &sendEnter, CString &command)
{
	if (manager->getBoxContainer()->getRealBox()->IsUnchanged())
		return false;

	manager->getBoxContainer()->getRealBox()->GetWindowTextW(command);
	if (command.IsEmpty())
		sendEnter = true;
	else
	{
		double dVal;
		IcString szNumber;
		if (manager->getBoxContainer()->getRealBox()->getValue(dVal))
		{
			realToString(command, dVal, szNumber);
		}
		else
		{
			// it's not a number, send it to command line as is
		}
	}

	return true;
}
//DynInputUIGetRealState
//======================================================================================

//======================================================================================
//DynInputUIGetIntState
void  DynInputUIGetIntState::changeActiveControl(IcDynamicInputUIManager* manager, bool considerZBox)
{
	CDynamicEdit* activeControls = manager->getActiveControl();
	manager->setActiveControl(manager->getBoxContainer()->getIntBox());
	if (activeControls)
		activeControls->setCanSwitchToCoordinate(false);
}

void  DynInputUIGetIntState::resetValues(IcDynamicInputUIManager* manager)
{
	if (manager->getBoxContainer()->getIntBox())
	{
		manager->getBoxContainer()->getIntBox()->setStatus(CDynamicEdit::eStateUnchanged);
		manager->getBoxContainer()->getIntBox()->SetWindowTextW(L"");
	}
}

void  DynInputUIGetIntState::deactivate(IcDynamicInputUIManager* manager, bool bClearState)
{
	if (manager->getBoxContainer()->getIntBox(false))
		manager->getBoxContainer()->getIntBox(false)->Hide(bClearState);
	if (manager->getBoxContainer()->getPromptBox(false))
		manager->getBoxContainer()->getPromptBox(false)->Hide(bClearState);
}

bool  DynInputUIGetIntState::onMouseMove(IcDynamicInputUIManager* manager)
{
	int dynShowDefault = 0;
	IIcadSysvarManager* pISysvar = getSysvarManagerInterface(getIcadInterfaces());
	ASSERT(pISysvar != NULL);
	int retVal = pISysvar->getVar(L"DYNSHOWDEFAULT", dynShowDefault);
	// this doesn't set any value by mouse position
	CDynamicEdit *realBox = manager->getBoxContainer()->getIntBox(true);
	realBox->setAcceptNull(false);

	if (dynShowDefault && realBox->IsUnchanged())
		realBox->SetWindowTextW((LPCTSTR)manager->getDefaultValue());

	realBox->postPoneShow();

	return false;
}

bool DynInputUIGetIntState::getCommand(IcDynamicInputUIManager* manager, bool &sendEnter, CString &command)
{
	manager->getBoxContainer()->getIntBox()->GetWindowTextW(command);
	if (command.IsEmpty())
		sendEnter = true;
	else
	{
		int iVal;
		IcString szNumber;
		// this control can accept also text (keyword)
		if (((CDynamicIntEdit*)manager->getBoxContainer()->getIntBox())->getValue(iVal))
		{
			szNumber.format(L"%d", iVal);
			command = (LPCTSTR)szNumber;
		}
	}

	return true;
}
//DynInputUIGetIntState
//======================================================================================

//======================================================================================
//DynInputUIGetDistState
void DynInputUIGetDistState::changeActiveControl(IcDynamicInputUIManager* manager, bool considerZBox)
{
	CDynamicEdit* activeControls = manager->getActiveControl();
	manager->setActiveControl(manager->getBoxContainer()->getRealBox());
	if (activeControls)
		activeControls->setCanSwitchToCoordinate(true);
}

void DynInputUIGetDistState::resetValues(IcDynamicInputUIManager* manager)
{
	if (manager->getBoxContainer()->getRealBox())
	{
		manager->getBoxContainer()->getRealBox()->setStatus(CDynamicEdit::eStateUnchanged);
		manager->getBoxContainer()->getRealBox()->SetWindowTextW(L"");
	}
}

void DynInputUIGetDistState::deactivate(IcDynamicInputUIManager* manager, bool bClearState)
{
	if (manager->getBoxContainer()->getRealBox(false))
		manager->getBoxContainer()->getRealBox(false)->Hide(bClearState);
	if (manager->getBoxContainer()->getPromptBox(false))
		manager->getBoxContainer()->getPromptBox(false)->Hide(bClearState);
}

bool DynInputUIGetDistState::onMouseMove(IcDynamicInputUIManager* manager)
{
	int dynShowDefault = 0;
	IIcadSysvarManager* pISysvar = getSysvarManagerInterface(getIcadInterfaces());
	ASSERT(pISysvar != NULL);
	int retVal = pISysvar->getVar(k_IC_RDYNSHOWDEFAULT, dynShowDefault);
	CDynamicEdit *realBox = manager->getBoxContainer()->getRealBox(true);

	if (realBox->IsUnchanged())
    {
	    if (dynShowDefault && !manager->getDefaultValue().isEmpty())
        {
            realBox->setValue((LPCTSTR)manager->getDefaultValue());
        }
        else
        {
			int lastPointInit = 0;
			retVal = pISysvar->getVar(k_IC_LASTPOINTINIT, lastPointInit);
			if(lastPointInit != 0)
			{
				OdGePoint3d ucsLastPoint;
				retVal = pISysvar->getVar(k_IC_LASTPOINT, ucsLastPoint);
				realBox->setValue(ucsLastPoint.distanceTo(manager->getLastUcsPoint()));
			}
			else
			{
				realBox->setValue("");
			}
        }
    }

	realBox->setAcceptNull(false);
	realBox->postPoneShow();

	return false;
}

bool DynInputUIGetDistState::getCommand(IcDynamicInputUIManager* manager, bool &sendEnter, CString &command)
{
	if (manager->getBoxContainer()->getRealBox()->IsUnchanged())
		return false;

	manager->getBoxContainer()->getRealBox()->GetWindowTextW(command);
	if (command.IsEmpty())
		sendEnter = true;
	else
	{
		double dVal;
		IcString szNumber;
		if (manager->getBoxContainer()->getRealBox()->getValue(dVal))
		{
			realToString(command, dVal, szNumber);
		}
		else
		{
			// it's not a number, send it to command line as is
		}
	}

	return true;
}
//DynInputUIGetDistState
//======================================================================================

//======================================================================================
//DynInputUIGetAngleState
void DynInputUIGetAngleState::changeActiveControl(IcDynamicInputUIManager* manager, bool considerZBox)
{
	CDynamicEdit* activeControls = manager->getActiveControl();
	manager->setActiveControl(manager->getBoxContainer()->getAngleBox());
	if (activeControls)
		activeControls->setCanSwitchToCoordinate(true);
}

void DynInputUIGetAngleState::resetValues(IcDynamicInputUIManager* manager)
{
	if (manager->getBoxContainer()->getAngleBox())
	{
		manager->getBoxContainer()->getAngleBox()->setStatus(CDynamicEdit::eStateUnchanged);
		manager->getBoxContainer()->getAngleBox()->SetWindowTextW(L"");
	}
}

void DynInputUIGetAngleState::deactivate(IcDynamicInputUIManager* manager, bool bClearState)
{
	if (manager->getBoxContainer()->getAngleBox(false))
		manager->getBoxContainer()->getAngleBox(false)->Hide(bClearState);
	if (manager->getBoxContainer()->getPromptBox(false))
		manager->getBoxContainer()->getPromptBox(false)->Hide(bClearState);
}

bool DynInputUIGetAngleState::onMouseMove(IcDynamicInputUIManager* manager)
{
	CDynamicEdit *angleBox = manager->getBoxContainer()->getAngleBox(true);
	if (angleBox)
	{
		IIcadSysvarManager* pISysvar = getSysvarManagerInterface(getIcadInterfaces());
		ASSERT(pISysvar != NULL);

		if (angleBox->IsUnchanged())
		{
			int dynShowDefault = 0;
			int retVal = pISysvar->getVar(k_IC_RDYNSHOWDEFAULT, dynShowDefault);
			if (dynShowDefault && !manager->getDefaultValue().isEmpty())
			{
				angleBox->SetWindowTextW((LPCTSTR)manager->getDefaultValue());
			}
			else
			{
				IIcadIOManager* pIO = getIOManagerInterface(getIcadInterfaces());
				PointContainer basePoint;
				pIO->getCoordsDisplayMode(basePoint);
				OdGeVector2d direction2d;
				if(!basePoint.isEmpty())
				{
					OdGeVector3d direction3d = manager->getLastUcsPoint() - basePoint.get();
					direction2d = direction3d.convert2d();
				}
				else
				{
					int lastPointInit = 0;
					retVal = pISysvar->getVar(k_IC_LASTPOINTINIT, lastPointInit);
					if (lastPointInit != 0)
					{
						OdGePoint3d ucsLastPoint;
						retVal = pISysvar->getVar(k_IC_LASTPOINT, ucsLastPoint);

						OdGeVector3d direction3d = manager->getLastUcsPoint() - ucsLastPoint;
						direction2d = direction3d.convert2d();
					}
				}

				if (!direction2d.isZeroLength())
				{
					angleBox->setValue(Oda2PI - direction2d.angleToCCW(OdGeVector2d::kXAxis));
				}
				else
				{
					angleBox->setValue("");
				}
			}
		}

		angleBox->setAcceptNull(false);
		angleBox->postPoneShow();
	}

	return false;
}

bool DynInputUIGetAngleState::getCommand(IcDynamicInputUIManager* manager, bool &sendEnter, CString &command)
{
	if (manager->getBoxContainer()->getAngleBox()->IsUnchanged())
		return false;

	manager->getBoxContainer()->getAngleBox()->GetWindowTextW(command);
	if (command.IsEmpty())
		sendEnter = true;

	return true;
}
//DynInputUIGetAngleState
//======================================================================================

//======================================================================================
//DynInputUIGetStringState
void DynInputUIGetStringState::changeActiveControl(IcDynamicInputUIManager* manager, bool considerZBox)
{
	manager->setActiveControl(manager->getBoxContainer()->getStringBox());
}

void DynInputUIGetStringState::resetValues(IcDynamicInputUIManager* manager)
{
	if (manager->getBoxContainer()->getStringBox())
	{
		manager->getBoxContainer()->getStringBox()->setStatus(CDynamicEdit::eStateUnchanged);
		manager->getBoxContainer()->getStringBox()->SetWindowTextW(L"");
	}
}

void DynInputUIGetStringState::deactivate(IcDynamicInputUIManager* manager, bool bClearState)
{
	if (manager->getBoxContainer()->getStringBox(false))
		manager->getBoxContainer()->getStringBox(false)->Hide(bClearState);
	if (manager->getBoxContainer()->getPromptBox(false))
		manager->getBoxContainer()->getPromptBox(false)->Hide(bClearState);
}

bool DynInputUIGetStringState::onMouseMove(IcDynamicInputUIManager* manager)
{
	IIcadIOManager* pIO = getIOManagerInterface(getIcadInterfaces());
	int userInputControls = pIO->getUserInputControls();
	bool hasKeyWords = !pIO->getKeywords().isEmpty();
	IIcadSysvarManager* pISysvar = getSysvarManagerInterface(getIcadInterfaces());
	ASSERT(pISysvar != NULL);
	int dynShowDefault = 0;
	int retVal = pISysvar->getVar(L"DYNSHOWDEFAULT", dynShowDefault);
	// this doesn't set any value by mouse position
	CDynamicEdit *stringBox = manager->getBoxContainer()->getStringBox(true);
	if (stringBox)
	{
		stringBox->setAcceptNull(!(userInputControls & IcUserInputControls::kNullResponseIsNotAccepted));
	}

	// show only list when :
	//	- no space are accepted
	//  - no other string
	bool onlyList =	(!(userInputControls & IcUserInputControls::kAcceptOtherInputString) ||
		manager->isOnlyKeywords()) &&
		manager->getOptionList()->size() > 1;

	if (onlyList)
	{
		if (manager->displayOptionList(userInputControls))
		{
			manager->getBoxContainer()->getOptionsListBox(false)->SelectString(0, (LPCTSTR)manager->getDefaultValue());
		}
		if (stringBox)
		{
			stringBox->SetWindowTextW(L"");
			stringBox->postPoneShow();
		}
	}
	else
	{
		if (stringBox)
		{
			if (dynShowDefault && stringBox->IsUnchanged())
			{
				stringBox->SetWindowTextW((LPCTSTR)manager->getDefaultValue());
			}
			stringBox->postPoneShow();
		}
	}

	return false;
}

void DynInputUIGetStringState::activate(IcDynamicInputUIManager* manager)
{
	CDynamicEdit *stringBox = manager->getBoxContainer()->getStringBox(true);
	if (stringBox)
	{
		stringBox->setEnterMask(CDynamicEdit::eEnterOnEnter | CDynamicEdit::eEnterOnSpace);
	}
}

bool DynInputUIGetStringState::getCommand(IcDynamicInputUIManager* manager, bool &sendEnter, CString &command)
{
	manager->getBoxContainer()->getStringBox()->GetWindowTextW(command);
	// if user press ENTER to start default option
	// I need to set string to space because parent function stops everything
	// if return string is empty
	if (command.IsEmpty())
		sendEnter = true;

	return true;
}
//DynInputUIGetStringState
//======================================================================================

//======================================================================================
//DynInputUIGetStringWithSpaceState
void DynInputUIGetStringWithSpaceState::changeActiveControl(IcDynamicInputUIManager* manager, bool considerZBox)
{
	manager->setActiveControl(manager->getBoxContainer()->getStringBox());
}

void DynInputUIGetStringWithSpaceState::resetValues(IcDynamicInputUIManager* manager)
{
	if (manager->getBoxContainer()->getStringBox())
	{
		manager->getBoxContainer()->getStringBox()->setStatus(CDynamicEdit::eStateUnchanged);
		manager->getBoxContainer()->getStringBox()->SetWindowTextW(L"");
	}
}

void DynInputUIGetStringWithSpaceState::deactivate(IcDynamicInputUIManager* manager, bool bClearState)
{
	if (manager->getBoxContainer()->getStringBox(false))
		manager->getBoxContainer()->getStringBox(false)->Hide(bClearState);
	if (manager->getBoxContainer()->getPromptBox(false))
		manager->getBoxContainer()->getPromptBox(false)->Hide(bClearState);
}

bool DynInputUIGetStringWithSpaceState::onMouseMove(IcDynamicInputUIManager* manager)
{
	IIcadIOManager* pIO = getIOManagerInterface(getIcadInterfaces());
	int userInputControls = pIO->getUserInputControls();
	bool hasKeyWords = !pIO->getKeywords().isEmpty();
	IIcadSysvarManager* pISysvar = getSysvarManagerInterface(getIcadInterfaces());
	ASSERT(pISysvar != NULL);
	int dynShowDefault = 0;
	int retVal = pISysvar->getVar(L"DYNSHOWDEFAULT", dynShowDefault);
	// this doesn't set any value by mouse position
	CDynamicEdit *stringBox = manager->getBoxContainer()->getStringBox(true);
	if (stringBox)
	{
		stringBox->setAcceptNull(!(userInputControls & IcUserInputControls::kNullResponseIsNotAccepted));
	}

	// show only list when :
	//	- no space are accepted
	//  - no other string
	bool onlyList = !(userInputControls & IcUserInputControls::kAcceptOtherInputString) &&
		manager->getOptionList()->size() > 1;

	if (stringBox)
	{
		if (dynShowDefault && stringBox->IsUnchanged())
		{
			stringBox->SetWindowTextW((LPCTSTR)manager->getDefaultValue());
		}
		stringBox->postPoneShow();
	}

	return false;
}

void DynInputUIGetStringWithSpaceState::activate(IcDynamicInputUIManager* manager)
{
	CDynamicEdit *stringBox = manager->getBoxContainer()->getStringBox(true);
	if (stringBox)
	{
		stringBox->setEnterMask(CDynamicEdit::eEnterOnEnter);
	}
}

bool DynInputUIGetStringWithSpaceState::getCommand(IcDynamicInputUIManager* manager, bool &sendEnter, CString &command)
{
	manager->getBoxContainer()->getStringBox()->GetWindowTextW(command);
	// if user press ENTER to start default option
	// I need to set string to space because parent function stops everything
	// if return string is empty
	if (command.IsEmpty())
		sendEnter = true;

	return true;
}
//DynInputUIGetStringWithSpaceState
//======================================================================================

//======================================================================================
//DynInputUIGetOrientState
void DynInputUIGetOrientState::changeActiveControl(IcDynamicInputUIManager* manager, bool considerZBox)
{
	CDynamicEdit* activeControls = manager->getActiveControl();
	manager->setActiveControl(manager->getBoxContainer()->getAngleBox());
	if (activeControls)
		activeControls->setCanSwitchToCoordinate(true);
}

void DynInputUIGetOrientState::resetValues(IcDynamicInputUIManager* manager)
{
	if (manager->getBoxContainer()->getAngleBox())
	{
		manager->getBoxContainer()->getAngleBox()->setStatus(CDynamicEdit::eStateUnchanged);
		manager->getBoxContainer()->getAngleBox()->SetWindowTextW(L"");
	}
}

void DynInputUIGetOrientState::deactivate(IcDynamicInputUIManager* manager, bool bClearState)
{
	if (manager->getBoxContainer()->getAngleBox(false))
		manager->getBoxContainer()->getAngleBox(false)->Hide(bClearState);
	if (manager->getBoxContainer()->getPromptBox(false))
		manager->getBoxContainer()->getPromptBox(false)->Hide(bClearState);
}

bool DynInputUIGetOrientState::onMouseMove(IcDynamicInputUIManager* manager)
{
	IIcadIOManager* pIO = getIOManagerInterface(getIcadInterfaces());
	int userInputControls = pIO->getUserInputControls();
	bool hasKeyWords = !pIO->getKeywords().isEmpty();
	IIcadSysvarManager* pISysvar = getSysvarManagerInterface(getIcadInterfaces());
	ASSERT(pISysvar != NULL);
	int dynShowDefault = 0;
	int retVal = pISysvar->getVar(L"DYNSHOWDEFAULT", dynShowDefault);
	// this doesn't set any value by mouse position
	CDynamicEdit *angleBox = manager->getBoxContainer()->getAngleBox(true);
	if (angleBox)
	{
		// if has a keyword it can accepts also alpha
		if (userInputControls & IcUserInputControls::kAcceptOtherInputString || hasKeyWords)
			angleBox->setAcceptMask(angleBox->getAcceptMask() | CDynamicEdit::eAcceptAlpha);
		else
			angleBox->setAcceptMask(CDynamicEdit::eAcceptNumbers | CDynamicEdit::eAcceptDot);

		if (dynShowDefault && angleBox->IsUnchanged())
			angleBox->SetWindowTextW((LPCTSTR)manager->getDefaultValue());

		angleBox->setAcceptNull(!(userInputControls & IcUserInputControls::kNullResponseIsNotAccepted));
		angleBox->postPoneShow();
	}

	return false;
}

bool DynInputUIGetOrientState::getCommand(IcDynamicInputUIManager* manager, bool &sendEnter, CString &command)
{
	if (manager->getBoxContainer()->getAngleBox()->IsUnchanged())
		return false;

	manager->getBoxContainer()->getAngleBox()->GetWindowTextW(command);
	if (command.IsEmpty())
		sendEnter = true;

	return true;
}
//DynInputUIGetOrientState
//======================================================================================

//======================================================================================
//DynInputUIDimensionsState
void DynInputUIDimensionsState::changeActiveControl(IcDynamicInputUIManager* manager, bool considerZBox)
{
	CDynamicEdit* activeControls = manager->getActiveControl();
	OdArray<CDynamicEdit*>* dataControls = manager->getDataControls();

	if (dataControls->size() > 0 && activeControls)
	{
		unsigned int currentIndex = 0;
		unsigned int nextActiveIndex = 0;
		if (dataControls->find(activeControls, currentIndex))
		{
			if (currentIndex == dataControls->size() - 1 ||
				!dataControls->getAt(currentIndex) ||
				!dataControls->getAt(currentIndex)->IsWindowVisible())
				nextActiveIndex = 0;// return to first control
			else
				nextActiveIndex = currentIndex + 1;
		}

		// search for an editable control
		while (!dataControls->getAt(nextActiveIndex)->isEditable() ||
			!dataControls->getAt(nextActiveIndex)->dimData() ||
			dataControls->getAt(nextActiveIndex)->dimData()->isDimInvisible() ||
			!dataControls->getAt(nextActiveIndex)->IsWindowVisible())
		{
			nextActiveIndex++;
			if (nextActiveIndex == dataControls->size())
				nextActiveIndex = 0;
			if (nextActiveIndex == currentIndex)
				break;
		}

		if (currentIndex == nextActiveIndex)
		{
			activeControls->setStatus(CDynamicEdit::eStateUnchanged);
			manager->updateDimensionValue();
		}
		else
		{
			manager->setActiveControl(dataControls->getAt(nextActiveIndex));
		}
		//if (m_ActiveControl->getAcceptMask() & CDynamicEdit::eAcceptAlpha)
		//	m_ActiveControl->setCanSwitchToCoordinate(false); // controls who accepts string cannot switch to coordinate
		//else
		manager->getActiveControl()->setCanSwitchToCoordinate(nextActiveIndex == 0); // only on first control you can switch
	}
}

void DynInputUIDimensionsState::resetValues(IcDynamicInputUIManager* manager)
{
	CDynamicEdit* activeControls = manager->getActiveControl();
	OdArray<CDynamicEdit*>* dataControls = manager->getDataControls();
	for (int i = 0; i != dataControls->size(); i++)
	{
		if (dataControls->at(i))
		{
			dataControls->at(i)->setStatus(CDynamicEdit::eStateUnchanged);
			if (dataControls->at(i)->dimData() && dataControls->at(i)->dimData()->isCustomString())
				dataControls->at(i)->SetWindowTextW(L"");
		}
	}
}

void DynInputUIDimensionsState::deactivate(IcDynamicInputUIManager* manager, bool bClearState)
{
	/*manager->setDimData(nullptr);*/
	if (manager->getBoxContainer()->getPromptBox(false))
		manager->getBoxContainer()->getPromptBox(false)->Hide(bClearState);

	OdArray<CDynamicEdit*>* pDataControls = manager->getDataControls();
	for (int i = 0; i != pDataControls->size(); i++)
	{
		if (pDataControls->at(i))
		{
			pDataControls->at(i)->setDimData(nullptr);
			pDataControls->at(i)->Hide(bClearState);
		}
	}

	if (IcDrawer* pDrawer = manager->getDrawer())
	{
		pDrawer->deactivateDrawer();
		pDrawer->getMyDrawList()->removeAllFromDrawList();
		delete pDrawer;
		manager->setDrawer(nullptr);
	}
}

bool DynInputUIDimensionsState::onMouseMove(IcDynamicInputUIManager* manager)
{
	IIcadSysvarManager* pISysvar = getSysvarManagerInterface(getIcadInterfaces());
	auto pDataControls = manager->getDataControls();
	for (unsigned int i = 0; i != pDataControls->size(); i++)
	{
		CDynamicEdit* pEdit = pDataControls->getAt(i);
		if (!pEdit)
			continue;

		if ((!pEdit->m_hWnd || !IsWindowVisible(pEdit->m_hWnd)) && !pEdit->ShowIsRequired())
			continue;

		OdDbDimData* pDimData = pEdit->dimData();
		if (pDimData &&
			pDimData->dimension().isNull() &&
			pDimData->isDimResultantAngle())
		{
			OdGePoint3d wcsLastPoint, ucsLastPoint;
			int retVal = pISysvar->getVar(L"LASTPOINT", ucsLastPoint);
			OdGeVector3d direction3d = manager->getLastUcsPoint() - ucsLastPoint;
			OdGeVector2d direction2d = direction3d.convert2d();
			if (pEdit->IsUnchanged())
			{
				if (!direction2d.isZeroLength())
					pEdit->setValue(Oda2PI - direction2d.angleToCCW(OdGeVector2d::kXAxis));
				else
					pEdit->setValue(0.0);
			}
		}
	}
	return false;
}

void DynInputUIDimensionsState::activate(IcDynamicInputUIManager* manager)
{
	if (!manager->getDrawer())
	{
		manager->setDrawer(new IcDrawer());
	}

	if (IcDrawer* pDrawer = manager->getDrawer())
	{
		pDrawer->activateDrawer();
		pDrawer->setDrawArea(IcDrawer::kActiveViewportOnly);
	}

	int dyndivis = 0;
	icedGetVar(L"DYNDIVIS", dyndivis);

	if(auto pDimDataArray = manager->getDimData())
	{
		auto pDataControls = manager->getDataControls();

		int countDim = 0;
		int firstEditableIndex = -1;
		// creates controls to input data for dimensions
		for (unsigned int i = 0; i != pDimDataArray->size(); i++)
		{
			CDynamicEdit *pControl = nullptr;
			OdDbDimData* pDimData = pDimDataArray->getAt(i);
			if (pDimData)
			{
				bool isVisible = manager->isDimVisible(pDimData);
				if ( pDimData->isDimEditable() && ((dyndivis == DynInputValues::KOnlyFirstDynamicDimension && countDim > 0) ||
					(dyndivis == DynInputValues::KOnlyFirstTwoDynamicDimensions && countDim > 1)))
					isVisible = false;

				if (isVisible)
					countDim++;

				if (firstEditableIndex < 0 && pDimData->isDimEditable())
					firstEditableIndex = i;

				if(pDataControls)
				{
					if (i >= pDataControls->size())
					{
						CPoint dcPosition;
						OdGePoint3d wcsTextPosition;

						pControl = new CDynamicDimEdit();

						bool isCreatedEditBox = manager->getBoxContainer()->createEditBox(dcPosition.x, dcPosition.y, false, pControl);
						if (isCreatedEditBox && pControl && pControl->m_hWnd)
						{
							pDataControls->append(pControl);
						}
						else
						{
							manager->getBoxContainer()->deleteBox(pControl);
						}
					}
					else
					{
						pControl = pDataControls->getAt(i);
					}
				}

				if (!pControl)
					continue;

				pControl->setDimData(pDimData);

				if (isVisible)
					pControl->Show();

				if (!pDimData->dimension().isNull() && manager->getDrawer())
				{
					manager->getDrawer()->getMyDrawList()->addObjectToDrawList(pDimData->dimension());
					pDimData->dimension()->setVisibility(isVisible ? OdDb::Visibility::kVisible : OdDb::Visibility::kInvisible);
				}
			}
		}

		if (pDimDataArray->size() && pDataControls->size() && firstEditableIndex >= 0)
			manager->setActiveControl(pDataControls->getAt(firstEditableIndex));
	}
	if (!manager->getActiveControl())
		manager->changeActiveControl(false);

	manager->updateDimensionValue();
	manager->resizeControls();
}

bool DynInputUIDimensionsState::getCommand(IcDynamicInputUIManager* manager, bool &sendEnter, CString &command)
{
	IcEdBaseJig* pCurrentJig = manager->getCurrentJig();
	std::pair<OdDbGripDataPtr, OdDbObjectId> appDataMap = manager->getAppDataMapFirstElement();
	CDynamicEdit* activeControl = manager->getActiveControl();
	if ((pCurrentJig || !appDataMap.second.isNull()) && activeControl)
	{
		if (activeControl->getStatus() != CDynamicEdit::eStateUnchanged)
		{
			CString text;
			activeControl->GetWindowTextW(text);
			int isExpression = 0;
			if(IIcOsnapManager* pOsnapManager = IcCoreExternal::getIcadOsnapManager())
			{
				IcDbOsnapGlyphBasePtr glyph;
				if(pOsnapManager->findOSnap(IcString(text), glyph) == RTNORM)
				{
					command = text;
					return true;
				}
			}
			IIcadSysvarManager* pISysvar = getSysvarManagerInterface(getIcadInterfaces());

			double val = 0.0;
			OdStaticRxObject<IcadUnitsFormatter> unitsFormatter;
			bool isValidValue = unitsFormatter.unformatOrientation(val, OdString((LPCTSTR)text)) == RTNORM ||
								unitsFormatter.unformatLinear(val, OdString((LPCTSTR)text)) == RTNORM;

			// string input
			if ((!(activeControl->IsKindOf(RUNTIME_CLASS(CDynamicStringEdit)))) && // avoid problem of TEXT command to insert string "2,2"
				(isValidValue || text[0] == L'-') &&
				!activeControl->isKeyword(IcString(text)))
			{
				double dValue = 0.0;
				OdDbDimData* pDimData = activeControl->dimData();
				if (!pDimData)
				{
					ASSERT(FALSE);
					return true;
				}

				if (pCurrentJig)
				{
					if (activeControl->getValue(dValue))
					{
						auto result = pCurrentJig->setDimValue(pDimData, dValue);
						if (result != Icad::eOk)
						{
							if (result == Icad::eInvalidInput)
							{
								activeControl->setStatus(CDynamicEdit::eStateError);
							}
							return true; // stop keyboard message but don't send anything to command line
						}
						else
						{
							if (IIcadViewInternal* pView = getViewInternalInterface(getIcadInterfaces()))
							{
								if (IIcadDragger * dragger = pView->getCurrentDragger())
								{
									OdGePoint3d point = manager->getLastWcsPoint();
									dragger->dragOne(point, pView->getDrawingSurface());
									manager->updateDimensionValue();
								}
							}
						}
					}

					if(manager->countDimData() > 1/* && !pDimData->isDimResultantLength()*/)
						sendEnter = true;
				}
				else if (pDimData->dimValueFunc())
				{
					IIcadViewInternal* pView = getViewInternalInterface(getIcadInterfaces());
					ASSERT(pView != NULL);
					if (pView == NULL)
						return false;

					IcGripPointManagerPtr pGripPointManager = pView->getGripPointManager();
					ASSERT(!pGripPointManager.isNull());
					if (pGripPointManager.isNull())
						return false;

					if (activeControl->getValue(dValue))
					{
						OdDbEntityPtr pEnt = appDataMap.second.openObject(OdDb::kForWrite);
						pDimData->dimValueFunc()(pDimData, pEnt, dValue, OdGeVector3d());
					}

					pGripPointManager->gripUpdate();
					pGripPointManager->done(RTCAN);
				}

				command = text;
				return true;
			}
			else
			{
				command = text; // place input text on command line and execute it
				return true;
			}
		}
		else
		{
			//jig will get RTNONE and it will compute
			command = L"";
			return false;
		}
	}

	return false;
}
//DynInputUIDimensionsState
//======================================================================================
