#pragma once

#include <Json.h>

#include <map>
#include <chrono>

#include "3DF.Signal.h"

#include "../Signal/Signal.h"

class HDB;

OPEN_3DF_NAMESPACE

class ApplicationManager
{
public:
	friend class Manager;

	void ExecuteSignal(Json::Object & cInObject);

protected:
	void InitInstance();
	void ExitInstance();

private: 
	// m_pcWrapper 변수는 Manager가 선언되면서 붙여줌.
	Manager * m_pcWrapper = nullptr;
	Manager & Wrapper() { return *m_pcWrapper; }

	CString GetExecuteDirectory();
};

CLOSE_3DF_NAMESPACE