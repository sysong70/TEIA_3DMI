#pragma once

#include <Json.h>

#include <map>
#include <chrono>

#include "3DFSignal.h"

#include "3DFSignal.ApplicationManager.h"
#include "3DFSignal.ViewManager.h"

OPEN_3DF_SIGNAL_NAMESPACE

class Manager
{
public:
	Manager(Interface * pc3dfInterface);
	~Manager();

	friend class ApplicationManager;
	friend class ViewManager;

	void ExecuteSignal(Json::Object & cInObject);

protected:
	ViewManager m_cViewManager;
	ApplicationManager m_cApplicationManager;

private:
	Interface * m_pc3dfInterface = nullptr;

	// Pointer to the HOOPS/MVO HDB object associated with this instance of the application
	HDB * m_pcHoopsDB = nullptr;

	std::map<int, _3DF::View *> m_mpcHoopsView;
};

CLOSE_3DF_SIGNAL_NAMESPACE