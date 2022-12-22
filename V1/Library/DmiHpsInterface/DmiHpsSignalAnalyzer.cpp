#include "stdafx.h"
#include "DmiHpsSignalAnalyzer.h"

#include "../HPS/include/hoops_license.h"

#include "DmiHpsInterface.h"

#include "DmiModelHandler.h"
#include "DmiHpsView.h"

#include "DmiHpsFileInterface.h"

#include <templ_p_array.h>
#include <templ_s_array_rw.h>

#include <model.h>

#include "../Signal/Signal.h"
#include <Common_Define.h>

#define SKW_MOUSE			"Mouse"
#define SKW_MOUSE_DOWN		"MouseDown"
#define SKW_MOUSE_UP		"MouseUp"
#define SKW_MOVE			"Move"
#define SKW_SCROLL			"Scroll"

#define SKW_CLICK_COUNT		"ClickCount"
#define SKW_SIGNAL			"Signal"

#define CHECK_HPS_VIEW if(0 == ptrHpsView) { assert(FALSE);  return false; } 

using namespace c3d;
using namespace std;
using namespace std::chrono;

DmiHpsSignalAnalyzer::DmiHpsSignalAnalyzer(DmiHpsInterface * pcHpsInterface) :
	m_pcHpsInterface(pcHpsInterface)
{
	m_pcWorld = nullptr;
}

DmiHpsSignalAnalyzer::~DmiHpsSignalAnalyzer()
{
	if(nullptr != m_pcWorld) {
		delete m_pcWorld;
	}
}
//== 전달 받은 명령어 분기 =============================================================================

void DmiHpsSignalAnalyzer::ExecuteSignal(Json::Object & cInObject)
{
	int nAction = cInObject.GetInteger(SKW_ACTION);
	int nViewId = cInObject.GetInteger(SKW_VIEWID);

	switch((Signal::View::Action) nAction)
	{
		case Signal::View::Action::OnConstruct:
			break;

		// CView Windows에서 OnInitialize 함수에서 전달 받음.
		case Signal::View::Action::OnInitialize:
			OnInitializeView(nViewId, cInObject);
			break;

		case Signal::View::Action::OnDestruct:
			OnDestructView(nViewId);
			break;

		case Signal::View::Action::OnPaint:
			OnPaintView(nViewId);
			break;

		case Signal::View::Action::OnResize:
		{
			int nX = cInObject.GetInteger(SKW_X);
			int nY = cInObject.GetInteger(SKW_Y);

			OnResizeView(nViewId, nX, nY);
		}
		break;

		case Signal::View::Action::OnMouseMove:
		case Signal::View::Action::OnLButtonDown:
		case Signal::View::Action::OnLButtonUp:
		case Signal::View::Action::OnMButtonDown:
		case Signal::View::Action::OnMButtonUp:
		case Signal::View::Action::OnRButtonDown:
		case Signal::View::Action::OnRButtonUp:
		case Signal::View::Action::OnMouseWheel:
			ExecuteMouseSignal(nViewId, nAction, cInObject);
		break;

		default:
			assert(false);
			break;
	}

/*
	CString strEvent = cInObject.GetString(SKW_EVENT);

	if(true == strEvent.IsEmpty()) {
		ASSERT(false);
		return false;
	}

	if(SKW_MOUSE == strEvent) {
		return ExecuteMouseSignal(cInObject);
	}

	ASSERT(false);
	return false;
*/
}

void DmiHpsSignalAnalyzer::SendSignal(const wchar_t * pchBuffer)
{
	if(nullptr == m_pcHpsInterface) {
		assert(false);
		return;
	}

//	m_pcHpsInterface->SendSignal(pchBuffer);
}

//== HPS 초기화 및 종료 명령어 처리 ====================================================================

// 1. HPS 초기화 작업 시작
void DmiHpsSignalAnalyzer::OnInitializeView(int nViewId, Json::Object & cInObject)
{
	// 생성할 때 HPS World를 검사해서 생성하도록 한다.
	if(nullptr == m_pcWorld) {
		m_pcWorld = new HPS::World(HOOPS_LICENSE);
		if(nullptr == m_pcWorld) {
			// #Require_retun_message
			assert(false);
		}

		//_world->SetMaterialLibraryDirectory("../../samples/data/materials");
		//this->setFontDirectory("../../samples/fonts");

		std::wstring buffer;
		buffer.resize(32767);
		buffer = GetExecuteDirectory() + L"3DX";
		std::wstringstream bin_dir;
		bin_dir << buffer.data();;
		m_pcWorld->SetExchangeLibraryDirectory(HPS::UTF8(bin_dir.str().data()));

		if(nullptr == m_pcWorld) {
			// #Require_retun_message
			assert(false);
		}
	}

	// Model Handler를 생성해서, HPS 관련 정보를 View Id에 맞추어서 갖고 있도록 한다.
	DmiModelHandler * pcModelHandler = new DmiModelHandler(m_pcHpsInterface);
	if(nullptr == pcModelHandler) {
		// #Require_retun_message
		assert(false);
		return;
	}

	if(false == pcModelHandler->CreateNewModel()) {
		// #Require_retun_message
		assert(false);
		return;
	}

	m_mapcModelHandlerMap[nViewId] = pcModelHandler;

	// Create View
	DmiHpsView * pcHpsView = new DmiHpsView(m_pcHpsInterface, nViewId);
	if(nullptr == pcHpsView) {
		// #Require_retun_message
		assert(false);
		return;
	}

	pcModelHandler->SetHpsView(pcHpsView);
	pcHpsView->SetModelHandler(pcModelHandler);

	HWND hWnd = (HWND)cInObject.GetDwordPtr(SKW_HWND);

	if(false == pcHpsView->InitialUpdate(hWnd, pcModelHandler)) {
		// #Require_retun_message
		assert(false);
		return;
	}

	CString strFilePathName = cInObject.GetString(SKW_FILEPATH);
	
	// File Path가 없는 경우 처리, File New 했을 경우
	if(true == strFilePathName.IsEmpty()) {
		pcHpsView->Update();
		return;
	}

	// File Import 시작
	ImportExchangeFile(nViewId, strFilePathName);

	CompleteImportExchangeFile(nViewId);

	// Progress dialog Showing
	Signal::Delivery delivery;
	delivery.ViewId = nViewId;
	delivery.SetSender(m_pcHpsInterface->GetSignalCallback());
	delivery.mainFrame.HideProgress();

	delivery.view.SetValidation();
}

void DmiHpsSignalAnalyzer::OnDestructView(int nViewId)
{
	m_mapcModelHandlerMap[nViewId]->GetHpsView()->GetCanvas().Delete();
}

bool DmiHpsSignalAnalyzer::ExecuteCommand(Json::Object & cInObject)
{
	DWORD_PTR nId = cInObject.GetDwordPtr("Id");
	DmiModelHandler * pcModelHandler = m_mapcModelHandlerMap[nId];

	CString strCommand = cInObject.GetString("Command");

	// #Require_convert_new_version
/*
	DmiHps::Command eCommandId = (DmiHps::Command) cInObject.GetDwordPtr((UINT) DmiHps::Command::Id);

	switch(eCommandId)
	{
		case DmiHps::Command::HpsInit:
			return InitDisplay();
			break;

		case DmiHps::Command::HpsExitDisplay:
			return ExitDisplay();
			break;
	}

	eCommandId = (DmiHps::Command) cInObject.GetDwordPtr("CommandId");

	switch(eCommandId)
	{
		case DmiHps::Command::CreateInsertModelThread:
			return CommandCreateInsertItemThread(nId);
			break;

		case DmiHps::Command::DesteructInsertModelThread:
			return EndInsertItemThread(nId);
			break;

		case DmiHps::Command::InsertItem:
			return InsertItem(nId, cInObject);
			break;
		
		case DmiHps::Command::ImportExchangeFile:
			return ImportExchangeFile(nId, cInObject);
			break;

		case DmiHps::Command::RequestImportExchangeNotifierStatus:
		{
			if(nullptr != pcModelHandler) {
				return pcModelHandler->RequestImportExchangeNotifierStatus();
			}
			return false;
		}
		break;

		case DmiHps::Command::ImportExchangeLog:
		{
			if(nullptr != pcModelHandler) {
				return pcModelHandler->ImportExchangeLog();
			}
			return false;
		}
		break;

		case DmiHps::Command::CompleteImportExchangeFile: 
			return CompleteImportExchangeFile(nId);
			break;
	}*/

	// == Mouse 관련 Command ========================================================================
	if(L"MouseMove" == strCommand) {
		DWORD_PTR nId = cInObject.GetDwordPtr("Id");
		int nFlag = (int) cInObject.GetDwordPtr("Value1");
		int nX = (int) cInObject.GetDwordPtr("Value2");
		int nY = (int) cInObject.GetDwordPtr("Value3");
		return MouseMoveHpsView(nId, nFlag, nX, nY);
	}
	else if(L"LButtonDown" == strCommand) {
		DWORD_PTR nId = cInObject.GetDwordPtr("Id");
		int nFlag = (int) cInObject.GetDwordPtr("Value1");
		int nX = (int) cInObject.GetDwordPtr("Value2");
		int nY = (int) cInObject.GetDwordPtr("Value3");
		return LButtonDownHpsView(nId, nFlag, nX, nY);
	}
	else if(L"LButtonUp" == strCommand) {
		DWORD_PTR nId = cInObject.GetDwordPtr("Id");
		int nFlag = (int) cInObject.GetDwordPtr("Value1");
		int nX = (int) cInObject.GetDwordPtr("Value2");
		int nY = (int) cInObject.GetDwordPtr("Value3");
		return LButtonUpHpsView(nId, nFlag, nX, nY);
	}
	else if(L"MButtonDown" == strCommand) {
		DWORD_PTR nId = cInObject.GetDwordPtr("Id");
		int nFlag = (int) cInObject.GetDwordPtr("Value1");
		int nX = (int) cInObject.GetDwordPtr("Value2");
		int nY = (int) cInObject.GetDwordPtr("Value3");
		return MButtonDownHpsView(nId, nFlag, nX, nY);
	}
	else if(L"MButtonUp" == strCommand) {
		DWORD_PTR nId = cInObject.GetDwordPtr("Id");
		int nFlag = (int) cInObject.GetDwordPtr("Value1");
		int nX = (int) cInObject.GetDwordPtr("Value2");
		int nY = (int) cInObject.GetDwordPtr("Value3");
		return MButtonUpHpsView(nId, nFlag, nX, nY);
	}
	else if(L"RButtonDown" == strCommand) {
		DWORD_PTR nId = cInObject.GetDwordPtr("Id");
		int nFlag = (int) cInObject.GetDwordPtr("Value1");
		int nX = (int) cInObject.GetDwordPtr("Value2");
		int nY = (int) cInObject.GetDwordPtr("Value3");
		return RButtonDownHpsView(nId, nFlag, nX, nY);
	}
	else if(L"RButtonUp" == strCommand) {
		DWORD_PTR nId = cInObject.GetDwordPtr("Id");
		int nFlag = (int) cInObject.GetDwordPtr("Value1");
		int nX = (int) cInObject.GetDwordPtr("Value2");
		int nY = (int) cInObject.GetDwordPtr("Value3");
		return RButtonUpHpsView(nId, nFlag, nX, nY);
	}
	else if(L"MouseWheel" == strCommand) {
		DWORD_PTR nId = (int) cInObject.GetDwordPtr("Id");
		int nFlag = (int) cInObject.GetDwordPtr("Value1");
		int zDelta = (int) cInObject.GetDwordPtr("Value2");
		int nX = (int) cInObject.GetDwordPtr("Value3");
		int nY = (int) cInObject.GetDwordPtr("Value4");
		return MouseWheelHpsView(nId, nFlag, zDelta, nX, nY);
	}

	// == HPS View 관련 Command =====================================================================
	else if(L"CreateHpsView" == strCommand) {
		DWORD_PTR nId = cInObject.GetDwordPtr("Id");
		return CreateHpsView(nId);
	}
	else if(L"DeleteHpsView" == strCommand) {
		DWORD_PTR nId = cInObject.GetDwordPtr("Id");
		return DeleteHpsView(nId);
	}
	else if(L"InitialUpdateHpsView" == strCommand) {
		DWORD_PTR nId = cInObject.GetDwordPtr("Id");
		HWND hWnd = (HWND) cInObject.GetDwordPtr("Value");
		return InitialUpdateHpsView(nId, hWnd);
	}
	else if(L"AttachModelHandlerHpsView" == strCommand) {
		DWORD_PTR nId = cInObject.GetDwordPtr("Id");
		int nDocId = (int) cInObject.GetDwordPtr("Value");
		return AttachModelHandlerHpsView(nId, nDocId);
	}
	else if(L"UpdateHpsView" == strCommand) {
		DWORD_PTR nId = cInObject.GetDwordPtr("Id");
		return OnPaintView(nId);
	}
	//:Ken - 20220810
	else if(L"ResizeHpsView" == strCommand) {
		DWORD_PTR nId = cInObject.GetDwordPtr("Id");
		return OnResizeView(nId);
	}
	else if(L"ZoomFit" == strCommand) {
		DWORD_PTR nId = cInObject.GetDwordPtr("Id");
		return ZoomFitHpsView(nId);
	}

	// == Model Hander 관련 Command =================================================================
	else if(L"CreateModelHandler" == strCommand) {
		DWORD_PTR nId = cInObject.GetDwordPtr("Id");
		return CreateNewModelHandler(nId);
	}
	else if(L"ResetModelHandler" == strCommand) {
		DWORD_PTR nId = cInObject.GetDwordPtr("Id");
		return ResetModelHandler(nId);
	}
	else if(L"WriteHsfFile" == strCommand) {
		DWORD_PTR nId = cInObject.GetDwordPtr("Id");
		CString strFilePathName = cInObject.GetString("Value");
		return WriteHsfFile(nId, strFilePathName);
	}

	// == File 관련 Command =========================================================================
	else if(L"FileOpen" == strCommand) {
		DWORD_PTR nId = cInObject.GetDwordPtr("Id");
		CString strFilePathName = cInObject.GetString("Value");
		return FileImport(strFilePathName, nId);
	}
	else if(L"EndFileImport" == strCommand) {
		DWORD_PTR nDocId = cInObject.GetDwordPtr("Id");
		DWORD_PTR nViewId = cInObject.GetDwordPtr("Value");
		return EndFileImport(nDocId, nViewId);
	}
	// #temp : remark GetImportLogMessage
/*
	else if(L"GetImportLogMessage" == strCommand) {
		DWORD_PTR nId = cInObject.GetDwordPtr("Id");
		return GetImportLogMessage(nId);
	}
	else if(L"GetImportStatusMessage" == strCommand) {
		DWORD_PTR nId = cInObject.GetDwordPtr("Id");
		return GetImportStatusMessage(nId);
	}
*/
	else if(L"InProgressImportStatus" == strCommand) {
		DWORD_PTR nId = cInObject.GetDwordPtr("Id");
		return InProgressImportStatus(nId);
	}
	else if(L"IsSuccessImportStatus" == strCommand) {
		DWORD_PTR nId = cInObject.GetDwordPtr("Id");
		return IsSuccessImportStatus(nId);
	}
	else if(L"ImportDataUpdate" == strCommand) {
		DWORD_PTR nId = cInObject.GetDwordPtr("Id");
		return ImportDataUpdate(nId, cInObject);
	}

	// == Model 생성 관련 Command ====================================================================
	else if(L"CreateInsertModelThread" == strCommand) {

	}
	else if(L"DesteructInsertModelThread" == strCommand) {

	}

	ASSERT(false);
	return false;
}

bool DmiHpsSignalAnalyzer::ExecuteCommand(Json::Object & cInObject, Json::Object & cRetObject)
{
	DWORD_PTR nId = cInObject.GetDwordPtr("Id");

	// #Require_convert_new_version
/*
	DmiHps::Command eCommandId = (DmiHps::Command) cInObject.GetDwordPtr("CommandId");

	switch(eCommandId)
	{
		// ----- Component Tree 관련 -----
		case DmiHps::Command::CreateComponentRootTreeItem:
			return CreateComponentRootTreeItem(nId, cRetObject);
			break;
		case DmiHps::Command::AddChildComponentTreeItem:
			return AddChildComponentTreeItem(nId, cInObject, cRetObject);
			break;

		case DmiHps::Command::SelectComponentTreeItem:
			return SelectComponentTreeItem(nId, cInObject);
			break;
	}
*/

	RETURN_FALSE;
}

bool DmiHpsSignalAnalyzer::SendExecuteCommand(Json::Object & cObject)
{
/*
	if(nullptr == m_pcHpsInterface) {
		return false;
	}

	return m_pcHpsInterface->SendExecuteCommand(cObject);*/

	return false;
}


//== HPS 관련 명령어 =================================================================================
bool DmiHpsSignalAnalyzer::InitDisplay()
{
	// [hps_init]
	m_pcWorld = new HPS::World(HOOPS_LICENSE);
	if(nullptr == m_pcWorld) {
		return false;
	}

	//_world->SetMaterialLibraryDirectory("../../samples/data/materials");
	//this->setFontDirectory("../../samples/fonts");

	std::wstring buffer;
	buffer.resize(32767);
	buffer = GetExecuteDirectory() + L"3DX";
	std::wstringstream bin_dir;
	bin_dir << buffer.data();;
	m_pcWorld->SetExchangeLibraryDirectory(HPS::UTF8(bin_dir.str().data()));

	return (DWORD_PTR) m_pcWorld;
}

bool DmiHpsSignalAnalyzer::ExitDisplay()
{
	if(nullptr != m_pcWorld) {
		delete m_pcWorld;
		m_pcWorld = nullptr;

		return true;
	}

	return false;
}

// 실행파일 디렉토리
CString DmiHpsSignalAnalyzer::GetExecuteDirectory()
{
	TCHAR szBuffer[MAX_PATH];
	TCHAR Drive[_MAX_DRIVE];
	TCHAR Path[_MAX_PATH];
	TCHAR Filename[_MAX_FNAME];
	TCHAR Ext[_MAX_EXT];

	GetModuleFileName(NULL, szBuffer, sizeof(szBuffer));	// get process file name
	_wsplitpath_s(szBuffer, Drive, _MAX_DRIVE, Path, _MAX_PATH, Filename, _MAX_FNAME, Ext, _MAX_EXT);		// get drive, path, file, ext name

	CString strFilePath;
	strFilePath.Format(L"%s%s", Drive, Path);

	return strFilePath;
}

// == Model 관련 함수 ===============================================================================
bool DmiHpsSignalAnalyzer::CreateNewModelHandler(DWORD_PTR nId)
{
	DmiModelHandler * pcModelHanler = new DmiModelHandler(m_pcHpsInterface);
	CHECK_POINTER(pcModelHanler);

	if(false == pcModelHanler->CreateNewModel()) {
		return false;
	}

	m_mapcModelHandlerMap[nId] = pcModelHanler;

	return true;
}

bool DmiHpsSignalAnalyzer::ResetModelHandler(DWORD_PTR nId)
{
	DmiModelHandler * pcLocalModelHanler = m_mapcModelHandlerMap[nId];
	CHECK_POINTER(pcLocalModelHanler);

	bool bStatus = pcLocalModelHanler->DeleteModel();
	pcLocalModelHanler->DeleteCadModel();
	delete pcLocalModelHanler;

	return bStatus; // pcLocalModelHanler->DeleteModel();
}

// == 2. Thread 관련 함수 ===========================================================================

// 2-1. Thread 생성 명령어 처리 함수
bool DmiHpsSignalAnalyzer::CommandCreateInsertItemThread(DWORD_PTR nId)
{
	m_pcInsertItemThread = new std::thread(CreateInsertItemThread, this, nId);
	//m_cInsertItemThread.join();

	return true;
}

// 2-2. Thread를 생성하는 함수
bool DmiHpsSignalAnalyzer::CreateInsertItemThread(DmiHpsSignalAnalyzer * pcHpsVisualize, DWORD_PTR nId)
{
	return pcHpsVisualize->BeginInsertItemThread(nId);
}

bool DmiHpsSignalAnalyzer::BeginInsertItemThread(DWORD_PTR nId)
{
	DmiModelHandler * pcModelHanler = m_mapcModelHandlerMap[nId];
	m_cHpsCreator.SetModelHandler(pcModelHanler);

	m_bThreadRuningFlag = true;

	int nCount = 0;
	// Thread runing flag이 true인 동안에는 계속 Loop를 실행한다.
	while(true == m_bThreadRuningFlag) {

		// Thread Item Vector에 내용이 있는 동안에는 계속해서 작동을 한다.
		// Thread Item Vector에 내용은 외부에서 계속 추가되도록 한다.
		while(false == m_vpcThreadItemVector.empty()) {
			//for(size_t nIndex = 0; nIndex < m_vpcThreadItemVector.size(); nIndex++) {
			MbItem * pcItem = m_vpcThreadItemVector.front();
			/*
						//MbItem * pcItem = m_vpcThreadItemVector[nIndex];
						if(nullptr == pcItem) {
							continue;
						}
			*/
			//m_vpcThreadItemVector[nIndex] = nullptr;
			//MbItem * pcItem1 = m_vpcThreadItemVector.front();

			m_vpcThreadItemVector.erase(m_vpcThreadItemVector.begin());

			m_cHpsCreator.CreateItem(pcItem);
			//pcHpsView->GetCanvas().Update(HPS::Window::UpdateType::Default);

			nCount++;

			// 			if(10 < nCount) {
			// 				ZoomFitHpsView(nId);
			// 				nCount = 0;
			// 			}


						//Sleep(10);
		}
	}

	ZoomFitHpsView(nId);

	DmiHpsView * pcHpsView = pcModelHanler->GetHpsView();
	HPS::UpdateNotifier updateNotifier = pcHpsView->GetCanvas().UpdateWithNotifier(HPS::Window::UpdateType::Exhaustive);
	updateNotifier.Wait();

	return true;
}

bool DmiHpsSignalAnalyzer::EndInsertItemThread(DWORD_PTR nId)
{
	m_bThreadRuningFlag = false;

	if(true == m_pcInsertItemThread->joinable()) {
		m_pcInsertItemThread->join();
		delete m_pcInsertItemThread;
		m_pcInsertItemThread = nullptr;
	}

	//ZoomFitHpsView(nId);

	return true;
}

bool DmiHpsSignalAnalyzer::InsertItem(DWORD_PTR nId, Json::Object & cInObject)
{
	DmiHpsView * pcHpsView = m_mapcModelHandlerMap[nId]->GetHpsView();

	MbItem * pcItem = (MbItem *) cInObject.GetDwordPtr(L"MbItem");
	m_vpcThreadItemVector.push_back(pcItem);

	//pcHpsView->GetCanvas().Update(HPS::Window::UpdateType::Default);

	return true;
}

//== HPS View 관련 함수 ==============================================================================
DWORD_PTR DmiHpsSignalAnalyzer::CreateHpsView(DWORD_PTR nId)
{
	DmiHpsView * pcHpsView = new DmiHpsView(m_pcHpsInterface, nId);
	CHECK_DWORD_PTR(pcHpsView);

	m_mapcModelHandlerMap[nId]->SetHpsView(pcHpsView);
	pcHpsView->SetModelHandler(m_mapcModelHandlerMap[nId]);

	//m_mapcHpsViewMap.insert(std::make_pair(nId, pcHpsView));

	return true;
}

bool DmiHpsSignalAnalyzer::InitialUpdateHpsView(DWORD_PTR nId, HWND hWnd)
{
	DmiModelHandler * pcModelHandler = m_mapcModelHandlerMap[nId];
	CHECK_POINTER(pcModelHandler);

	DmiHpsView * pcHpsView = pcModelHandler->GetHpsView();
	CHECK_DWORD_PTR(pcHpsView);

	return pcHpsView->InitialUpdate(hWnd, pcModelHandler);
}

bool DmiHpsSignalAnalyzer::OnPaintView(DWORD_PTR nId)
{
	DmiHpsView * pcHpsView = m_mapcModelHandlerMap[nId]->GetHpsView();
	CHECK_DWORD_PTR(pcHpsView);

	pcHpsView->Update();

	TRACE(L"DmiHpsView::Update() : %d\r\n", nId);

	return true;
}

bool DmiHpsSignalAnalyzer::DeleteHpsView(DWORD_PTR nId)
{
/*
	for(auto cIterator = m_mapcHpsViewMap.begin(); cIterator != m_mapcHpsViewMap.end(); cIterator++)
	{
		if(nId == cIterator->first)
		{
			delete cIterator->second;
			m_mapcHpsViewMap.erase(cIterator);

			return true;
		}
	}
*/

	return false;
}

bool DmiHpsSignalAnalyzer::OnResizeView(DWORD_PTR nId, int width, int height)
{
	DmiHpsView * pcHpsView = m_mapcModelHandlerMap[nId]->GetHpsView();
	CHECK_DWORD_PTR(pcHpsView);

	pcHpsView->RefreshUpdate();
	//pcHpsView->ResizeCanvas(width, height);

	return true;
}

bool DmiHpsSignalAnalyzer::AttachModelHandlerHpsView(DWORD_PTR nViewId, int nDocId)
{
	DmiModelHandler * pcModelHandler = m_mapcModelHandlerMap[nDocId];
	CHECK_POINTER(pcModelHandler);

	DmiHpsView * pcHpsView = pcModelHandler->GetHpsView();
	CHECK_POINTER(pcHpsView);

	return pcHpsView->AttachModelHandler(pcModelHandler);
}

bool DmiHpsSignalAnalyzer::ZoomFitHpsView(DWORD_PTR nId)
{
	DmiHpsView * pcHpsView = m_mapcModelHandlerMap[nId]->GetHpsView();
	CHECK_DWORD_PTR(pcHpsView);

	return pcHpsView->ZoomFit();
}

// == Mouse Function ===============================================================================

// 1. Mouse Signal 처리 함수
bool DmiHpsSignalAnalyzer::ExecuteMouseSignal(int nViewId, int nAction, Json::Object & cInObject)
{
	int nFlag = cInObject.GetInteger(SKW_FLAG);
	int x = cInObject.GetInteger(SKW_X);
	int y = cInObject.GetInteger(SKW_Y);

	switch((Signal::View::Action)nAction)
	{
		case Signal::View::Action::OnMouseMove:
			return MouseMoveHpsView(nViewId, nFlag, x, y);
			break;

		case Signal::View::Action::OnLButtonDown:
			return LButtonDownHpsView(nViewId, nFlag, x, y);
			break;

		case Signal::View::Action::OnLButtonUp:
			return LButtonUpHpsView(nViewId, nFlag, x, y);
			break;

		case Signal::View::Action::OnMButtonDown:
			return MButtonDownHpsView(nViewId, nFlag, x, y);
			break;

		case Signal::View::Action::OnMButtonUp:
			return MButtonUpHpsView(nViewId, nFlag, x, y);
			break;

		case Signal::View::Action::OnRButtonDown:
			return RButtonDownHpsView(nViewId, nFlag, x, y);
			break;

		case Signal::View::Action::OnRButtonUp:
			return RButtonUpHpsView(nViewId, nFlag, x, y);
			break;

		case Signal::View::Action::OnMouseWheel:
		{
			int zDelta = cInObject.GetInteger(SKW_DELTA, -120);
			return MouseWheelHpsView(nViewId, nFlag, zDelta, x, y);
		}
		break;
		
		default:
			assert(false);
			break;
	}

	return false;
}

// 2. Left Button 처리 함수
bool DmiHpsSignalAnalyzer::LButtonDownHpsView(DWORD_PTR nId, int nFlags, int x, int y)
{
	DmiHpsView * pcHpsView = m_mapcModelHandlerMap[nId]->GetHpsView();
	CHECK_DWORD_PTR(pcHpsView);

	return pcHpsView->LButtonDown(nFlags, x, y);
}

bool DmiHpsSignalAnalyzer::LButtonUpHpsView(DWORD_PTR nId, int nFlags, int x, int y)
{
	DmiHpsView * pcHpsView = m_mapcModelHandlerMap[nId]->GetHpsView();
	CHECK_DWORD_PTR(pcHpsView);

	HPS::View cView = pcHpsView->GetCanvas().GetFrontView();

	HPS::CameraControl cCameraControl = cView.GetSegmentKey().GetCameraControl();

	return pcHpsView->LButtonUp(nFlags, x, y);
}

// 3. Middle Button 처리 함수
bool DmiHpsSignalAnalyzer::MButtonDownHpsView(DWORD_PTR nId, int nFlags, int x, int y)
{
	DmiHpsView * pcHpsView = m_mapcModelHandlerMap[nId]->GetHpsView();
	CHECK_DWORD_PTR(pcHpsView);

	return pcHpsView->MButtonDown(nFlags, x, y);
}

bool DmiHpsSignalAnalyzer::MButtonUpHpsView(DWORD_PTR nId, int nFlags, int x, int y)
{
	DmiHpsView * pcHpsView = m_mapcModelHandlerMap[nId]->GetHpsView();
	CHECK_DWORD_PTR(pcHpsView);

	return pcHpsView->MButtonUp(nFlags, x, y);
}

// 4. Right Button 처리 함수
bool DmiHpsSignalAnalyzer::RButtonUpHpsView(DWORD_PTR nId, int nFlags, int x, int y)
{
	DmiHpsView * pcHpsView = m_mapcModelHandlerMap[nId]->GetHpsView();
	CHECK_DWORD_PTR(pcHpsView);

	return pcHpsView->RButtonUp(nFlags, x, y);
}


bool DmiHpsSignalAnalyzer::RButtonDownHpsView(DWORD_PTR nId, int nFlags, int x, int y)
{
	DmiHpsView * pcHpsView = m_mapcModelHandlerMap[nId]->GetHpsView();
	CHECK_DWORD_PTR(pcHpsView);

	return pcHpsView->RButtonDown(nFlags, x, y);
}

// 5. Mouse Move 처리 함수
bool DmiHpsSignalAnalyzer::MouseMoveHpsView(DWORD_PTR nId, int nFlags, int x, int y)
{
	DmiHpsView * pcHpsView = m_mapcModelHandlerMap[nId]->GetHpsView();
	CHECK_DWORD_PTR(pcHpsView);

	return pcHpsView->MouseMove(nFlags, x, y);
}

// 6. Mouse Wheel 처리 함수
bool DmiHpsSignalAnalyzer::MouseWheelHpsView(DWORD_PTR nId, int nFlags, int zDelta, int x, int y)
{
	DmiHpsView * pcHpsView = m_mapcModelHandlerMap[nId]->GetHpsView();
	CHECK_DWORD_PTR(pcHpsView);

	return pcHpsView->MouseWheel(nFlags, zDelta, x, y);
}

// == File 관련 함수 ================================================================================

bool DmiHpsSignalAnalyzer::FileImport(CString strFilePathName, DWORD_PTR nViewId)
{
	// Progress dialog Showing
	Signal::Delivery delivery;
	delivery.ViewId = nViewId;
	delivery.SetSender(m_pcHpsInterface->GetSignalCallback());
	delivery.mainFrame.ShowProgress();

	DmiModelHandler * pcModelHandler = m_mapcModelHandlerMap[nViewId];
	CHECK_POINTER(pcModelHandler);

	return pcModelHandler->FileImport(strFilePathName);
}

bool DmiHpsSignalAnalyzer::EndFileImport(DWORD_PTR nDocId, DWORD_PTR nViewId)
{
	DmiModelHandler * pcModelHandler = m_mapcModelHandlerMap[nDocId];
	CHECK_POINTER(pcModelHandler);

	CHECK_POINTER(pcModelHandler->GetHpsFileInterface());

	DmiHpsView * pcHpsView = pcModelHandler->GetHpsView();
	CHECK_POINTER(pcHpsView);

	return pcModelHandler->GetHpsFileInterface()->EndFileImport(pcModelHandler, pcHpsView);
}

// HPS에서 Exchange를 이용해서 CadModel로 Import하는 함수
bool DmiHpsSignalAnalyzer::ImportExchangeFile(DWORD_PTR nViewId, CString & strFilePathName)
{
	// Progress dialog Showing
	Signal::Delivery delivery;
	delivery.ViewId = nViewId;
	delivery.SetSender(m_pcHpsInterface->GetSignalCallback());
	delivery.mainFrame.ShowProgress();

	m_cTimes[0] = system_clock::now();

	m_nId = nViewId;
	DmiModelHandler * pcModelHandler = m_mapcModelHandlerMap[nViewId];
	CHECK_POINTER(pcModelHandler);

	return pcModelHandler->ImportExchangeFile(strFilePathName, m_cTimes);
}

bool DmiHpsSignalAnalyzer::CompleteImportExchangeFile(DWORD_PTR nId)
{
	DmiModelHandler * pcModelHandler = m_mapcModelHandlerMap[nId];
	CHECK_POINTER(pcModelHandler);

	DmiHpsView * pcView = pcModelHandler->GetHpsView();
	CHECK_POINTER(pcView);

	pcModelHandler->CompleteImportExchangeFile();

	// CAD Model의 내용이 있는 경우 처리
	if(false == pcModelHandler->GetCADModel().Empty()) {
		pcModelHandler->GetCADModel().GetModel().GetSegmentKey().GetPerformanceControl().SetStaticModel(HPS::Performance::StaticModel::Attribute);
		pcView->AttachView(pcModelHandler->GetCADModel().ActivateDefaultCapture().FitWorld(), pcModelHandler->GetCADModel());
	}

	m_cTimes[2] = system_clock::now();

	// 화면 Update Notifer 처리
	HPS::UpdateNotifier updateNotifier = pcView->GetCanvas().UpdateWithNotifier(HPS::Window::UpdateType::Exhaustive);
	// 화면 Update까지 대기
	updateNotifier.Wait();

	m_cTimes[3] = system_clock::now();

	// #Require_convert_new_version
/*
	Json::Object cResultMsgObject;
	cResultMsgObject.SetDwordPtr("Id", m_nId);
	cResultMsgObject.SetString("Type", MSG_APP_TYPE);
	cResultMsgObject.SetInteger("MSG", MSG_FILE_IMPORT_EXCHANGE_INFORMATION);
	cResultMsgObject.SetLongLong("TimeTick1", m_cTimes[0].time_since_epoch().count());
	cResultMsgObject.SetLongLong("TimeTick2", m_cTimes[1].time_since_epoch().count());
	cResultMsgObject.SetLongLong("TimeTick3", m_cTimes[2].time_since_epoch().count());
	cResultMsgObject.SetLongLong("TimeTick4", m_cTimes[3].time_since_epoch().count());

	SendExecuteCommand(cResultMsgObject);
*/

	return true;
}

bool DmiHpsSignalAnalyzer::InProgressImportStatus(DWORD_PTR nId)
{
	DmiModelHandler * pcModelHandler = m_mapcModelHandlerMap[nId];
	CHECK_POINTER(pcModelHandler);

	CHECK_POINTER(pcModelHandler->GetHpsFileInterface());

	return pcModelHandler->GetHpsFileInterface()->InProgressImportStatus();
}

bool DmiHpsSignalAnalyzer::IsSuccessImportStatus(DWORD_PTR nId)
{
	DmiModelHandler * pcModelHandler = m_mapcModelHandlerMap[nId];
	CHECK_POINTER(pcModelHandler);

	CHECK_POINTER(pcModelHandler->GetHpsFileInterface());

	return pcModelHandler->GetHpsFileInterface()->IsSuccessImportStatus();
}

/*

bool RtHpsVisualize::GetImportLogMessage(DWORD_PTR nId, CString & strJsonReturnMessage)
{
	RtModelHandler * pcModelHandler = m_mapcModelHandlerMap[nId];
	CHECK_POINTER(pcModelHandler);

	CHECK_POINTER(pcModelHandler->GetHpsFileInterface());

	return pcModelHandler->GetHpsFileInterface()->GetImportLogMessage(strJsonReturnMessage);
}


bool RtHpsVisualize::GetImportStatusMessage(DWORD_PTR nId)
{
	RtModelHandler * pcModelHandler = m_mapcModelHandlerMap[nId];
	CHECK_POINTER(pcModelHandler);

	CHECK_POINTER(pcModelHandler->GetHpsFileInterface());

	return true;
}
*/

bool DmiHpsSignalAnalyzer::WriteHsfFile(DWORD_PTR nId, CString & strFilePathName)
{
	DmiModelHandler * pcModelHandler = m_mapcModelHandlerMap[nId];
	CHECK_POINTER(pcModelHandler);

	DmiHpsView * pcHpsView = pcModelHandler->GetHpsView();

	HPS::Model cModel = pcModelHandler->GetModel();
	if(cModel.Type() == HPS::Type::None) {
		return false;
	}

	HPS::Stream::ExportOptionsKit cExportOptionsKit;
	HPS::SegmentKey cExportFromHere = pcModelHandler->GetModel().GetSegmentKey();

	CStringA strAnsiFilePathName(strFilePathName);
	HPS::Stream::ExportNotifier cNotifier = HPS::Stream::File::Export(strAnsiFilePathName, cExportFromHere, cExportOptionsKit);

	return true;
}

bool DmiHpsSignalAnalyzer::ImportDataUpdate(DWORD_PTR nId, Json::Object & cInObject)
{
	//Math::SetMultithreaded(false);
	DmiModelHandler * pcModelHanler = m_mapcModelHandlerMap[nId];
	if(nullptr != pcModelHanler) {
		RETURN_FALSE;
	}

	MbModel * pcModel = (MbModel *) cInObject.GetDwordPtr(L"Model");
	if(nullptr != pcModelHanler) {
		RETURN_FALSE;
	}

	DmiHpsView * pcHpsView = pcModelHanler->GetHpsView();
	if(nullptr != pcHpsView) {
		RETURN_FALSE;
	}

	// Tessellation 도중에 화면에 표시하지 않는다.
	//HPS::UpdateNotifier updateNotifier = pcHpsView->GetCanvas().UpdateWithNotifier(HPS::Window::UpdateType::Complete).Wait();
	//HPS::UpdateNotifier updateNotifier = pcHpsView->GetCanvas().GetWindowKey().UpdateWithNotifier(HPS::Window::UpdateType::Complete).Wait();

	DmiHpsCreator cCreator(pcModelHanler);

	// Error Message를 받아온다.
	std::vector<std::string> vstrHpsErrMsgVector;
	cCreator.SetErrorMessageVector(&vstrHpsErrMsgVector);
	bool bStatus = cCreator.CreateMbModel(pcModel);

	// #Require_convert_new_version
/*
	Json::Array & cHpsError = cInObject.GenArray("HpsError");
	for(auto strErrMsg : vstrHpsErrMsgVector) {
		cHpsError.AddString(strErrMsg.c_str());
	}
*/

	pcHpsView->ZoomFit();

	HPS::UpdateNotifier updateNotifier = pcHpsView->GetCanvas().UpdateWithNotifier(HPS::Window::UpdateType::Exhaustive);
	updateNotifier.Wait();

	return true;
}

//== Component Tree 관련 함수 ========================================================================

// 1. 처음 Component Root Tree Item을 생성
bool DmiHpsSignalAnalyzer::CreateComponentRootTreeItem(DWORD_PTR nId, Json::Object & cRetObject)
{
	DmiModelHandler * pcModelHandler = m_mapcModelHandlerMap[nId];
	CHECK_POINTER(pcModelHandler);

	DmiHpsView * pcHpsView = pcModelHandler->GetHpsView();
	CHECK_DWORD_PTR(pcHpsView);

// 	HPS::ComponentArray acComponentArray = pcModelHandler->GetCADModel().GetSubcomponents();
// 	size_t nSize = acComponentArray.size();
// 	HPS::Component::ComponentType eType = acComponentArray[0].GetComponentType();

	// 1. 신규 Component Tree 생성
	DmiHpsComponentTreePtr pcComponentTree = std::make_shared<DmiHpsComponentTree>(pcHpsView->GetCanvas());

	// 2. Model Handler에 저장
	pcModelHandler->SetComponentTree(pcComponentTree);

	// 3. Highlight Option Kit 지정
	HPS::HighlightOptionsKit highlight_options;
	highlight_options.SetStyleName(HIGHLIGHT_STYLE_NAME).SetNotification(true);
	pcComponentTree->SetHighlightOptions(highlight_options);

	// 4. Root Item 생성
	//const ComponentTreeItemPtr * pcRoot = std::make_shared<DmiHpsComponentTreeItem>(pcComponentTree, pcModelHandler->GetCADModel());
	DmiHpsComponentTreeItemPtr pcRoot = std::make_shared<DmiHpsComponentTreeItem>(pcComponentTree, pcModelHandler->GetCADModel());
	pcComponentTree->SetRoot(pcRoot);

	//pcRoot->GetTreeCtrlItem()->Expand();
	//pcRoot->Expand();

	pcRoot->GetJsonObject(cRetObject);

/*
	return pcHpsView->InitialUpdate(hWnd, pcModelHandler);

	// CreateComponentRootTreeItem
	componentTree = std::make_shared<DmiHpsComponentTree>(doc->GetCHPSView()->GetCanvas(), this);
	auto root = std::make_shared<DmiHpsComponentTreeItem>(componentTree, cadModel);
	componentTree->SetRoot(root);
*/

	return true;
}

// 2. 하부 Component Root Tree Item 생성 (주어진 Item의 하부 아이탬을 전개해서 Child Item을 Json으로 전달)
bool DmiHpsSignalAnalyzer::AddChildComponentTreeItem(DWORD_PTR nId, Json::Object & cInObject, Json::Object & cRetObject)
{
	DmiModelHandler * pcModelHandler = m_mapcModelHandlerMap[nId];
	CHECK_POINTER(pcModelHandler);

	DmiHpsView * pcHpsView = pcModelHandler->GetHpsView();
	CHECK_DWORD_PTR(pcHpsView);

	// 1. Component Tree를 Model Handler에서 가져옴.
	DmiHpsComponentTreePtr pcComponentTree = pcModelHandler->GetComponentTree();

	// 2. 전달받은 ComponentTreeItem
	DmiHpsComponentTreeItem * pcTreeItem = (DmiHpsComponentTreeItem *)cInObject.GetDwordPtr("CompPtr");

	// 3. 확장 여부 확인
	if(true == pcTreeItem->IsExpanded()) {
		return true;;
	}

	pcTreeItem->Expand();

	pcTreeItem->GetJsonObject(cRetObject);

// 	CString strText;
// 	cRetObject.Stringify(strText);

	return true;
}

// 3. 선택된 Component Tree Item을 Highlight하는 함수
bool DmiHpsSignalAnalyzer::SelectComponentTreeItem(DWORD_PTR nId, Json::Object & cInObject)
{
	DmiModelHandler * pcModelHandler = m_mapcModelHandlerMap[nId];
	CHECK_POINTER(pcModelHandler);

	DmiHpsView * pcHpsView = pcModelHandler->GetHpsView();
	CHECK_DWORD_PTR(pcHpsView);

	// 1. Component Tree를 Model Handler에서 가져옴.
	DmiHpsComponentTreePtr pcComponentTree = pcModelHandler->GetComponentTree();

	// 2. 전달받은 ComponentTreeItem
	DmiHpsComponentTreeItem * pcTreeItem = (DmiHpsComponentTreeItem *) cInObject.GetDwordPtr("CompPtr");

	pcHpsView->OnSelection(pcTreeItem);

	return true;
}