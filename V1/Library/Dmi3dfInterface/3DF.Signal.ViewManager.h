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

	void SaveHsfFile(CString strFilePathName, H3DF::Canvas * pcHoopsView);
	
	void LoadPointCloudFile(CString strFilePathName, H3DF::Canvas * pcHoopsView);

	//== Command 관련 함수 ===========================================================================
	void CancelCommands(int nViewId);

	//== Mouse 관련 함수 =============================================================================
protected:
	bool ExecuteMouseSignal(int nViewId, int nAction, Json::Object & cInObject);

	bool LButtonUp(H3DF::Canvas * pcView, int nFlags, int x, int y);
	bool LButtonDown(H3DF::Canvas * pcView, int nFlags, int x, int y);

	bool MButtonUp(H3DF::Canvas * pcView, int nFlags, int x, int y);
	bool MButtonDown(H3DF::Canvas * pcView, int nFlags, int x, int y);

	bool RButtonUp(H3DF::Canvas * pcView, int nFlags, int x, int y);
	bool RButtonDown(H3DF::Canvas * pcView, int nFlags, int x, int y);

	bool MouseMove(H3DF::Canvas * pcView, int nFlags, int x, int y);

	bool MouseWheel(H3DF::Canvas * pcView, int nFlags, int zDelta, int x, int y, Json::Object & cInObject);

	//:Ken - 20230607
	bool ExecuteKeyboardSignal(int nViewId, int nAction, Json::Object& cInObject);

private: 
	Manager * m_pcWrapper = nullptr; 
	Manager & Wrapper() { return *m_pcWrapper; }

	H3DF::Model * m_pcHoopsModel = nullptr;
};

CLOSE_3DF_NAMESPACE