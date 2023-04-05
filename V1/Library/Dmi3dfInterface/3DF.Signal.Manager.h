#pragma once

#include <Json.h>

#include <map>
#include <chrono>

#include "3DF.Signal.h"

#include "3DF.Signal.Interface.h"
#include "3DF.Signal.ApplicationManager.h"
#include "3DF.Signal.ViewManager.h"

OPEN_3DF_NAMESPACE

class Manager
{
public:
	Manager(Interface * pc3dfInterface);
	~Manager();

	friend class ApplicationManager;
	friend class ViewManager;

	void ExecuteSignal(Json::Object & cInObject);

	Interface * GetInterface() { return m_pc3dfInterface; }

protected:
	ViewManager m_cViewManager;
	ApplicationManager m_cApplicationManager;

private:
	Interface * m_pc3dfInterface = nullptr;

	// Pointer to the HOOPS/MVO HDB object associated with this instance of the application
	HDB * m_pcHoopsDB = nullptr;

	std::map<int, TDF::Canvas *> m_mpcCanvas;
};

CLOSE_3DF_NAMESPACE

extern TDF::Manager theManager;