#include "stdafx.h"
#include "UserIO.h"
#include "Application.h"
#include "Commands.h"
#include "CommandStack.h"
#include "Renderer.h"

#include "Ed/EdUserIO.h"

//**************************************************************************************************

void UserIO::KeywordIndexer::Initialize(CString value)
{
	Clear();

	value.MakeLower();

	WStringArray byLanguage;
	WStr::Split(value, L'|', byLanguage);

	WStringArray byData;
	WStr::Split(byLanguage[0], L'/', byData);

	for (auto data : byData) {
		CString code = WStr::Get(data, 0, L'(', false, true);

		wchar_t key = 0;
		swscanf_s(data, L"(%c)", &key, 1);
		ASSERT(key != 0);

		Indexer.push_back({ code, key });
	}
}



int UserIO::KeywordIndexer::Find(const CString& value, Data& found)
{
	int index = 0;
	for (auto data : Indexer) {
		if (value == data.Code || (value.GetLength() == 1 && value[0] == data.Key)) {
			found = data;
			return index;
		}

		index++;
	}

	return -1;
}



void UserIO::KeywordIndexer::Check(CString value)
{
	value.MakeLower();

	Data found;
	int index = Find(value, found);
	if (index > -1) {
		throw OdEdKeyword(index, OdString(found.Code));
	}
}



void UserIO::KeywordIndexer::Clear()
{
	Indexer.clear();
}

//**************************************************************************************************

void UserIO::Data::Clear()
{
	Prompt.Empty();
	Keywords.Clear();

	Mode = EMode::None;
	OSnap = EOverride::None;
	Return = EReturn::None;

	Real = 0.0;
	Integer = 0;
	Point = OdGePoint3d::kOrigin;
	String.Empty();

	Keyword.Empty();
	KeywordIndex = -1;

	MousePoint = CPoint(0, 0);
}

//**************************************************************************************************

#define theRenderer		(*m_data.pRenderer)
#define theGsView		m_data.pRenderer->GetGsView()
#define theDelivery		m_data.pRenderer->GetDelivery()
#define theCoord		m_data.pRenderer->GetCoordConvertor()
#define theKeywords		m_data.Keywords
#define theTrackers		m_data.trackers

#define ThrowKeyword	throw OdEdKeyword(m_data.KeywordIndex, m_data.Keyword.GetBuffer());
#define ThrowCancel		throw OdEdCancel()

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

	// TEST
	if (DRAW_2D_CMD_Line) {
		return ParseCommand(L"line");
	}
	else {
		return false;
	}

	return true;
}



bool UserIO::OnContextCommand(SignalArgs::Base* pSignal)
{
	return false;
}



bool UserIO::OnKeyDown(SignalArgs::Base* pSignal)
{
	SignalArgs::Keyboard& signal = *(SignalArgs::Keyboard*)pSignal;

	switch (signal.Char) {
	case VK_ESCAPE: return Cancel();
	case VK_RETURN:
		break;

	default:
		break;
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
	m_data.MousePoint = CPoint(signal.X, signal.Y);

	return true;
}



bool UserIO::OnMButtonUp(SignalArgs::Base* pSignal)
{
	return false;
}



bool UserIO::OnRButtonDown(SignalArgs::Base* pSignal)
{
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
		theRenderer.Dolly(point.x - m_data.MousePoint.x, point.y - m_data.MousePoint.y);
		m_data.MousePoint = point;
	}
	else {
		OdGePoint3d gePoint = theCoord.ToEyeToWorld(point);
		theTrackers.SetValue(gePoint);
	}

	// WARNING - thread operation failed
	//theRenderer.RedrawWindow();
	theRenderer.PostPaintSignal();

	return true;
}



bool UserIO::OnInput(SignalArgs::Base* pSignal)
{
	SignalArgs::Text& signal = *(SignalArgs::Text*)pSignal;
	CString value = signal.Buffer;

	value.Trim(L" \t\r\n");
	value.MakeLower();

	if (value == L"^c") {
		return Cancel();
	}
	else if (value[0] == L'_') {
		return ParseKeyword(value);
	}
	else if (value[0] == L'.') {
		return ParseOverride(value);
	}

	switch (m_data.Mode) {
		case EMode::Integer:	return ParseInteger(value);
		case EMode::Distance:	return ParseReal(value);
		case EMode::Point:		return ParsePoint(value);
		case EMode::String:		return ParseString(value);

		default:
			return ParseCommand(value);
	}

	RETURN_FALSE;
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
	return m_data.Mode != EMode::None;
}



void UserIO::SetRenderer(Renderer* pRenderer)
{
	m_data.pRenderer = pRenderer;
}



void UserIO::LockProcess(bool value)
{
	if (value) {
		// CHECK
		theTrackers.SetGsView(theGsView);

		std::unique_lock<std::mutex> lock(m_waitMutex);
		m_waitCondition.wait(lock);
	}
	else {
		// CHECK
		theTrackers.Clear(theGsView);

		m_waitCondition.notify_one();
	}
}

//--------------------------------------------------------------------------------------------------

double UserIO::GetDistance(const CString& prompt, const CString& keyword, TrackerBase* pTracker)
{
	m_data.Clear();

	m_data.Mode = EMode::Distance;
	m_data.Return = EReturn::None;
	m_data.Prompt = prompt;
	m_data.Keyword = keyword;
	theTrackers.Push(pTracker);

	LockProcess();

	if (m_data.Return == EReturn::Real) {
		return m_data.Real;
	}
	else if (m_data.Return == EReturn::Keyword) {
		ThrowKeyword;
	}
	else if (m_data.Return == EReturn::Cancel) {
		ThrowCancel;
	}

	DEBUG_STOP;
	return {};
}



OdGePoint3d UserIO::GetPoint(const CString& prompt, const CString& keyword, TrackerBase* pTracker)
{
	m_data.Clear();

	m_data.Mode = EMode::Point;
	m_data.Return = EReturn::None;
	m_data.Prompt = prompt;
	m_data.Keyword = keyword;
	theTrackers.Push(pTracker);

	LockProcess();

	if (m_data.Return == EReturn::Point) {
		return m_data.Point;
	}
	else if (m_data.Return == EReturn::Keyword) {
		ThrowKeyword;
	}
	else if (m_data.Return == EReturn::Cancel) {
		ThrowCancel;
	}

	DEBUG_STOP;
	return {};
}



bool UserIO::SetDistance(double value)
{
	m_data.Return = EReturn::Real;
	m_data.Real = value;
	theTrackers.SetValue(value);

	LockProcess(false);
	return true;
}



bool UserIO::SetPoint(const OdGePoint3d& value)
{
	m_data.Return = EReturn::Point;
	m_data.LastPoint = value;
	m_data.Point = value;
	theTrackers.SetValue(value);

	LockProcess(false);
	return true;
}

//--------------------------------------------------------------------------------------------------

bool UserIO::Cancel()
{
	if (m_data.Command.IsEmpty() == false) {
		m_data.Command.Empty();
		m_data.Return = EReturn::Cancel;

		LockProcess(false);
	}

	return true;
}



bool UserIO::ParseCommand(const CString& value)
{
	CommandBase* pCommand = TheCommandStack.Find(value);
	if (pCommand != nullptr) {
		if (TheCommandStack.Execute(pCommand, &theRenderer)) {
			m_data.Command = value;
			return true;
		}
		else {
			InputError(Io::ErrorInvalidCommand);
		}
	}

	return false;
}



bool UserIO::ParseInteger(const CString& value)
{
	RETURN_FALSE;
}



bool UserIO::ParseKeyword(const CString& value)
{
	KeywordIndexer::Data found;
	int index = m_data.Keywords.Find(value, found);

	if (index > -1) {
		m_data.Return = EReturn::Keyword;
		m_data.KeywordIndex = index;
		m_data.Keyword = found.Code;

		LockProcess(false);
	}
	else {
		InputError(Io::ErrorInvalidKeyword);
	}

	return true;
}



bool UserIO::ParseOverride(const CString& value)
{
#define SetOSnap(x) else if (snap == L#x) m_data.OSnap = EOverride::x
	
	CString snap = CString(value[1]).MakeLower() + value.Mid(2);

	if (value == L"") m_data.OSnap = EOverride::None;
	SetOSnap(X);
	SetOSnap(Y);
	SetOSnap(Point);
	SetOSnap(End);
	SetOSnap(Mid);
	SetOSnap(Intersection);
	SetOSnap(Perpendicular);
	SetOSnap(Center);
	SetOSnap(Quadrant);
	SetOSnap(Near);

	return true;

#undef SetOSnap
}



bool UserIO::ParsePoint(const CString& value)
{
	const wchar_t CARTESIAN[] = L"%lf,%lf";
	const wchar_t POLAR[] = L"%lf<%lf";

	wchar_t* pValue = (wchar_t*)value.GetString();
	double v1, v2;
	bool relative = false;

	if (pValue[0] == L'@') {
		pValue++;
		relative = true;
	}

	if (swscanf_s(pValue, CARTESIAN, &v1, &v2) == 2) {
		if (relative) {
			return SetPoint(m_data.LastPoint + OdGeVector3d(v1, v2, 0));
		}
		else {
			return SetPoint({ v1, v2, 0 });
		}
	}
	else if (swscanf_s(pValue, POLAR, &v1, &v2) == 2) {
		OdGePoint3d point(v1, 0, 0);
		point.rotateBy(v2, OdGeVector3d::kZAxis, relative ? m_data.LastPoint : OdGePoint3d::kOrigin);

		return SetPoint(point);
	}
	else if (ParseKeyword(value)) {
		return true;
	}
	else {
		return InputError(Io::ErrorInvalidPoint);
	}

	return false;
}



bool UserIO::ParseReal(const CString& value)
{
	double result;

	if (swscanf_s(value, L"%lf", &result) != 1) {
		return InputError(Io::ErrorInvalidDistance);
	}

	if (m_data.Mode == EMode::Distance && result < 0.0) {
		return InputError(Io::ErrorNotPositive);
	}

	return SetDistance(result);
}



bool UserIO::ParseString(const CString& value)
{
	RETURN_FALSE;
}

//--------------------------------------------------------------------------------------------------

bool UserIO::InputError(const CString& value)
{
	// TODO
	//theDelivery.userIO.InputError(value);
	return true;
}



bool UserIO::InputEcho(const CString& value)
{
	// TODO
	//theDelivery.userIO.InputEcho(value);
	return true;
}

#undef theRenderer
#undef theGsView
#undef theDelivery
#undef theCoord
#undef theKeywords
#undef theTrackers

#undef ThrowKeyword
#undef ThrowCancel
