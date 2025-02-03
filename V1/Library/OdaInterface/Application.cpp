#include "stdafx.h"
#include "Application.h"
#include "Renderer.h"
#include "SignalArgs.h"
#include "UserIO.h"

//**************************************************************************************************

namespace
{
	int Language = 0;
}

HMODULE Application::Instance = nullptr;
// single instance
Application TheApp;

//--------------------------------------------------------------------------------------------------

Signal::Delivery& Application::GetDelivery(int viewId)
{
	TheDelivery.ViewId = viewId;
	return TheDelivery;
}



void Application::SetLanguage(int value)
{
	Language = value;
}



CString Application::Local(const CString& value)
{
	WStringArray buffer;
	if (WStr::Split(value, L'|', buffer)) {
		return buffer[Language];
	}
	else {
		DEBUG_STOP;
		return value;
	}
}



OdDbUndoControllerPtr Application::newUndoController()
{
	return OdRxObjectImpl<ExUndoController>::createObject();
}



Application::Application()
{
}



Application::~Application()
{
	//ASSERT(Renderers.size() == 0);
}



bool Application::Initialize()
{
	try {
		// ExSystemServices
		::odInitialize(this);
	}
	catch (...) {
		RETURN_FALSE;
	}

	return true;
}



bool Application::Terminate()
{
	Clear();

	::odUninitialize();

	return true;
}



void Application::AddNew(int id, Renderer* pRenderer)
{
	// WARNING - C++20
	//ASSERT(Renderers.contains(id) == false);
	// C++17
	ASSERT(Renderers.size() == 0 || Renderers.find(id) == Renderers.end());
	Renderers[id] = pRenderer;
}



void Application::Clear()
{
	for (auto thread : Renderers) {
		thread.second->Terminate();
		REMOVE_POINTER(thread.second);
	}

	Renderers.clear();
}



Renderer* Application::Find(int id)
{
	//if (Renderers.contains(id)) {
	if (Renderers.find(id) != Renderers.end()) {
		return Renderers[id];
	}
	else {
		return nullptr;
	}
}



void Application::Remove(int id)
{
	auto thread = Renderers[id];

	Renderers.erase(id);

	thread->Terminate();
	REMOVE_POINTER(thread);
}



bool Application::ReceiveSignalFromUi(const wchar_t* pContent)
{
	Json::Object root;
	// WARNING - for pointer shift
	wchar_t* pContentPos = (wchar_t*)pContent;

	if (Json::Reader::ReadObject(pContentPos, root) == false) {
		RETURN_FALSE;
	}

	int signalTarget = -1;
	int signalAction = -1;
	SignalArgs::Base* signal = SignalArgs::Base::CreateInstance(root, signalTarget, signalAction);

	if (signal == nullptr) {
		if ((Signal::Target)signalTarget == Signal::Target::Application) {
			Signal::Application::Action action = (Signal::Application::Action)signalAction;

			switch (action) {
				case Signal::Application::Action::OnInitInstance:
					Initialize();
					return true;

				case Signal::Application::Action::OnExitInstance:
					Terminate();
					return true;

				default:
					return false;
			}
		}
		else {
			return false;
		}
	}

	Renderer* pRenderer = nullptr;
	if ((pRenderer = Find(signal->ViewId)) != nullptr) {
	}
	else {
		pRenderer = new Renderer();
		AddNew(signal->ViewId, pRenderer);
	}

	pRenderer->PostSignal(signal);

	return true;
}



void Application::OnFileClose(int id)
{
}
