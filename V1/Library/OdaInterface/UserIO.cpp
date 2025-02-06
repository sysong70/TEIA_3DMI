#include "stdafx.h"
#include "UserIO.h"
#include "Application.h"
#include "Commands.h"
#include "CommandStack.h"
#include "Renderer.h"

#include "Ed/EdUserIO.h"

//**************************************************************************************************

void IoKeywords::Initialize(CString value)
{
	Clear();

	if (value.IsEmpty()) {
		return;
	}

	value.MakeUpper();
	int index = 0;

	WStringArray byLanguage;
	WStr::Split(value, L'|', byLanguage);

	WStringArray byKeyword;
	WStr::Split(byLanguage[0], L'/', byKeyword);

	for (auto item : byKeyword) {
		CString code = WStr::Front(item, L'(', false, false);
		CString shortcut = WStr::Back(item, L'(', L')', false, false);
		ASSERT(code.IsEmpty() == false && shortcut.IsEmpty() == false);

		push_back({ index++, code, shortcut });
	}
}



int IoKeywords::Find(const CString& value, IoKeyword& found)
{
	for (auto it = cbegin(); it != cend(); ++it) {
		if (it->Code.CompareNoCase(value) == 0 || it->Shortcut.CompareNoCase(value) == 0) {
			found = *it;
			return it->Index;
		}
	}

	return -1;
}



void IoKeywords::Check(CString value)
{
	value.MakeUpper();

	IoKeyword found;
	int index = Find(value, found);
	if (index > -1) {
		DEBUG_STOP;
		//throw OdEdKeyword(index, OdString(found.Code));
	}
}



void IoKeywords::Clear()
{
	clear();
}

//**************************************************************************************************

void IoSteps::Initialize(CString value)
{
	Clear();

	WStringArray steps;
	WStr::Split(value, L' ', steps);
	for (auto item : steps) {
		push(item);
	}
}



void IoSteps::Clear()
{
	// WARNING - Clear(fast initialize)
	IoSteps dummy;
	std::swap(*this, dummy);
}



void IoSteps::Next(Io::EOSnap& osnap, IoKeywords& keywords)
{
#define FrontAndPop() front(); pop()

	if (size() == 0) {
		return;
	}

	CString value = FrontAndPop();
	if (value.IsEmpty()) {
		return;
	}

	HasOsnap(value, osnap);
	HasKeyword(value, keywords);

#undef FrontAndPop
}



bool IoSteps::HasOsnap(CString value, Io::EOSnap& osnap)
{
	if (value[0] != PRE_OSNAP) {
		return false;
	}

	CString snap = CString(value[1]).MakeUpper() + value.Mid(2);

#define SetOSnap(x) else if (snap == L#x) { \
	osnap = Io::EOSnap::x; \
	return true; \
}

	if (value.IsEmpty()) {
		osnap = Io::EOSnap::None;
		return false;
	}
	SetOSnap(X)
	SetOSnap(Y)
	SetOSnap(Point)
	SetOSnap(End)
	SetOSnap(Mid)
	SetOSnap(Intersection)
	SetOSnap(Perpendicular)
	SetOSnap(Center)
	SetOSnap(Quadrant)
	SetOSnap(Near)

	return false;

#undef SetOSnap
}



bool IoSteps::HasKeyword(CString value, IoKeywords& keywords)
{
	if (value[0] != PRE_KEYWORD) {
		return false;
	}

	IoKeyword found;
	int index = keywords.Find(value.Mid(1), found);

	if (index > -1) {
		throw OdEdKeyword(found.Index, found.Code.GetBuffer());
	}
	else {
		RETURN_FALSE;
	}
}

//**************************************************************************************************

void IoResult::Initialize()
{
	Real = 0.0;
	Integer = 0;
	Point = OdGePoint3d::kOrigin;
	String.Empty();

	Keyword.Index = -1;
	Keyword.Code.Empty();
	Keyword.Shortcut.Empty();
}

//**************************************************************************************************

void UserIO::IoState::Initialize()
{
	Mode = Io::EMode::None;
	OSnap = Io::EOSnap::None;

	MousePoint = CPoint(0, 0);
}



void UserIO::IoState::Clear()
{
	Command.Empty();
	Prompt.Empty();
	Keyword.Empty();
	Keywords.Clear();
	Steps.Clear();
	Options = 0;
}



void UserIO::IoState::Set(const CString& prompt, int options, const wchar_t* keyword, TrackerBase* pTracker)
{
	Prompt = prompt;

	if (keyword != nullptr) {
		Keyword = keyword;
		Keywords.Initialize(Keyword);
	}
	else {
		Keyword.Empty();
	}

	Options = options;

	Trackers.Push(pTracker);
}

//**************************************************************************************************

#define theRenderer		(*m_state.pRenderer)
#define theGsView		m_state.pRenderer->GetGsView()
#define theDelivery		m_state.pRenderer->GetDelivery().userIO
#define theCoord		m_state.pRenderer->GetCoordConvertor()
#define theKeywords		m_state.Keywords
#define theTrackers		m_state.Trackers

//--------------------------------------------------------------------------------------------------

bool UserIO::OnSignal(std::shared_ptr<EventWrapper> wrapper)
{
	WorkerThread::Event e = (WorkerThread::Event)wrapper->Type;

	if (e == WorkerThread::Event::Signal) {
		SignalArgs::Base* pSignal = (SignalArgs::Base*)wrapper->EventData;
		if (pSignal == nullptr) {
			RETURN_FALSE;
		}

		if (pSignal->Target == Signal::Target::View) {
#define OnAction(x) Signal::View::Action::On##x: On##x(pSignal); break

			switch ((Signal::View::Action)pSignal->Action) {
			case OnAction(ContextCommand);
			case OnAction(KeyDown);
			case OnAction(LButtonDown);
			case OnAction(LButtonUp);
			case OnAction(MButtonDown);
			case OnAction(MButtonUp);
			case OnAction(RButtonDown);
			case OnAction(RButtonUp);
			case OnAction(MouseMove);
			case OnAction(Input);

			default:
				RETURN_FALSE;
			}

#undef OnAction
		}
		else if (pSignal->Target == Signal::Target::UserIO) {
#define OnAction(x) Signal::UserIO::Action::On##x: On##x(pSignal); break

			switch ((Signal::UserIO::Action)pSignal->Action) {
			case OnAction(Input);

			default:
				RETURN_FALSE;
			}
		}
		else {
			RETURN_FALSE;
		}

#undef OnAction
	}
	else if (e == WorkerThread::Event::Close) {
		return OnClose();
	}
	else {
		RETURN_FALSE;
	}

	return true;
}

//--------------------------------------------------------------------------------------------------

bool UserIO::OnCommand(SignalArgs::Base* pSignal)
{
	SignalArgs::Command& signal = *(SignalArgs::Command*)pSignal;
	CommandInfo* pInfo = TheCommandStack.Find((UINT)signal.Id);

	if (pInfo == nullptr) {
		return false;
	}

	CString name = pInfo->pCommand->Name().MakeUpper();
	SendCommand(name);

	if (TheCommandStack.Execute(pInfo->pCommand, &theRenderer)) {
		m_state.Command = name;
		m_state.Steps.Initialize(pInfo->Step);
		return true;
	}
	else {
		SendError(Io::ErrorInvalidCommand);
	}

	return false;
}



bool UserIO::OnLButtonDown(SignalArgs::Base* pSignal)
{
	SignalArgs::Mouse& signal = *(SignalArgs::Mouse*)pSignal;
	CPoint point = { signal.X, signal.Y };

	SetPoint(theCoord.ToEyeToWorld(point));

	return true;
}



bool UserIO::OnLButtonUp(SignalArgs::Base* pSignal)
{
	return false;
}



bool UserIO::OnMButtonDown(SignalArgs::Base* pSignal)
{
	SignalArgs::Mouse& signal = *(SignalArgs::Mouse*)pSignal;
	m_state.MousePoint = CPoint(signal.X, signal.Y);

	return true;
}



bool UserIO::OnMButtonUp(SignalArgs::Base* pSignal)
{
	return false;
}



bool UserIO::OnRButtonDown(SignalArgs::Base* pSignal)
{
	// TODO - Context menu
	return false;
}



bool UserIO::OnRButtonUp(SignalArgs::Base* pSignal)
{
	return false;
}



bool UserIO::OnMouseMove(SignalArgs::Base* pSignal)
{
	SignalArgs::Mouse& signal = *(SignalArgs::Mouse*)pSignal;
	CPoint point = { signal.X, signal.Y };

	if (signal.Flags & MK_MBUTTON) {
		theRenderer.Dolly(point.x - m_state.MousePoint.x, point.y - m_state.MousePoint.y);
		m_state.MousePoint = point;
	}
	else {
		OdGePoint3d gePoint = theCoord.ToEyeToWorld(point);
		theTrackers.SetValue(gePoint);
	}

	// WARNING - thread operation failed, do not call RedrawWindow()
	theRenderer.PostPaintSignal();

	return true;
}



bool UserIO::OnInput(SignalArgs::Base* pSignal)
{
	static wchar_t* TRIMER = L" \t\r\n";

	SignalArgs::Text& signal = *(SignalArgs::Text*)pSignal;
	CString value = signal.Buffer;

	value.Trim(TRIMER);
	value.MakeUpper();

	if (value.IsEmpty() || value == KEY_CANCEL) {
		return CancelCommand();
	}

	// Activated command
	if (m_state.Command.IsEmpty() == false) {
		// Has keyword?
		if (value[0] == PRE_KEYWORD && m_state.Keyword.IsEmpty() == false) {
			return ParseKeyword(value);
		}
		else if (value[1] == PRE_OSNAP) {
			return ParseOSnap(value);
		}

		// Wait input
		bool success = false;

		switch (m_state.Mode) {
		case Io::EMode::Integer:	success = ParseInteger(value); break;
		case Io::EMode::Point:		success = ParsePoint(value); break;
		case Io::EMode::Real:		success = ParseReal(value); break;
		case Io::EMode::String:		success = ParseString(value); break;

		default:
			DEBUG_STOP;
			break;
		}
	}

	return ParseCommand(value);
}

//--------------------------------------------------------------------------------------------------

UserIO::UserIO()
	: EventDelegator()
{
}



UserIO::~UserIO()
{
	LockProcess(false);
}



bool UserIO::IsActivated()
{
	return m_state.Mode != Io::EMode::None;
}



void UserIO::LockProcess(bool value)
{
	if (value) {
		theTrackers.Initialize(theGsView);
		std::unique_lock<std::mutex> lock(m_waitMutex);
		m_waitCondition.wait(lock);
	}
	else {
		theTrackers.Clear(theGsView);
		m_waitCondition.notify_one();
	}
}



void UserIO::SetRenderer(Renderer* pRenderer)
{
	m_state.pRenderer = pRenderer;
	StandbyCommand();
}



bool UserIO::CancelCommand()
{
	if (m_state.Command.IsEmpty() == false) {
		m_state.Command.Empty();
		m_result.Return = Io::EReturn::Cancel;

		LockProcess(false);
	}

	return true;
}



void UserIO::StandbyCommand()
{
	m_state.Clear();
	theDelivery.StandbyCommand(Io::PromptTypeACommand);
}

//--------------------------------------------------------------------------------------------------

OdGePoint3d UserIO::GetPoint(const CString& prompt, int options, const wchar_t* keyword, TrackerBase* pTracker)
{
	m_state.Initialize();
	m_state.Set(prompt, options, keyword, pTracker);
	m_state.Mode = Io::EMode::Point;
	m_state.Steps.Next(m_state.OSnap, m_state.Keywords);

	RubberBand rubberBand;
	if (GetBit(options, Io::eRubberBand)) {
		rubberBand.SetBasePoint(m_state.LastPoint);
		theTrackers.Push(&rubberBand);
	}

	// CHECK
	RubberRect rubberRect;
	if (GetBit(options, Io::eRubberRect)) {
		rubberBand.SetBasePoint(m_state.LastPoint);
		theTrackers.Push(&rubberRect);
	}

	SendPrompt(prompt, keyword);
	LockProcess();

	switch (m_result.Return) {
	case Io::EReturn::Point:	return m_result.Point;
	case Io::EReturn::Keyword:	throw OdEdKeyword(m_result.Keyword.Index, m_result.Keyword.Code.GetBuffer());
	case Io::EReturn::Cancel:	throw OdEdCancel();

	default:
		ASSERT(m_result.Return != Io::EReturn::None);
		throw m_result;
	}

	DEBUG_STOP;
	return {};
}



bool UserIO::SetInteger(int value)
{
	m_result.Return = Io::EReturn::Integer;
	m_result.Integer = value;
	theTrackers.SetValue(value);

	LockProcess(false);
	return true;
}



bool UserIO::SetPoint(const OdGePoint3d& value)
{
	m_state.LastPoint = value;
	m_result.Return = Io::EReturn::Point;
	m_result.Point = value;
	theTrackers.SetValue(value);

	LockProcess(false);
	return true;
}



bool UserIO::SetReal(double value)
{
	m_result.Return = Io::EReturn::Real;
	m_result.Real = value;
	theTrackers.SetValue(value);

	LockProcess(false);
	return true;
}



bool UserIO::SetString(const CString& value)
{
	m_result.Return = Io::EReturn::Real;
	m_result.String = value;
	theTrackers.SetValue(value);

	LockProcess(false);
	return true;
}

//--------------------------------------------------------------------------------------------------

bool UserIO::SendCommand(const CString& value)
{
	theDelivery.PutCommand(value);
	return true;
}



bool UserIO::SendPrompt(const CString& prompt, const CString& keyword)
{
	theDelivery.PutPrompt(prompt, keyword);
	return true;
}



bool UserIO::SendEcho(const CString& value)
{
	theDelivery.PutEcho(value);
	return true;
}



bool UserIO::SendError(const CString& value)
{
	theDelivery.PutError(value);
	return true;
}

//--------------------------------------------------------------------------------------------------

bool UserIO::ParseCommand(CString& value)
{
	CommandInfo* pInfo = TheCommandStack.Find(value);

	if (pInfo == nullptr) {
		return false;
	}

	CString name = pInfo->Name.MakeUpper();

	if (TheCommandStack.Execute(pInfo->pCommand, &theRenderer)) {
		m_state.Command = name;
		return SendCommand(name);
	}
	else {
		SendError(Io::ErrorInvalidCommand);
	}

	return false;
}



bool UserIO::ParseInteger(CString& value)
{
	RETURN_FALSE;
}



bool UserIO::ParseKeyword(CString& value)
{
	IoKeyword found;
	// Shift PRE_KEYWORD
	int index = m_state.Keywords.Find(value.Mid(1), found);

	if (index > -1) {
		m_result.Return = Io::EReturn::Keyword;
		m_result.Keyword = found;

		LockProcess(false);
	}
	else {
		SendError(Io::ErrorInvalidKeyword);
	}

	return true;
}



bool UserIO::ParseOSnap(CString& value)
{
	// Shift PRE_OSNAP
	CString snap = CString(value[1]).MakeUpper() + value.Mid(2);

#define SetOSnap(x) else if (snap == L#x) { \
	m_state.OSnap = Io::EOSnap::x; \
	return true; \
}

	if (value.IsEmpty()) {
		m_state.OSnap = Io::EOSnap::None;
	}
	SetOSnap(X)
	SetOSnap(Y)
	SetOSnap(Point)
	SetOSnap(End)
	SetOSnap(Mid)
	SetOSnap(Intersection)
	SetOSnap(Perpendicular)
	SetOSnap(Center)
	SetOSnap(Quadrant)
	SetOSnap(Near)

#undef SetOSnap

	return false;
}



bool UserIO::ParseOtherInput(CString& value)
{
	if (GetBit(m_state.Options, Io::eAllowInteger)) return ParseInteger(value);
	if (GetBit(m_state.Options, Io::eAllowReal)) return ParseReal(value);

	return false;
}



bool UserIO::ParsePoint(CString& value)
{
	static wchar_t* CARTESIAN = L"%lf,%lf";
	static wchar_t* POLAR = L"%lf<%lf";

	wchar_t* pValue = (wchar_t*)value.GetString();
	double v1, v2;
	bool relative = false;

	if (value[0] == PRE_OSNAP) {
		value.TrimLeft(PRE_OSNAP);
		return ParseOSnap(value);
	}

	if (pValue[0] == PRE_RELATIVE) {
		relative = true;
		pValue++;
	}

	if (swscanf_s(pValue, CARTESIAN, &v1, &v2) == 2) {
		if (relative) {
			return SetPoint(m_state.LastPoint + OdGeVector3d(v1, v2, 0));
		}
		else {
			return SetPoint({ v1, v2, 0 });
		}
	}
	else if (swscanf_s(pValue, POLAR, &v1, &v2) == 2) {
		OdGePoint3d point(v1, 0, 0);
		point.rotateBy(v2, OdGeVector3d::kZAxis, relative ? m_state.LastPoint : OdGePoint3d::kOrigin);

		return SetPoint(point);
	}
	else if (m_state.Options > 0) {
		return ParseOtherInput(value);
	}
	else {
		return SendError(Io::ErrorInvalidPoint);
	}

	return false;
}



bool UserIO::ParseReal(CString& value)
{
	double result = 0.0;

	if (swscanf_s(value, L"%lf", &result) != 1) {
		return SendError(Io::ErrorInvalidValue);
	}

	if (GetBit(m_state.Options, Io::eNoZero) && OdZero(result)) {
		return SendError(Io::ErrorZeroValue);
	}
	if (GetBit(m_state.Options, Io::eNoNegative) && result < 0) {
		return SendError(Io::ErrorNegativeValue);
	}

	return SetReal(result);
}



bool UserIO::ParseString(CString& value)
{
	RETURN_FALSE;
}

#undef theRenderer
#undef theGsView
#undef theDelivery
#undef theCoord
#undef theKeywords
#undef theTrackers

#undef ThrowCancel
