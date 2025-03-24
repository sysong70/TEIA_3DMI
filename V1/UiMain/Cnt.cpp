#include "stdafx.h"

#include "Cnt.h"
#include "Wnd.Application.h"
#include "Wnd.MainFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

using InitializeFunc = bool (*)(void);
using TerminateFunc = bool (*)(void);

//**************************************************************************************************

#pragma region Cnt2d Namespace

class Broker2d
{
public:

	bool IsValid = false;
	HINSTANCE hInstance = nullptr;
	DWORD ErrorCode = -1;

	SendSignalFunc SendSignal = nullptr;
	AssignSendSignalFunc SetReceiver = nullptr;
	GetCoordinateFunc GetCoordinate = nullptr;
	SetIntegerFunc SetLanguage = nullptr;

	SgnDelivery2d TheDelivery;

public:

	Broker2d()
	{
	}

	~Broker2d()
	{
		if (hInstance != nullptr) {
			::FreeLibrary(hInstance);
		}
	}

	bool Load(const CString& filePath)
	{
		hInstance = ::LoadLibrary(filePath);
		if (hInstance == nullptr) {
			ErrorCode = ::GetLastError();
			return false;
		}

		SendSignal = (SendSignalFunc)GetProcAddress(hInstance, "ReceiveSignal");
		SetReceiver = (AssignSendSignalFunc)GetProcAddress(hInstance, "AssignSendSignalFunc");
		GetCoordinate = (GetCoordinateFunc)GetProcAddress(hInstance, "GetCoordinate");
		SetLanguage = (SetIntegerFunc)GetProcAddress(hInstance, "SetLanguage");

		if (IsValid = (SendSignal && SetReceiver)) {
			DEBUG_VALID(GetCoordinate);
			DEBUG_VALID(SetLanguage);

			SetReceiver(Cnt2d::ReceiveSignal);
			TheDelivery.SetSender(SendSignal);

			return true;
		}

		return false;
	}
};

Broker2d theBroker2d;

//**************************************************************************************************

bool Cnt2d::Initialize()
{
	return theBroker2d.Load(L"OdaInterface.dll");
}



SendSignalFunc Cnt2d::GetSender()
{
	DEBUG_VALID(theBroker2d.SendSignal);
	return theBroker2d.SendSignal;
}



SgnDelivery2d& Cnt2d::GetInstance()
{
	return theBroker2d.TheDelivery;
}

SgnDelivery2d& Cnt2d::GetInstance(int viewId)
{
	theBroker2d.TheDelivery.ViewId = viewId;
	return theBroker2d.TheDelivery;
}



void Cnt2d::ReceiveSignal(const wchar_t* content)
{
	// WARNING - delete this pointer after use
	Json::Object* pData = new Json::Object();
	Json::Object& data = *pData;
	Json::Helper::Load(content, data);

	Sgn::ETarget target = (Sgn::ETarget)data.GetInteger(SKW_TARGET, -1);

	switch (target) {
	case Sgn::ETarget::Application:
	case Sgn::ETarget::Command:
	case Sgn::ETarget::MainFrame:
	case Sgn::ETarget::DebugTracer:
	case Sgn::ETarget::ProgressBar:
	case Sgn::ETarget::StatusBar:
	case Sgn::ETarget::TaskBar:
	case Sgn::ETarget::UserIO:
	case Sgn::ETarget::View:
		// CHECK - why not PostMessage
		TheApp.GetMainFrame().SendMessage((UINT)Wnd::EMessage::OnSignal2d, (WPARAM)pData);
		break;

	case Sgn::ETarget::Unknown:
	default:
		REMOVE_POINTER(pData);
		DEBUG_STOP;
	}
}



double* Cnt2d::GetCoordinate(int viewId, int x, int y)
{
	return theBroker2d.GetCoordinate(viewId, x, y);
}



void Cnt2d::SetLanguage(int value)
{
	theBroker2d.SetLanguage(value);
}

#pragma endregion // REGION

//**************************************************************************************************

#pragma region Cnt3d Namespace

class Broker3d
{
public:

	bool IsValid = false;
	HINSTANCE hInstance = nullptr;
	DWORD ErrorCode = -1;

	SendSignalFunc SendSignal = nullptr;
	AssignSendSignalFunc SetReceiver = nullptr;

	Signal::Delivery TheDelivery;

public:

	Broker3d()
	{
	}

	~Broker3d()
	{
		if (hInstance != nullptr) {
			::FreeLibrary(hInstance);
		}
	}

	bool Load(const CString& filePath)
	{
		hInstance = ::LoadLibrary(filePath);
		if (hInstance == nullptr) {
			ErrorCode = ::GetLastError();
			RETURN_FALSE;
		}

		SendSignal = (SendSignalFunc)GetProcAddress(hInstance, "ExecuteCommand");
		SetReceiver = (AssignSendSignalFunc)GetProcAddress(hInstance, "AssignSendSignalFunc");

		if (IsValid = (SendSignal && SetReceiver)) {
			SetReceiver(Cnt3d::ReceiveSignal);
			TheDelivery.SetSender(SendSignal);

			return true;
		}

		RETURN_FALSE;
	}
};

Broker3d theBroker3d;

//**************************************************************************************************

bool Cnt3d::Initialize()
{
	return theBroker3d.Load(L"3DMISessionInterface.dll");
}



SendSignalFunc Cnt3d::GetSender()
{
	return theBroker3d.SendSignal;
}



Signal::Delivery& Cnt3d::GetInstance()
{
	return theBroker3d.TheDelivery;
}

Signal::Delivery& Cnt3d::GetInstance(int viewId)
{
	theBroker3d.TheDelivery.ViewId = viewId;
	return theBroker3d.TheDelivery;
}



void Cnt3d::ReceiveSignal(const wchar_t* content)
{
	// WARNING - delete this pointer after use
	Json::Object* pData = new Json::Object();
	Json::Object& data = *pData;
	if (Json::Helper::Load(content, data) == false) {
		REMOVE_POINTER(pData);
		DEBUG_RETURN;
	}

	Signal::Target target = (Signal::Target)data.GetInteger(SKW_TARGET, -1);

	switch (target) {
	case Signal::Target::Application:
	case Signal::Target::MainFrame:
	case Signal::Target::StatusBar:
	case Signal::Target::View:
	case Signal::Target::ModelPanel:
	case Signal::Target::Progress:
	case Signal::Target::TaskBar:
	case Signal::Target::Command:
		TheApp.GetMainFrame().SendMessage((UINT)Wnd::EMessage::OnSignal3d, (WPARAM)pData);
		break;

	case::Signal::Target::Unknown:
	default:
		REMOVE_POINTER(pData);
		DEBUG_STOP;
	}
}

#pragma endregion // REGION
