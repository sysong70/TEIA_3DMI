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

	void SaveHsfFile(CString strFilePathName, TDF::Canvas * pcHoopsView);
	
	void LoadPointCloudFile(CString strFilePathName, TDF::Canvas * pcHoopsView);

	//== Command 관련 함수 ===========================================================================
	void CancelCommands(int nViewId);

	//== Mouse 관련 함수 =============================================================================
protected:
	bool ExecuteMouseSignal(int nViewId, int nAction, Json::Object & cInObject);

	bool LButtonUp(TDF::Canvas * pcView, int nFlags, int x, int y);
	bool LButtonDown(TDF::Canvas * pcView, int nFlags, int x, int y);

	bool MButtonUp(TDF::Canvas * pcView, int nFlags, int x, int y);
	bool MButtonDown(TDF::Canvas * pcView, int nFlags, int x, int y);

	bool RButtonUp(TDF::Canvas * pcView, int nFlags, int x, int y);
	bool RButtonDown(TDF::Canvas * pcView, int nFlags, int x, int y);

	bool MouseMove(TDF::Canvas * pcView, int nFlags, int x, int y);

	bool MouseWheel(TDF::Canvas * pcView, int nFlags, int zDelta, int x, int y, Json::Object & cInObject);

	//:Ken - 20230607
	bool ExecuteKeyboardSignal(int nViewId, int nAction, Json::Object& cInObject);

private: 
	Manager * m_pcWrapper = nullptr; 
	Manager & Wrapper() { return *m_pcWrapper; }

	TDF::Model * m_pcHoopsModel = nullptr;
};

CLOSE_3DF_NAMESPACE