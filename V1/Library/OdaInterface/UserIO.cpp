#include "stdafx.h"

#include "Application.h"
#include "Commands.h"
#include "Renderer.h"
#include "UserIO.h"

#include "Ed/EdUserIO.h"

#include "WStr.h"
#include <atltypes.h>

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



int IoKeywords::Find(const CString& value, UioKeyword& found)
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

	UioKeyword found;
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

void UioSteps::Initialize(CString value)
{
	Clear();

	WStringArray steps;
	WStr::Split(value, L' ', steps);
	for (auto item : steps) {
		push(item);
	}
}



void UioSteps::Clear()
{
	// WARNING - Clear(fast initialize)
	UioSteps dummy;
	std::swap(*this, dummy);
}



void UioSteps::Next(Uio::EOSnap& osnap, IoKeywords& keywords)
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



bool UioSteps::HasOsnap(CString value, Uio::EOSnap& osnap)
{
	if (value[0] != PRE_OSNAP) {
		return false;
	}

	CString snap = CString(value[1]).MakeUpper() + value.Mid(2);

#define SetOSnap(x) else if (snap == L#x) { \
	osnap = Uio::EOSnap::x; \
	return true; \
}

	if (value.IsEmpty()) {
		osnap = Uio::EOSnap::None;
		return false;
	}
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



bool UioSteps::HasKeyword(CString value, IoKeywords& keywords)
{
	if (value[0] != PRE_KEYWORD) {
		return false;
	}

	UioKeyword found;
	int index = keywords.Find(value.Mid(1), found);

	if (index > -1) {
		throw OdEdKeyword(found.Index, found.Code.GetBuffer());
	}
	else {
		RETURN_FALSE;
	}
}

//**************************************************************************************************

void UioResult::Initialize()
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

void UioState::Initialize()
{
	Wait = Uio::EWait::None;
	OSnap = Uio::EOSnap::None;

	Filter.Flag = Uio::EFilter::None;
	Filter.Angle = 0.0;
	Filter.Length = 0.0;
	Filter.X = 0.0;
	Filter.Y = 0.0;
}



void UioState::Clear()
{
	Command.Empty();
	Prompt.Empty();
	Keyword.Empty();
	Keywords.Clear();
	Steps.Clear();
	Options = 0;
}



void UioState::Set(const CString& prompt, int options, const wchar_t* keyword, TrackerBase* pTracker)
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

#define theRenderer		(*State.RendererPtr)
#define theGsViewPtr	State.RendererPtr->GetGsView()
#define theDelivery		State.RendererPtr->Delivery.UserIO
#define theCoord		State.RendererPtr->Coordinate
#define theKeywords		State.Keywords
#define theTrackers		State.Trackers

//--------------------------------------------------------------------------------------------------

bool UserIO::OnSignal(std::shared_ptr<EventWrapper> wrapper)
{
	WorkerThread::Event e = (WorkerThread::Event)wrapper->Type;

	if (e == WorkerThread::Event::Signal) {
		SignalParams* pSignal = (SignalParams*)wrapper->EventData;
		if (pSignal == nullptr) {
			RETURN_FALSE;
		}

		if (pSignal->Target == Sgn::ETarget::View) {
#define OnAction(x) SgnView::Action::On##x: On##x(pSignal); break

			switch ((SgnView::Action)pSignal->Action) {
			case OnAction(ContextCommand);
			case OnAction(Command);
			case OnAction(KeyDown);
			case OnAction(LButtonDown);
			case OnAction(LButtonUp);
			case OnAction(MButtonDown);
			case OnAction(MButtonUp);
			case OnAction(RButtonDown);
			case OnAction(RButtonUp);
			case OnAction(MouseMove);

			default:
				RETURN_FALSE;
			}

#undef OnAction
		}
		else if (pSignal->Target == Sgn::ETarget::UserIO) {
#define OnAction(x) SgnUserIO::Action::On##x: On##x(pSignal); break

			switch ((SgnUserIO::Action)pSignal->Action) {
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

bool UserIO::SendSignal(SignalParams* pSignal)
{
	if (pSignal->Target == Sgn::ETarget::View) {
#define OnAction(x) SgnView::Action::On##x: On##x(pSignal); break

		switch ((SgnView::Action)pSignal->Action) {
		case OnAction(ContextCommand);
		case OnAction(Command);
		case OnAction(KeyDown);
		case OnAction(LButtonDown);
		case OnAction(LButtonUp);
		case OnAction(MButtonDown);
		case OnAction(MButtonUp);
		case OnAction(RButtonDown);
		case OnAction(RButtonUp);
		case OnAction(MouseMove);

		default:
			REMOVE_POINTER(pSignal);
			RETURN_FALSE;
		}

#undef OnAction
	}
	else if (pSignal->Target == Sgn::ETarget::UserIO) {
		switch ((SgnUserIO::Action)pSignal->Action) {
		case SgnUserIO::Action::OnInput:
			OnInput(pSignal);
			break;

		default:
			DEBUG_STOP;
			break;
		}
	}
	else {
		DEBUG_STOP;
	}

	REMOVE_POINTER(pSignal);
	return true;
}

//--------------------------------------------------------------------------------------------------

bool UserIO::OnCommand(SignalParams* pSignal)
{
	CommandSignal& signal = *(CommandSignal*)pSignal;
	CommandInfo* pInfo = TheCommandStack.Find((UINT)signal.Id);

	if (pInfo == nullptr) {
		return SendError(Uio::ErrorInvalidCommand);
	}

	CancelCommand(true);

	State.Command = pInfo->GetName();
	State.Steps.Initialize(pInfo->Step);
	SendCommand(State.Command);

	if (TheCommandStack.Execute(pInfo, &theRenderer) == false) {
		DEBUG_STOP;
	}

	return true;
}



bool UserIO::OnInput(SignalParams* pSignal)
{
	TextSignal& signal = *(TextSignal*)pSignal;
	CString value = signal.Buffer;

	if (value.IsEmpty()) {
		return false;
	}
	else if (State.Wait == Uio::EWait::String) {
		//:WARING - Spectial case, pass all string
		return ParseString(value);
	}

	// Remove white space
	value.Trim(L" \t\r\n");

	if (value.CompareNoCase(KEY_CANCEL) == 0) {
		return CancelCommand(false);
	}
	else if (State.Wait == Uio::EWait::None) {
		return ParseCommand(value);
	}

	// Compare by lower
	value.MakeLower();
	WCHAR first = value[0];

	if (first == PRE_KEYWORD) {
		return ParseKeyword(value);
	}
	else if (first == PRE_OSNAP) {
		return ParseOSnap(value);
	}
	else if (first == PRE_FILTERANGLE) {
		return ParseFilter(value);
	}
	else if (first == PRE_FILTERLENGTH) {
		// Case of "@100<45"
		if (value.Find(PRE_FILTERANGLE) > 0) {
			return ParsePoint(value);
		}
		// Case of "@100"
		ParseFilter(value);
	}
	else if (value.Find(PRE_FILTERX) == 0 || value.Find(PRE_FILTERY) == 0) {
		return ParseFilter(value);
	}

	switch (State.Wait) {
	case Uio::EWait::Integer:	return ParseInteger(value); break;
	case Uio::EWait::Real:		return ParseReal(value); break;
	case Uio::EWait::String:	return ParseString(value); break;

	case Uio::EWait::Point:
	case Uio::EWait::PointOrAngle:
	case Uio::EWait::PointOrLength:
		return ParsePoint(value);
		break;

	default:
		DEBUG_STOP;
		break;
	}

	return false;
}



bool UserIO::OnLButtonDown(SignalParams* pSignal)
{
	MouseSignal& signal = *(MouseSignal*)pSignal;
	State.MousePoint = { signal.X, signal.Y };

	OdGePoint3d point = theCoord.ToEyeToWorld(State.MousePoint);
	bool unlock = true;

	if (GetBit(State.Filter.Flag, Uio::EFilter::WaitX)) {
		State.Filter.X = point.x;
		BitOn(State.Filter.Flag, Uio::EFilter::X);
		BitOff(State.Filter.Flag, Uio::EFilter::WaitX);
		unlock = false;
	}
	else if (GetBit(State.Filter.Flag, Uio::EFilter::WaitY)) {
		State.Filter.Y = point.y;
		BitOn(State.Filter.Flag, Uio::EFilter::Y);
		BitOff(State.Filter.Flag, Uio::EFilter::WaitY);
		unlock = false;
	}

	SetPoint(point, unlock);

	return true;
}



bool UserIO::OnMButtonDown(SignalParams* pSignal)
{
	MouseSignal& signal = *(MouseSignal*)pSignal;
	State.MousePoint = CPoint(signal.X, signal.Y);

	return true;
}



bool UserIO::OnMouseMove(SignalParams* pSignal)
{
	MouseSignal& signal = *(MouseSignal*)pSignal;
	CPoint point = { signal.X, signal.Y };
	SignalParams* pPaintSignal = nullptr;

	if (signal.MiddleButton()) {
		theRenderer.Dolly(point.x - State.MousePoint.x, point.y - State.MousePoint.y);
		State.MousePoint = point;
		// WARNING - thread operation failed, do not call RedrawWindow()
		theRenderer.SendPaintSignal(UseThread);
	}
	else if (IsActivated()) {
		State.MousePoint = point;
		SetPoint(theCoord.ToEyeToWorld(point), false);
/*
		OdGePoint3d gePoint = theCoord.ToEyeToWorld(point);
		theTrackers.SetValue(gePoint);
		theTrackers.Invalidate();

		if (State.Trackers.UseDynamicInput) {
			RubberBandTracker* pTracker = theTrackers.GetRubberBand();
			if (pTracker != nullptr) {
				const int lineCount = 5;
				const int arcCount = 4;

				RubberBandTracker& tracker = *pTracker;
				pPaintSignal = new PaintSignal(theRenderer.ViewId);
				Json::Object& options = ((PaintSignal*)pPaintSignal)->Options;

				if (tracker.LengthGuide) {
					Json::Array& items = options.CreateArray(SKW_LENGTH);
					//:WARNING - unlock input control
					items.AddBoolean(GetBit(tracker.Params.GetFixed(), TrackerParams::eLength));
					items.AddReal(tracker.Result.Length);
					items.AddInteger(lineCount);

					for (int i = 0; i < lineCount; i++) {
						CPoint dcPoint = theCoord.ToWorldToEye(tracker.Result.LinePoints[i]);
						items.AddInteger(dcPoint.x);
						items.AddInteger(dcPoint.y);
					}
				}

				if (tracker.AngleGuide) {
					Json::Array& items = options.CreateArray(SKW_ANGLE);
					items.AddBoolean(GetBit(tracker.Params.GetFixed(), TrackerParams::eAngle));
					items.AddReal(OdaToDegree(tracker.Result.Angle));
					items.AddInteger(arcCount);

					for (int i = 0; i < arcCount; i++) {
						CPoint dcPoint = theCoord.ToWorldToEye(tracker.Result.ArcPoints[i]);
						items.AddInteger(dcPoint.x);
						items.AddInteger(dcPoint.y);
					}
				}
			}
		}
*/
	}
	else {
	}

	return true;
}

//--------------------------------------------------------------------------------------------------

UserIO::UserIO(bool useThread)
	: EventDelegator()
	, UseThread(useThread)
{
	if (UseThread) {
		Create();
	}
}



UserIO::~UserIO()
{
	LockProcess(false);
}



bool UserIO::IsActivated()
{
	return State.Wait != Uio::EWait::None;
}



void UserIO::LockProcess(bool value)
{
	if (value) {
		std::unique_lock<std::mutex> lock(WaitMutex);
		WaitCondition.wait(lock);
	}
	else {
		WaitCondition.notify_one();
	}
}



void UserIO::SetRenderer(Renderer* pRenderer)
{
	State.RendererPtr = pRenderer;
	StandbyCommand();
}



bool UserIO::CancelCommand(bool hasPostProcess)
{
	if (State.Command.IsEmpty() == false) {
		State.Initialize();
		Result.Return = Uio::EReturn::Cancel;
		//:WARNING - release Command
		LockProcess(false);
	}

	if (hasPostProcess == false) {
		HasPostProcess = false;
	}

	if (TheCommandStack.IsActivated()) {
		//:WARNING - wait CommandCompleted
		LockProcess();
		return true;
	}

	return false;
}



void UserIO::StandbyCommand()
{
	State.Clear();
	theDelivery.StandbyCommand(Uio::PromptTypeACommand);
}



void UserIO::CommandCompleted()
{
	if (HasPostProcess == false) {
		State.Initialize();
		StandbyCommand();
	}

	theRenderer.SendPaintSignal(UseThread);
	//:WARNING - release CancelCommand
	LockProcess(HasPostProcess = false);
}

//--------------------------------------------------------------------------------------------------

OdGePoint3d UserIO::GetPoint(const CString& prompt, int options, const wchar_t* keyword, TrackerBase* pTracker)
{
	State.Initialize();
	State.Set(prompt, options, keyword, pTracker);
	State.Wait = Uio::EWait::Point;
	State.Steps.Next(State.OSnap, State.Keywords);

	RubberBandTracker rubberBand;
	if (GetBit(options, Uio::eRubberBand)) {
		rubberBand.Params.Set(this);

		rubberBand.BandLine = GetBit(options, Uio::eBandLine);
		rubberBand.LengthGuide = GetBit(options, Uio::eLengthGuide);
		rubberBand.AngleGuide = GetBit(options, Uio::eAngleGuide);
		rubberBand.SetBasePoint(State.LastPoint);

		theTrackers.Push(&rubberBand);
	}

	//:CHECK
	RubberRectTracker rubberRect;
	if (GetBit(options, Uio::eRubberRect)) {
		rubberBand.SetBasePoint(State.LastPoint);
		theTrackers.Push(&rubberRect);
	}

	theTrackers.Initialize(theGsViewPtr);
	//:WARING - for keyboard signal
	theTrackers.SetValue(theCoord.ToEyeToWorld(State.MousePoint));
	theTrackers.Invalidate(State.RendererPtr);

	SendPrompt(prompt, keyword, options);
	LockProcess();

	theTrackers.Terminate(theGsViewPtr);

	switch (Result.Return) {
	case Uio::EReturn::Point:	return Result.Point;
	case Uio::EReturn::Keyword:	throw OdEdKeyword(Result.Keyword.Index, Result.Keyword.Code.GetBuffer());
	case Uio::EReturn::Cancel:	throw OdEdCancel();

	default:
		ASSERT(Result.Return != Uio::EReturn::None);
		throw Result;
	}

	DEBUG_STOP;
	return {};
}



bool UserIO::SetInteger(int value)
{
	Result.Return = Uio::EReturn::Integer;
	Result.Integer = value;
	theTrackers.SetValue(value);

	LockProcess(false);
	return true;
}



bool UserIO::SetPoint(OdGePoint3d value, bool unlock)
{
	if (State.Wait == Uio::EWait::None) {
		return false;
	}

	OdGePoint3d start = State.LastPoint;
	if (OdNonZero((start - value).normalizeGetLength())) {
		if (GetBit(State.Filter.Flag, Uio::EFilter::Angle) && GetBit(State.Filter.Flag, Uio::EFilter::Length)) {
			ASSERT(State.Filter.Length > 0);

			value = start + (OdGeVector3d::kXAxis * State.Filter.Length);
			value.rotateBy(State.Filter.Angle, OdGeVector3d::kZAxis, start);
			//:WARNING
			unlock = true;
		}
		else if (GetBit(State.Filter.Flag, Uio::EFilter::Angle)) {
			ASSERT(OdNonZero(State.Filter.Angle));

			double length = (value - start).normalizeGetLength();
			value = start + (OdGeVector3d::kXAxis * length);
			value.rotateBy(State.Filter.Angle, OdGeVector3d::kZAxis, start);
		}
		else if (GetBit(State.Filter.Flag, Uio::EFilter::Length)) {
			value = start + (value - start).normalize() * State.Filter.Length;
		}

		if (GetBit(State.Filter.Flag, Uio::EFilter::X) && GetBit(State.Filter.Flag, Uio::EFilter::Y)) {
			value.x = State.Filter.X;
			value.y = State.Filter.Y;
			//:WARNING
			unlock = true;
		}
		else if (GetBit(State.Filter.Flag, Uio::EFilter::X)) {
			value.x = State.Filter.X;
		}
		else if (GetBit(State.Filter.Flag, Uio::EFilter::Y)) {
			value.y = State.Filter.Y;
		}
	}

	theTrackers.SetValue(value);
	theTrackers.Invalidate(State.RendererPtr);

	if (unlock) {
		State.LastPoint = value;
		Result.Return = Uio::EReturn::Point;
		Result.Point = value;

		LockProcess(false);
	}

	return true;
}



bool UserIO::SetReal(double value)
{
	Result.Return = Uio::EReturn::Real;
	Result.Real = value;
	theTrackers.SetValue(value);

	LockProcess(false);
	return true;
}



bool UserIO::SetString(const CString& value)
{
	Result.Return = Uio::EReturn::Real;
	Result.String = value;
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



bool UserIO::SendPrompt(const CString& prompt, const CString& keyword, int options)
{
	theDelivery.PutPrompt(prompt, keyword, options);
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
	return false;
}

//--------------------------------------------------------------------------------------------------

bool UserIO::ParseCommand(CString value)
{
	CommandInfo* pInfo = TheCommandStack.Find(value.MakeUpper());
	if (pInfo == nullptr) {
		return SendError(Uio::ErrorInvalidCommand);
	}

	CancelCommand(true);

	State.Command = pInfo->GetName();
	State.Steps.Initialize(pInfo->Step);
	SendCommand(State.Command);

	if (TheCommandStack.Execute(pInfo, &theRenderer) == false) {
		DEBUG_STOP;
	}

	return true;
}



bool UserIO::ParseFilter(CString value)
{
	static wchar_t* ANGLE = L"<%lf";
	static wchar_t* LENGTH = L"@%lf<%lf";
	static wchar_t* COORDX = L".x%lf";
	static wchar_t* COORDY = L".y%lf";

	value.MakeLower();

	WCHAR first = value[0];
	double result = 0.0;
	bool parsed = false;

	if (value == L".x") {
		BitOn(State.Filter.Flag, Uio::EFilter::WaitX);
		//:TODO
		return SendEcho(L".x of");
	}
	else if (value == L".y") {
		BitOn(State.Filter.Flag, Uio::EFilter::WaitY);
		//:TODO
		return SendEcho(L".y of");
	}
	else if (swscanf_s(value, ANGLE, &result) == 1) {
		BitOn(State.Filter.Flag, Uio::EFilter::Angle);
		State.Filter.Angle = Uio::ToRadian(result);
		parsed = true;
	}
	else if (swscanf_s(value, LENGTH, &result) == 1) {
		if (OdNonZero(result)) {
			BitOn(State.Filter.Flag, Uio::EFilter::Length);
			State.Filter.Length = result;
			parsed = true;
		}
	}
	else if (swscanf_s(value, COORDX, &result) == 1) {
		BitOn(State.Filter.Flag, Uio::EFilter::X);
		State.Filter.X = result;
		parsed = true;
	}
	else if (swscanf_s(value, COORDY, &result) == 1) {
		BitOn(State.Filter.Flag, Uio::EFilter::Y);
		State.Filter.Y = result;
		parsed = true;
	}

	if (parsed) {
		SetPoint(theCoord.ToEyeToWorld(State.MousePoint), false);
		return true;
	}

	SendError(Uio::ErrorInvalidValue);
	return false;
}



bool UserIO::ParseKeyword(CString value)
{
	UioKeyword found;
	// Shift PRE_KEYWORD
	int index = State.Keywords.Find(value.Mid(1), found);

	if (index > -1) {
		Result.Return = Uio::EReturn::Keyword;
		Result.Keyword = found;

		LockProcess(false);
	}
	else {
		SendError(Uio::ErrorInvalidKeyword);
	}

	return true;
}



bool UserIO::ParseOSnap(CString value)
{
	// Shift PRE_OSNAP
	CString snap = CString(value[1]).MakeUpper() + value.Mid(2);

#define SetOSnap(x) else if (snap == L#x) { \
	State.OSnap = Uio::EOSnap::x; \
	return true; \
}

	if (value.IsEmpty()) {
		State.OSnap = Uio::EOSnap::None;
	}
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



bool UserIO::ParseOtherInput(CString value)
{
	if (GetBit(State.Options, Uio::eAllowInteger)) return ParseInteger(value);
	if (GetBit(State.Options, Uio::eAllowReal)) return ParseReal(value);

	return false;
}



bool UserIO::ParsePoint(CString value)
{
	static wchar_t* CARTESIAN = L"%lf,%lf";
	static wchar_t* POLAR = L"%lf<%lf";

	wchar_t* pValue = (wchar_t*)value.GetString();
	double v1, v2;
	bool relative = false;

	if (pValue[0] == PRE_RELATIVE) {
		relative = true;
		pValue++;
	}

	if (swscanf_s(pValue, CARTESIAN, &v1, &v2) == 2) {
		double x = v1;
		double y = v2;

		if (relative) {
			return SetPoint(State.LastPoint + OdGeVector3d(x, y, 0), true);
		}
		else {
			return SetPoint(OdGePoint3d(x, y, 0), true);
		}
	}
	else if (swscanf_s(pValue, POLAR, &v1, &v2) == 2) {
		double length = v1;
		double angle = Uio::ToRadian(v2);

		OdGePoint3d point(length, 0, 0);
		point += State.LastPoint.asVector();
		point.rotateBy(angle, OdGeVector3d::kZAxis, relative ? State.LastPoint : OdGePoint3d::kOrigin);

		return SetPoint(point, true);
	}
	else if (State.Options > 0) {
		return ParseOtherInput(value);
	}
	else {
		return SendError(Uio::ErrorInvalidPoint);
	}

	return false;
}



bool UserIO::ParseReal(CString value)
{
	double result = 0.0;

	if (swscanf_s(value, L"%lf", &result) != 1) {
		return SendError(Uio::ErrorInvalidValue);
	}

	if (GetBit(State.Options, Uio::eNoZero) && OdZero(result)) {
		return SendError(Uio::ErrorZeroValue);
	}
	if (GetBit(State.Options, Uio::eNoNegative) && result < 0) {
		return SendError(Uio::ErrorNegativeValue);
	}

	return SetReal(result);
}

#undef theRenderer
#undef theGsView
#undef theDelivery
#undef theCoord
#undef theKeywords
#undef theTrackers

#undef ThrowCancel
