#pragma once

#include "Connector.h"

#include "OdToolKit.h"
#include "ExHostAppServices.h"
#include "ExSystemServices.h"

#include <string>
#include <map>

class Renderer;



class Application
	: public ExHostAppServices
	, public ExSystemServices
{
public:

	std::map<int, Renderer*> Renderers;

protected:

	using OdRxObject::operator new;

	using OdRxObject::operator delete;

	void addRef() override {}

	void release() override {}

public:

	Application();

	virtual ~Application();

	bool Initialize();

	bool Terminate();

public:

	void AddNew(int id, Renderer* thread);

	void Clear();

	Renderer* Find(int id);

	void Remove(int id);

public:

	bool ReceiveSignalFromUi(const wchar_t* content);

	SendSignalFunc SendSignalToUi;

public:

	void OnFileClose(int id);

private:

	clock_t on_paint = 0;
};



extern Application TheApp;