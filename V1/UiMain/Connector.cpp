#include "stdafx.h"
#include "Connector.h"
#include "Window.Application.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

using InitializeFunc = bool (*)(void);
using TerminateFunc = bool (*)(void);

//**************************************************************************************************

#pragma region Connector3d Namespace

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
			SetReceiver(Connector3d::ReceiveSignal);
			TheDelivery.SetSender(SendSignal);

			return true;
		}

		RETURN_FALSE;
	}
};

Broker3d theBroker3d;

//**************************************************************************************************

bool Connector3d::Initialize()
{
	return theBroker3d.Load(L"3DMISessionInterface.dll");
}



SendSignalFunc Connector3d::GetSender()
{
	return theBroker3d.SendSignal;
}



Signal::Delivery& Connector3d::GetInstance()
{
	return theBroker3d.TheDelivery;
}

Signal::Delivery& Connector3d::GetInstance(int viewId)
{
	theBroker3d.TheDelivery.ViewId = viewId;
	return theBroker3d.TheDelivery;
}



void Connector3d::ReceiveSignal(const wchar_t* content)
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
	case Signal::Target::DebugTracer:
		TheApplication.GetMainFrame().SendMessage((UINT)Window::EUserMessage::OnSignal, (WPARAM)pData);
		break;

	case::Signal::Target::Unknown:
	default:
		REMOVE_POINTER(pData);
		DEBUG_STOP;
	}
}

#pragma endregion // REGION

//**************************************************************************************************

#pragma region Connector2d Namespace

class Broker2d
{
public:

	bool IsValid = false;
	HINSTANCE hInstance = nullptr;
	DWORD ErrorCode = -1;

	SendSignalFunc SendSignal = nullptr;
	AssignSendSignalFunc SetReceiver = nullptr;
	// TEST - StatusBar::ShowCoordinate()
	GetCoordinateFunc GetCoordinate = nullptr;
	SetIntegerFunc SetLanguage = nullptr;

	Signal::Delivery TheDelivery;

public:

	Broker2d()
	{
	}
	// TEST
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

			SetReceiver(Connector2d::ReceiveSignal);
			TheDelivery.SetSender(SendSignal);

			return true;
		}

		return false;
	}
};

Broker2d theBroker2d;

//**************************************************************************************************

bool Connector2d::Initialize()
{
	return theBroker2d.Load(L"OdaInterface.dll");
}



SendSignalFunc Connector2d::GetSender()
{
	DEBUG_VALID(theBroker2d.SendSignal);
	return theBroker2d.SendSignal;
}



Signal::Delivery& Connector2d::GetInstance()
{
	return theBroker2d.TheDelivery;
}

Signal::Delivery& Connector2d::GetInstance(int viewId)
{
	theBroker2d.TheDelivery.ViewId = viewId;
	return theBroker2d.TheDelivery;
}



void Connector2d::ReceiveSignal(const wchar_t* content)
{
	// WARNING - delete this pointer after use
	Json::Object* pData = new Json::Object();
	Json::Object& data = *pData;
	Json::Helper::Load(content, data);

	Signal::Target target = (Signal::Target)data.GetInteger(SKW_TARGET, -1);

	switch (target) {
	case Signal::Target::MainFrame:
	case Signal::Target::Progress:
	case Signal::Target::View:
	case Signal::Target::StatusBar:
	case Signal::Target::UserIO:
		// CHECK - why not PostMessage
		TheApplication.GetMainFrame().SendMessage((UINT)Window::EUserMessage::OnSignal, (WPARAM)pData);
		break;

	case::Signal::Target::Unknown:
	default:
		REMOVE_POINTER(pData);
		DEBUG_STOP;
	}
}

double* Connector2d::GetCoordinate(int viewId, int x, int y)
{
	return theBroker2d.GetCoordinate(viewId, x, y);
}



void Connector2d::SetLanguage(int value)
{
	theBroker2d.SetLanguage(value);
}

#pragma endregion // REGION
