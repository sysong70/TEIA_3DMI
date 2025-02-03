#pragma once

#include "Connector.h"
#include "Signal.h"

#include "OdToolKit.h"
#include "ExHostAppServices.h"
#include "ExSystemServices.h"
#include "ExUndoController.h"

class Renderer;

//--------------------------------------------------------------------------------------------------

class Application
	: public ExHostAppServices
	, public ExSystemServices
{
	Signal::Delivery TheDelivery;

public:

	// WARNING - defined for future enhancements, from DllMain
	static HMODULE Instance;

	std::map<int, Renderer*> Renderers;

	Signal::Delivery& GetDelivery(int viewId);

public: // Tools

	void SetLanguage(int value);

	CString Local(const CString& value);

protected:

	using OdRxObject::operator new;

	using OdRxObject::operator delete;

	void addRef() override {}

	void release() override {}

public: // OdDbHostAppServices

	OdDbUndoControllerPtr newUndoController() override;

public:

	Application();

	virtual ~Application();

	bool Initialize();

	bool Terminate();

public:

	void AddNew(int id, Renderer* pRenderer);

	void Clear();

	Renderer* Find(int id);

	void Remove(int id);

public:

	bool ReceiveSignalFromUi(const wchar_t* pContent);

public:

	void OnFileClose(int id);
};



extern Application TheApp;