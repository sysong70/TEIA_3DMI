#pragma once

#include "Ge/GePoint3dArray.h"
#include "Ed/EdCommandStack.h"
#include "Ed/EdUserIO.h"
#include "Json.h"

//--------------------------------------------------------------------------------------------------

class CommandBase : public OdEdCommand
{
public:

	virtual void Initialize(OdEdCommandContext* pCmdCtx);
	virtual void Cancel();

protected:


};

//--------------------------------------------------------------------------------------------------

class SingleCommandBase : public CommandBase
{

};



class OpenCommand : public SingleCommandBase
{
public:


};

//--------------------------------------------------------------------------------------------------

class TrackerCommandBase : public CommandBase
{
protected:

	bool m_continue = true;
	bool m_useSelectionSet = false;
	int m_commandStage = 0;
	OdGePoint3dArray m_points;
	OdEdInputTrackerPtr m_tracker;
};



class LineCommand : public TrackerCommandBase
{

};