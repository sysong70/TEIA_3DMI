#pragma once

#include "Ge/GePoint3dArray.h"
#include "Ed/EdCommandStack.h"
#include "Ed/EdUserIO.h"
#include "Json.h"

class Renderer;

#define RESULT_OK   0
#define RESULT_ERR -1

//--------------------------------------------------------------------------------------------------

class CommandBase : public OdEdCommand
{
public:

	CommandBase(Renderer* renderer, OdEdCommandContext* context);

	virtual void Initialize(OdEdCommandContext* context) {}
	virtual void Cancel() {}

protected:

	virtual int	OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) { return RESULT_OK; }
	virtual int OnLButtonDown(UINT nFlags, CPoint point) { return RESULT_OK; }
	virtual int OnLButtonUp(UINT nFlags, CPoint point) { return RESULT_OK; }
	virtual int OnMButtonDown(UINT nFlags, CPoint point) { return RESULT_OK; }
	virtual int OnMButtonUp(UINT nFlags, CPoint point) { return RESULT_OK; }
	virtual int OnRButtonDown(UINT nFlags, CPoint point) { return RESULT_OK; }
	virtual int OnRButtonUp(UINT nFlags, CPoint point) { return RESULT_OK; }
	virtual int OnMouseMove(UINT nFlags, CPoint point) { return RESULT_OK; }
	virtual int OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) { return RESULT_OK; }

protected:

	Renderer* m_pRenderer = nullptr;

	CPoint m_mousePoint;
	CPoint m_startPoint;
	CPoint m_endPoint;
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
public:



protected:

	bool m_continue = true;
	bool m_useSelectionSet = false;
	int m_commandStage = 0;
	OdGePoint3dArray m_points;
	OdEdInputTrackerPtr m_tracker;
};



class FastPanCommand : public SingleCommandBase
{
public:
};



class LineCommand : public TrackerCommandBase
{

};