#pragma once

#include <Json.h>

#include <map>
#include <chrono>

#include "3DF/3DF.h"

#include "3DF.Signal.h"

OPEN_3DF_NAMESPACE

class ViewManager
{
public:
	friend class Manager;

	void ExecuteSignal(Json::Object & cInObject);

protected:
	void Initialize(int nViewId, Json::Object & cInObject);
	void Destruct(int nViewId);
	void Paint(int nViewId, Json::Object & cInObject);
	void Resize(int nViewId, int x, int y);

	void SaveHsfFile(CString strFilePathName, _3DF::Canvas * pcHoopsView);
	
	void LoadPointCloudFile(CString strFilePathName, _3DF::Canvas * pcHoopsView);

	//== Command 관련 함수 ===========================================================================
	void CancelCommands(int nViewId);

	//== Mouse 관련 함수 =============================================================================
protected:
	bool ExecuteMouseSignal(int nViewId, int nAction, Json::Object & cInObject);

	bool LButtonUp(_3DF::Canvas * pcView, int nFlags, int x, int y);
	bool LButtonDown(_3DF::Canvas * pcView, int nFlags, int x, int y);

	bool MButtonUp(_3DF::Canvas * pcView, int nFlags, int x, int y);
	bool MButtonDown(_3DF::Canvas * pcView, int nFlags, int x, int y);

	bool RButtonUp(_3DF::Canvas * pcView, int nFlags, int x, int y);
	bool RButtonDown(_3DF::Canvas * pcView, int nFlags, int x, int y);

	bool MouseMove(_3DF::Canvas * pcView, int nFlags, int x, int y);

	bool MouseWheel(_3DF::Canvas * pcView, int nFlags, int zDelta, int x, int y, Json::Object & cInObject);

private: 
	Manager * m_pcWrapper = nullptr; 
	Manager & Wrapper() { return *m_pcWrapper; }

	_3DF::Model * m_pcHoopsModel = nullptr;
};

CLOSE_3DF_NAMESPACE