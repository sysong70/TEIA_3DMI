#pragma once

#include "EventDelegator.h"
#include "Exceptions.h"
#include "Prompt.h"
#include "Trackers.h"

#include "Ge/GePoint3d.h"

class Renderer;

//--------------------------------------------------------------------------------------------------

namespace Io
{
	//  do not use enum class
	enum EInputOptions
	{
		eDefault = 0,
		eAllowInteger = 1,
		eAllowReal = eAllowInteger * 2,
		eNoZero = eAllowReal * 2,
		eNoNegative = eNoZero * 2,
		eRubberBand = eNoNegative * 2,
		eRubberRect = eRubberBand * 2,
		eUseLastPoint = eRubberRect * 2,
	};

	enum class EOSnap
	{
		None,
		// point filters
		X,
		Y,
		Z,
		XY,
		XZ,
		YZ,
		// snap
		Point,
		End,
		Mid,
		Intersection,
		Perpendicular,
		Center,
		Quadrant,
		Near,
	};

	enum class EMode
	{
		None,
		Integer,
		Point,
		Real,
		String,
	};

	enum class EReturn
	{
		None,
		Integer,
		Point,
		Real,
		String,

		Cancel,
		Keyword,
	};
}

//--------------------------------------------------------------------------------------------------

struct IoKeyword
{
	int Index = -1;
	CString Code;
	CString Shortcut;
};

class IoKeywords : public std::vector<IoKeyword>
{
public:

	void Initialize(CString value);

	int Find(const CString& value, IoKeyword& found);
	// throw exception
	void Check(CString value);

	void Clear();
};

//--------------------------------------------------------------------------------------------------

class IoSteps : public std::queue<CString>
{
	friend class UserIO;

public:

	void Initialize(CString value);

	void Clear();

	void Next(Io::EOSnap& osnap, IoKeywords& keywords);

private:

	bool HasOsnap(CString value, Io::EOSnap& osnap);

	bool HasKeyword(CString valaue, IoKeywords& keywords);
};

//--------------------------------------------------------------------------------------------------

struct IoResult
{
	Io::EReturn Return = Io::EReturn::None;

	double Real = 0.0;
	int Integer = 0;
	OdGePoint3d Point;
	CString String;
	IoKeyword Keyword;

	void Initialize();
};

//--------------------------------------------------------------------------------------------------

class UserIO : public EventDelegator
{
	friend class Renderer;

protected:

	struct IoState
	{
		// References
		Renderer* pRenderer = nullptr;
		TrackerStack Trackers;
		// Input values
		CString Command;
		CString Prompt;
		CString Keyword;
		IoKeywords Keywords;
		IoSteps Steps;
		int Options = 0;
		// State values
		Io::EMode Mode = Io::EMode::None;
		Io::EOSnap OSnap = Io::EOSnap::None;
		// Interactive UI values
		CPoint MousePoint;
		OdGePoint3d LastPoint;

		void Initialize();

		void Clear();

		void Set(const CString& prompt, int options, const wchar_t* keyword, TrackerBase* pTracker);
	};

	IoState m_state;
	IoResult m_result;

	// Wait process (different with WorkerThread)
	std::mutex m_waitMutex;
	std::condition_variable m_waitCondition;

protected: // WorkerThread

	bool OnSignal(std::shared_ptr<EventWrapper> wrapper) override;

protected: // EventDelegator

	bool OnCommand(SignalArgs::Base* pSignal) override;

	bool OnLButtonDown(SignalArgs::Base* pSignal) override;

	bool OnLButtonUp(SignalArgs::Base* pSignal) override;
	// Fast Pan
	bool OnMButtonDown(SignalArgs::Base* pSignal) override;

	bool OnMButtonUp(SignalArgs::Base* pSignal) override;
	// Context Menu
	bool OnRButtonDown(SignalArgs::Base* pSignal) override;

	bool OnRButtonUp(SignalArgs::Base* pSignal) override;

	bool OnMouseMove(SignalArgs::Base* pSignal) override;

	bool OnInput(SignalArgs::Base* pSignal) override;

public:

	UserIO();

	virtual ~UserIO();

	bool IsActivated();
	// lock or unlock(false)
	void LockProcess(bool value = true);

	void SetRenderer(Renderer* pRenderer);

	bool CancelCommand();

	void StandbyCommand();

public:

	OdGePoint3d GetPoint(const CString& prompt, int options, const wchar_t* keyword = nullptr, TrackerBase* pTracker = nullptr);

	bool SetInteger(int value);

	bool SetPoint(const OdGePoint3d& value);

	bool SetReal(double value);

	bool SetString(const CString& value);

public:

	bool SendCommand(const CString& value);

	bool SendPrompt(const CString& prompt, const CString& keyword);

	bool SendEcho(const CString& value);

	bool SendError(const CString& value);

private:

	bool ParseCommand(CString& value);

	bool ParseInteger(CString& value);

	bool ParseKeyword(CString& value);

	bool ParseOSnap(CString& value);

	bool ParseOtherInput(CString& value);

	bool ParsePoint(CString& value);

	bool ParseReal(CString& value);

	bool ParseString(CString& value);
};
