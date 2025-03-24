#pragma once

#include "EventDelegator.h"
#include "Exceptions.h"
#include "Trackers.h"
#include "Uio.h"
#include "Uio.Prompt.h"

#include "Ge/GePoint3d.h"

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <vector>

class Renderer;

//--------------------------------------------------------------------------------------------------

struct UioKeyword
{
	int Index = -1;
	CString Code;
	CString Shortcut;
};

class IoKeywords : public std::vector<UioKeyword>
{
public:

	void Initialize(CString value);

	int Find(const CString& value, UioKeyword& found);
	// throw exception
	void Check(CString value);

	void Clear();
};

//--------------------------------------------------------------------------------------------------

class UioSteps : public std::queue<CString>
{
	friend class UserIO;

public:

	void Initialize(CString value);

	void Clear();

	void Next(Uio::EOSnap& osnap, IoKeywords& keywords);

private:

	bool HasOsnap(CString value, Uio::EOSnap& osnap);

	bool HasKeyword(CString valaue, IoKeywords& keywords);
};

//--------------------------------------------------------------------------------------------------

struct UioResult
{
	Uio::EReturn Return = Uio::EReturn::None;

	double Real = 0.0;
	int Integer = 0;
	OdGePoint3d Point;
	CString String;
	UioKeyword Keyword;

	void Initialize();
};

//--------------------------------------------------------------------------------------------------

struct UioState
{
	Renderer* RendererPtr = nullptr;
	TrackerStack Trackers;

	CString Command;
	CString Prompt;
	CString Keyword;
	IoKeywords Keywords;
	UioSteps Steps;
	int Options = 0;

	Uio::EWait Wait = Uio::EWait::None;
	Uio::EOSnap OSnap = Uio::EOSnap::None;
	struct {
		int Flag = Uio::EFilter::None;
		double Angle = 0.0;
		double Length = 0.0;
		double X = 0.0;
		double Y = 0.0;
	} Filter;

	CPoint MousePoint;
	OdGePoint3d LastPoint;

	void Initialize();

	void Clear();

	void Set(const CString& prompt, int options, const wchar_t* keyword, TrackerBase* pTracker);
};

//--------------------------------------------------------------------------------------------------

class UserIO : public EventDelegator
{
protected: // WorkerThread

	bool OnSignal(std::shared_ptr<EventWrapper> wrapper) override;

public: // EventDelegator

	bool SendSignal(SignalParams* pSignal) override;

protected:

	bool OnCommand(SignalParams* pSignal) override;

	bool OnInput(SignalParams* pSignal) override;

	bool OnLButtonDown(SignalParams* pSignal) override;

	bool OnLButtonUp(SignalParams* pSignal) override { return false; }
	// Fast Pan
	bool OnMButtonDown(SignalParams* pSignal) override;

	bool OnMButtonUp(SignalParams* pSignal) override { return false; }
	// Context Menu
	bool OnRButtonDown(SignalParams* pSignal) override { return false;  }

	bool OnRButtonUp(SignalParams* pSignal) override { return false; }

	bool OnMouseMove(SignalParams* pSignal) override;

public:

	bool UseThread = false;
	UioState State;
	UioResult Result;
	// For CancelCommand
	bool HasPostProcess = false;
	// Wait process (different with WorkerThread)
	std::mutex WaitMutex;
	std::condition_variable WaitCondition;

	UserIO(bool useThread);

	virtual ~UserIO();

	bool IsActivated();
	// lock or unlock(false)
	void LockProcess(bool value = true);

	void SetRenderer(Renderer* pRenderer);

	bool CancelCommand(bool hasPostProcess);

	void StandbyCommand();

	void CommandCompleted();

public:

	OdGePoint3d GetPoint(const CString& prompt, int options, const wchar_t* keyword = nullptr, TrackerBase* pTracker = nullptr);

	bool SetInteger(int value);

	bool SetPoint(OdGePoint3d value, bool unlock);

	bool SetReal(double value);

	bool SetString(const CString& value);

public:

	bool SendCommand(const CString& value);

	bool SendPrompt(const CString& prompt, const CString& keyword, int options);

	bool SendEcho(const CString& value);

	bool SendError(const CString& value);

private:

	bool ParseCommand(CString value);

	bool ParseFilter(CString value);

	bool ParseInteger(CString value) { RETURN_FALSE; }

	bool ParseKeyword(CString value);

	bool ParseOSnap(CString value);

	bool ParseOtherInput(CString value);

	bool ParsePoint(CString value);

	bool ParseReal(CString value);

	bool ParseString(CString value) { RETURN_FALSE; }
};
