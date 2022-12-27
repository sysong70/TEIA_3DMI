#include "stdafx.h"
#include "Component.StatusBar.h"
#include "Signal.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



#define PRESET PresetStatusBar

namespace PresetStatusBar
{
	enum class EPaneId
	{
		Unknown = WM_USER,
		Message,
		Coordinate,
	};
}



Component::StatusBar::StatusBar()
{
}



Component::StatusBar::~StatusBar()
{
}



bool Component::StatusBar::Initialize(CWnd* pMainFrame)
{
	if (Create(pMainFrame) == FALSE) {
		RETURN_FALSE;
	}

	AddElement(new CBCGPRibbonStatusBarPane((UINT)PRESET::EPaneId::Message, L"", TRUE), L"MessagePane");
	AddExtendedElement(new CBCGPRibbonStatusBarPane((UINT)PRESET::EPaneId::Coordinate, L"", TRUE), L"CoordinatePane");

#ifdef _DEBUG
	GetElement(0)->SetText(L"Message Pane");
	GetExElement(0)->SetText(L"Coordinate Pane");
#endif

	return true;
}



void Component::StatusBar::ReceiveSignal(Json::Object* pData)
{
	Json::Object& data = *pData;

	Signal::StatusBar::Action action = (Signal::StatusBar::Action)data.GetInteger(SKW_ACTION);
	switch (action) {
	case Signal::StatusBar::Action::ShowMessage: ShowMessage(data); break;
	case Signal::StatusBar::Action::ShowCoordinate: ShowCoordinate(data); break;

	default:
		DEBUG_STOP;
		break;
	}

	REMOVE_POINTER(pData);
}



void Component::StatusBar::ShowMessage(Json::Object& data)
{
	CBCGPRibbonStatusBarPane* pPane = (CBCGPRibbonStatusBarPane*)GetElement(0);
	if (pPane == nullptr) {
		DEBUG_RETURN;
	}

	pPane->SetText(data.GetString(SKW_MESSAGE));
}



void Component::StatusBar::ShowCoordinate(Json::Object& data)
{
	CBCGPRibbonStatusBarPane* pPane = (CBCGPRibbonStatusBarPane*)GetExElement(0);
	if (pPane == nullptr) {
		DEBUG_RETURN;
	}

	CString coord;
	if (data.FindValue(SKW_Z) == nullptr) {
		coord.Format(L"%.4f, %.4f", data.GetReal(SKW_X), data.GetReal(SKW_Y));
	}
	else {
		coord.Format(L"%.4f, %.4f, %.4f", data.GetReal(SKW_X), data.GetReal(SKW_Y), data.GetReal(SKW_Z));
	}

	pPane->SetText(coord);
}
