#pragma once

#include <Json.h>

#include <map>
#include <chrono>

#include "3DFSignal.h"

class HDB;

OPEN_3DF_SIGNAL_NAMESPACE

class ApplicationManager
{
public:
	friend class Manager;

	void ExecuteSignal(Json::Object & cInObject);

protected:
	void InitInstance();
	void ExitInstance();

private: 
	Manager * m_pcWrapper = nullptr; 
	Manager & Wrapper() { return *m_pcWrapper; }

	CString GetExecuteDirectory();
};

CLOSE_3DF_SIGNAL_NAMESPACE