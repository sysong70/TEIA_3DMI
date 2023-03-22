#include "stdafx.h"
#include "Application.h"
#include "Json.h"
#include "Renderer.h"
#include "SignalArgs.h"

//--------------------------------------------------------------------------------------------------

HMODULE Application::Instance = nullptr;
// single instance
Application TheApp;

//--------------------------------------------------------------------------------------------------

Application::Application()
	: SendSignalToUi(nullptr)
{
}



Application::~Application()
{
	ASSERT(Renderers.size() == 0);
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



void Application::AddNew(int id, Renderer* renderer)
{
	ASSERT(Renderers.contains(id) == false);
	Renderers[id] = renderer;
}



void Application::Clear()
{
	for (auto thread : Renderers) {
		REMOVE_POINTER(thread.second);
	}

	Renderers.clear();
}



Renderer* Application::Find(int id)
{
	if (Renderers.contains(id)) {
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
	REMOVE_POINTER(thread);
}



bool Application::ReceiveSignalFromUi(const wchar_t* content)
{
	Json::Object root;
	//:WARNING - for pointer moving
	wchar_t* pContent = (wchar_t*)content;
	//TRACE(L"%s\n", pContent);

	if (Json::Reader::ReadObject(pContent, root) == false) {
		RETURN_FALSE;
	}

	SignalArgs::Base* signal = SignalArgs::Base::CreateInstance(root);
	if (signal == nullptr) {
		return false;
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
