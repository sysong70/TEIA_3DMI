#pragma once

#include "DmiHpsCreator.h"

#include "ComponentTree/DmiHpsComponentTree.h"

#include <Json.h>

#include <map>
#include <chrono>

class MbItem;
class DmiHpsInterface;
class DmiModelHandler;
class DmiHpsView;

class DmiHpsSignalAnalyzer
{
public:
	DmiHpsSignalAnalyzer(DmiHpsInterface * pcHpsInterface);
	~DmiHpsSignalAnalyzer();

	void ExecuteSignal(Json::Object & cInObject);
	void SendSignal(const wchar_t * pchBuffer);

protected:
	void OnInitializeView(int nViewId, Json::Object & cInObject);
	void OnDestructView(int nViewId);


	bool ExecuteCommand(Json::Object & cInObject);
	bool ExecuteCommand(Json::Object & cInObject, Json::Object & cRetObject);
	bool SendExecuteCommand(Json::Object & cObject);

	//== HPS 관련 명령어 =============================================================================
	bool InitDisplay();
	bool ExitDisplay();

	void SetErrorMessageVector(std::vector<std::string> * pvstrErrMsgVector) {
		m_pvstrErrMsgVector = pvstrErrMsgVector;
	}

	// == Model 관련 함수 ============================================================================
	bool CreateNewModelHandler(DWORD_PTR nId);
	bool ResetModelHandler(DWORD_PTR nId);

	// == Thread 관련 함수 ===========================================================================
	bool CommandCreateInsertItemThread(DWORD_PTR nId);
	static bool CreateInsertItemThread(DmiHpsSignalAnalyzer * pcHpsVisualize, DWORD_PTR nId);
	bool BeginInsertItemThread(DWORD_PTR nId);
	bool EndInsertItemThread(DWORD_PTR nId);

private:
	//Concurrency::concurrent_vector<MbItem *> m_vpcThreadItemVector;
	std::vector<MbItem *> m_vpcThreadItemVector;
	std::thread * m_pcInsertItemThread;
	bool m_bThreadRuningFlag = false;
	DmiHpsCreator m_cHpsCreator;

	// == View 관련 함수 =============================================================================
protected:
	DWORD_PTR CreateHpsView(DWORD_PTR nId);
	bool InitialUpdateHpsView(DWORD_PTR nId, HWND hWnd);

	bool OnPaintView(DWORD_PTR nId);
	bool DeleteHpsView(DWORD_PTR nId);
	//:Ken - 20220810
	bool OnResizeView(DWORD_PTR nId, int width = 0, int height = 0);

	bool AttachModelHandlerHpsView(DWORD_PTR nViewId, int nDocId);

	bool ZoomFitHpsView(DWORD_PTR nId);

	//== Mouse 관련 함수 =============================================================================
protected:
	bool ExecuteMouseSignal(int nViewId, int nAction, Json::Object & cInObject);

	bool LButtonUpHpsView(DWORD_PTR nId, int nFlags, int x, int y);
	bool LButtonDownHpsView(DWORD_PTR nId, int nFlags, int x, int y);

	bool MButtonUpHpsView(DWORD_PTR nId, int nFlags, int x, int y);
	bool MButtonDownHpsView(DWORD_PTR nId, int nFlags, int x, int y);

	bool RButtonUpHpsView(DWORD_PTR nId, int nFlags, int x, int y);
	bool RButtonDownHpsView(DWORD_PTR nId, int nFlags, int x, int y);

	bool MouseMoveHpsView(DWORD_PTR nId, int nFlags, int x, int y);

	bool MouseWheelHpsView(DWORD_PTR nId, int nFlags, int zDelta, int x, int y);

	//== File 관련 함수 ==============================================================================
	bool FileImport(CString strFilePathName, DWORD_PTR nId);
	bool EndFileImport(DWORD_PTR nDocId, DWORD_PTR nViewId);
	bool ImportExchangeFile(DWORD_PTR nViewId, CString & strFilePathName);
	bool CompleteImportExchangeFile(DWORD_PTR nId);

	//== Component Tree 관련 함수 ====================================================================
	bool CreateComponentRootTreeItem(DWORD_PTR nId, Json::Object & cRetObject);
	bool AddChildComponentTreeItem(DWORD_PTR nId, Json::Object & cInObject, Json::Object & cRetObject);
	bool SelectComponentTreeItem(DWORD_PTR nId, Json::Object & cInObject);

	bool InProgressImportStatus(DWORD_PTR nId);
	bool IsSuccessImportStatus(DWORD_PTR nId);
	// 	bool GetImportLogMessage(DWORD_PTR nId, CString & strJsonReturnMessage);
	// 	bool GetImportStatusMessage(DWORD_PTR nId, CString & strReturnMessage);

	bool WriteHsfFile(DWORD_PTR nId, CString & strFilePathName);

	// == C3D Data 관련 함수 =========================================================================
	bool ImportDataUpdate(DWORD_PTR nId, Json::Object & cInObject);
	bool InsertItem(DWORD_PTR nId, Json::Object & cInObject);

protected:
	CString GetExecuteDirectory();

private:
	// App
	HPS::World * m_pcWorld;

	std::map<DWORD_PTR, DmiModelHandler *> m_mapcModelHandlerMap;
	//std::map<DWORD_PTR, DmiHpsView *> m_mapcHpsViewMap;

	std::vector<std::string> * m_pvstrErrMsgVector = nullptr;

	DWORD_PTR m_nId = 0xffffff;
	DmiHpsInterface * m_pcHpsInterface = nullptr;

	std::chrono::system_clock::time_point m_cTimes[4];
};