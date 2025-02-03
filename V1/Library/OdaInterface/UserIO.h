#pragma once

#include "EventDelegator.h"
#include "Prompt.h"
#include "Trackers.h"

#include "Ge/GePoint3d.h"

class Renderer;

//--------------------------------------------------------------------------------------------------

class UserIO : public EventDelegator
{
	friend class Renderer;

	enum class EMode
	{
		None,
		Integer,
		Real,

		Point,
		Distance,
		String,
	};
	// Object snap override
	enum class EOverride
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

	enum class EReturn
	{
		None,
		Integer,
		Real,
		Point,
		String,

		Cancel,
		CommaCommand,
		Keyword,
	};

protected:

	struct KeywordIndexer
	{
		struct Data
		{
			CString Code;
			wchar_t Key;
		};

		std::vector<Data> Indexer;

		void Initialize(CString value);

		int Find(const CString& value, Data& found);
		// throw exception
		void Check(CString value);

		void Clear();
	};

	struct Data
	{
		Renderer* pRenderer = nullptr;
		TrackerStack trackers;

		CString Command;
		CString Prompt;
		KeywordIndexer Keywords;

		EMode Mode = EMode::None;
		EOverride OSnap = EOverride::None;
		EReturn Return = EReturn::None;

		// Result values
		double Real = 0.0;
		int Integer = 0;
		OdGePoint3d Point;
		CString String;

		CString Keyword;
		int KeywordIndex = -1;

		CPoint MousePoint;
		OdGePoint3d LastPoint;

		void Clear();
	}
	m_data;

	// Wait process (different with WorkerThread)
	std::mutex m_waitMutex;
	std::condition_variable m_waitCondition;

protected: // WorkerThread

	bool OnSignal(std::shared_ptr<EventWrapper> wrapper) override;

protected: // EventDelegator

	bool OnCommand(SignalArgs::Base* pSignal) override;

	bool OnContextCommand(SignalArgs::Base* pSignal) override;

	bool OnKeyDown(SignalArgs::Base* pSignal) override;

	bool OnLButtonDown(SignalArgs::Base* pSignal) override;

	bool OnLButtonUp(SignalArgs::Base* pSignal) override;

	bool OnMButtonDown(SignalArgs::Base* pSignal) override;

	bool OnMButtonUp(SignalArgs::Base* pSignal) override;

	bool OnRButtonDown(SignalArgs::Base* pSignal) override;

	bool OnRButtonUp(SignalArgs::Base* pSignal) override;

	bool OnMouseMove(SignalArgs::Base* pSignal) override;

	bool OnInput(SignalArgs::Base* pSignal) override;

public:

	UserIO();

	virtual ~UserIO();

	bool IsActivated();

	void SetRenderer(Renderer* pRenderer);
	// lock or unlock(false)
	void LockProcess(bool value = true);

public:

	double GetDistance(const CString& prompt, const CString& keyword = L"", TrackerBase* pTracker = nullptr);

	OdGePoint3d GetPoint(const CString& prompt, const CString& keyword = L"", TrackerBase* pTracker = nullptr);

	bool SetDistance(double value);

	bool SetPoint(const OdGePoint3d& value);

public:

	bool Cancel();

	bool ParseCommand(const CString& value);

	bool ParseInteger(const CString& value);

	bool ParseKeyword(const CString& value);

	bool ParseOverride(const CString& value);

	bool ParsePoint(const CString& value);

	bool ParseReal(const CString& value);

	bool ParseString(const CString& value);

public:

	bool InputError(const CString& value);

	bool InputEcho(const CString& value);
};
