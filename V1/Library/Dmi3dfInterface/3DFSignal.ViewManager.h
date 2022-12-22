#pragma once

#include <Json.h>

#include <map>
#include <chrono>

#include "3DF/3DF.h"

#include "3DFSignal.h"

OPEN_3DF_SIGNAL_NAMESPACE

class ViewManager
{
public:
	friend class Manager;

	void ExecuteSignal(Json::Object & cInObject);

protected:
	void Initialize(int nViewId, Json::Object & cInObject);
	void Paint(int nViewId, Json::Object & cInObject);

	void SaveHsfFile(CString strFilePathName, _3DF::View * pcHoopsView);

	//== Mouse 관련 함수 =============================================================================
protected:
	bool ExecuteMouseSignal(int nViewId, int nAction, Json::Object & cInObject);

	bool LButtonUp(_3DF::View * pcView, int nFlags, int x, int y);
	bool LButtonDown(_3DF::View * pcView, int nFlags, int x, int y);

	bool MButtonUp(_3DF::View * pcView, int nFlags, int x, int y);
	bool MButtonDown(_3DF::View * pcView, int nFlags, int x, int y);

	bool RButtonUp(_3DF::View * pcView, int nFlags, int x, int y);
	bool RButtonDown(_3DF::View * pcView, int nFlags, int x, int y);

	bool MouseMove(_3DF::View * pcView, int nFlags, int x, int y);

	bool MouseWheel(_3DF::View * pcView, int nFlags, int zDelta, int x, int y, Json::Object & cInObject);

private: 
	Manager * m_pcWrapper = nullptr; 
	Manager & Wrapper() { return *m_pcWrapper; }

	_3DF::Model * m_pcHoopsModel = nullptr;
};

CLOSE_3DF_SIGNAL_NAMESPACE