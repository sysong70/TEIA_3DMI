#include "StdAfx.h"

#include <hc.h>
#include <HTools.h>
#include <HBaseModel.h>
#include <HBhvBehaviorManager.h>
#include <HMarkupManager.h>
#include <HSharedKey.h>
#include <HUtilityGeomHandle.h>
#include <HEventListener.h>
#include <HOpCameraManipulate.h>
#include <HOpCameraOrbit.h>
#include <HOpCameraPan.h>
#include <HOpCameraZoom.h>
#include <HOpMoveHandle.h>
#include <HUndoManager.h>
#include <hic.h>
#include <HConstantFrameRate.h>

#include "3DF.Canvas.h"
#include "Impl/CanvasImpl.h"

#include "3DF.View.h"
#include "Impl/3DF.View.Impl.h"

#include "3DF.Model.h"

#include "../3DF/Window.h"
#include "../3DF/Impl/WindowImpl.h"
#include "../3DF/Segment.h"
#include "../3DF/Impl/SegmentImpl.h"
#include "../3DF/Selection.h"
#include "../3DF/SelectionSet.h"
#include "../3DF/Visibility.h"
#include "../3DF/Material.h"
#include "../3DF/LineAttribute.h"
#include "../3DF/NavigationCube.h"
#include "../3DF/3DF.Utility.h"

#include "../3DF/Operator.CameraControl.h"
#include "../3DF/Operator.SelectArea.h"

#include "../3DF/Facility.AppOptions.h"

#include "../3DF/Operator.KinematicTest.h"

#include <Common_Define.h>
#include <Path.h>
#include <WStr.h>

#include "../../Signal/Signal.h"

#include <chrono>
#include <thread>

#include "../Import/DLL.Interface.h"

#include "LogManager.h"

#define SEGMENT_TYPE						1
#define ENTITY_TYPE							2
#define SUBENTITY_TYPE						3
#define REGION_TYPE							4


#define	DEBUG_NO_WINDOWS_HOOK				0x00000040
#define DEBUG_STARTUP_CLEAR_BLACK			0x00004000
#define DEBUG_FORCE_SOFTWARE				0x01000000

#define H_VIEW_POINTER_INDEX				4155

#define UINT2bool(__uint__val)  (__uint__val > 0) ?  true: false
#define BOOL2bool(TRUE_Or_FALSE) (( TRUE_Or_FALSE == TRUE ) ? true : false)

using namespace H3DF;
using namespace std::chrono;

#define TheKenel TheAppOptions.Kernel
#define ThePreset TheAppOptions.Preset
#define ColorValue(x) GetRValue(x) / 255.0f, GetGValue(x) / 255.0f, GetBValue(x) / 255.0f
#define ColorRGBA(x, alpha) GetRValue(x), GetGValue(x), GetBValue(x), (unsigned char)alpha

#ifdef _DEBUG
//#define		USED_LOG_MANAGER
#endif

// KEN - 20240229
//#define SAVE_HSF_FILE

//== Camera 관련 Class ==============================================================================

H3DF::CameraPos::CameraPos() {
	w = 0.0f;
	h = 0.0f;
	bActive = false;
}

//== Canvas 관련 Class ==============================================================================

H3DF::Canvas::Canvas()
{
	CanvasImpl * pcImpl = new CanvasImpl();
	if (nullptr == pcImpl) {
		assert(false);
	}

	m_pcImpl = pcImpl;
}

H3DF::Canvas::Canvas(Canvas const & cInThat)
{
	m_pcImpl = new CanvasImpl();
	Set(cInThat);
}

H3DF::Canvas::~Canvas()
{
	// HC_Relinquish_Memory();
}

void H3DF::Canvas::Set(Canvas const & cInThat)
{
	CanvasImpl * pcImpl = (CanvasImpl *)m_pcImpl;
	CanvasImpl * pcInThatImpl = (CanvasImpl *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

Canvas const & H3DF::Canvas::operator = (Canvas const & cInThat)
{
	Set(cInThat);
	return *this;
}

// Attaches a View to this HPS::Canvas using an implicit Layout that covers the whole window.
// 전체 창을 덮는 암시적 레이아웃을 사용하여 이 HPS:Canvas에 View 연결.
// 여기서 BaseView를 생성한다.
void H3DF::Canvas::AttachViewAsLayout(View const & cInView)
{
	CanvasImpl * pcCanvasImpl = static_cast<CanvasImpl *>(m_pcImpl);
	if(nullptr == pcCanvasImpl) {
		DEBUG_RETURN;
	}

	H3DF::Model * pcModel = pcCanvasImpl->m_pcModel;
	if (nullptr == pcModel) {
		DEBUG_RETURN;
	}

	H3DF::WindowHandle nWindowHandle = pcCanvasImpl->m_nInWindowHandle;
	if (0 == nWindowHandle) {
		DEBUG_RETURN;
	}

	ViewImpl * pcViewImpl = (ViewImpl *) cInView.GetImpl();
	if (pcViewImpl == nullptr) {
		DEBUG_RETURN; // 변환 실패 처리
	}

	CStringA strName = pcViewImpl->m_strName;

	// setlocale(LC_ALL, "ko_KR.utf8");

	// View에 필요한 정보를 저장한다. #3DF_View
	pcViewImpl->m_cWindow = pcCanvasImpl->m_cWindowKey;
	pcViewImpl->m_pcModel = pcModel;

	// View의 SegmentKey값에 window정보를 저장한다. 현제 Key값은 Invalid Key 상태임.
	SegmentKeyImpl * pcKeyImpl = (SegmentKeyImpl *)pcViewImpl->m_cKey.GetImpl();
	DEBUG_VALID(pcViewImpl);

	pcKeyImpl->SetWindow(&pcCanvasImpl->m_cWindowKey);

	pcCanvasImpl->m_vcViewArray.push_back(cInView);
}

HWND H3DF::Canvas::GetHwnd()
{
	CanvasImpl * pcCanvasImpl = static_cast<CanvasImpl *>(m_pcImpl);
	DEBUG_VALID(pcCanvasImpl);

	return (HWND)pcCanvasImpl->m_nInWindowHandle;
}

void H3DF::Canvas::SetDelivery(Signal::Delivery & cDelivery, int nViewId)
{
	CanvasImpl * pcImpl = (CanvasImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->SetDelivery(&cDelivery, nViewId);
}

// #Import: File Open
void H3DF::Canvas::FileOpen(CString strFilePathName, H3DF::CADModel & cInCADModel)
{
	CanvasImpl * pcCanvasImpl = (CanvasImpl *)m_pcImpl;
	DEBUG_VALID(pcCanvasImpl);

#ifdef USED_LOG_MANAGER
	CString strExecuteDirectory = LogManager::GetExecuteDirectory();
	LogManager::CreateFolder(strExecuteDirectory + L"\\Log");

	CString strLogFilePathName = strExecuteDirectory + L"\\Log\\3DF.log";
 	LogManager::CreateLog(LOGMANAGER_3DF_LOG_ID, strLogFilePathName);
 	LogManager::SetWriteTimeLog(LOGMANAGER_3DF_LOG_ID, true);
	LogManager::Log(LOGMANAGER_3DF_LOG_ID, L"Open File: " + strFilePathName);

	//m_cA3dTracer.CreateLog(L"D:\\Temp\\A3dXInfo.log");
	//CreateLog(1, L"d:\\Temp\\AssyStruct.log");
#endif
// 	CString strErrorMessage1;
// 	DLL::H3DF::Interface cInterfaace;
// 	cInterfaace.TDFInitializeA3DLibrary(strErrorMessage1);

//	cDelivery.mainFrame.ShowProgress();

/*
	std::thread cFileOpenThread(ThreadFileOpen, std::ref(*this), strFilePathName, std::ref(cInCADModel));
	if (true == cFileOpenThread.joinable()) {
		cFileOpenThread.join();
	}
	
	if(true == cFileOpenThread.joinable()) {
		cFileOpenThread.join();
	}

	return;
*/

// 	CString strFilePathName = cInObject.GetString(SKW_FILEPATH);
// 	if (true == strFilePathName.IsEmpty()) {
// 		return;
// 	}

	// Update Callback 설정
	//pcImpl->SetFinishPictureCallback();

	// 업데이트 강제 중지
	SuppressUpdate(true);

	const CString EXTENSIONS[] = {
		L"PTS", L"PTX", L"XYZ", // Point Cloud
	};

	CString ext = Path::GetExtension(strFilePathName);
	ext.MakeUpper();

	pcCanvasImpl->m_bPointColudData = false;

	for (auto & pre : EXTENSIONS) {
		if (pre == ext) {
			pcCanvasImpl->m_bPointColudData = true;
		}
	}

	bool bHsfFile = false;
	if (false == pcCanvasImpl->m_bPointColudData) {
		if (L"HSF" == ext) {
			bHsfFile = true;
		}
	}

	// Progress dialog 나타내기
	pcCanvasImpl->Delivery().mainFrame.ShowProgress();
	pcCanvasImpl->m_cTimes[0] = system_clock::now();
	pcCanvasImpl->Delivery().progress.SetMessage(strFilePathName);

	if (true == pcCanvasImpl->m_bPointColudData) {
		pcCanvasImpl->Delivery().progress.AddLog(Signal::Progress::Status::Succeed, "Stage 1/2 : Loading point cloud data");
	}
	else if (true == bHsfFile) {
		pcCanvasImpl->Delivery().progress.AddLog(Signal::Progress::Status::Succeed, "Stage 1/2 : Loading stream file");
	}
	else {
		pcCanvasImpl->Delivery().progress.AddLog(Signal::Progress::Status::Succeed, "Stage 1/3 : Import and Tessellation");
	}
	// HC_Define_System_Options("update control=thread=off");

	CString strErrorMessage;

	SegmentKey cModelSegmentKey = GetModel().GetSegmentKey();

	if (H3DF::Type::None == GetWindowKey().Type()) {
		DEBUG_STOP;
	}

	WindowKeyImpl * pcWindowImpl = (WindowKeyImpl *) GetWindowKey().GetImpl();
	DEBUG_VALID(pcWindowImpl);

	BaseView * pcBaseView = pcWindowImpl->GetBaseView();
	DEBUG_VALID(pcBaseView);

	//----- File을 실제로 읽어 드리는 부분 -----
	bool bFileLoadingStatus = true;
	if (true == pcCanvasImpl->m_bPointColudData) {
		LogManager::Log(LOGMANAGER_3DF_LOG_ID, L"Load Point Cloud File Start");

		GetFrontView().LoadPointCloudFile(strFilePathName);

		LogManager::Log(LOGMANAGER_3DF_LOG_ID, L"Load Point Cloud File End");
	}
	else if (true == bHsfFile) {
		HC_Open_Segment_By_Key(pcBaseView->GetModel()->GetModelKey()); {
			TK_Status read_status = HTK_Read_Stream_File(strFilePathName, pcBaseView->GetModel()->GetStreamFileTK());
		} HC_Close_Segment();
	}
	else {
		SegmentKey cViewKey(pcBaseView->GetViewKey());
		SegmentKeyImpl::LocalOpen(cViewKey); {
			HC_Set_Driver_Options("eye dome lighting = off");
		} SegmentKeyImpl::LocalClose(cViewKey);

		DLL::H3DF::Interface cInterfaace;
		bFileLoadingStatus = cInterfaace.TDFImportFile(strFilePathName, cModelSegmentKey, cInCADModel, pcCanvasImpl->Delivery(), strErrorMessage);
	}

	if (false == bFileLoadingStatus) {
		//pcImpl->Delivery().mainFrame.HideProgress();
		pcCanvasImpl->m_bInitUpdate = true;
		return;
	}

	pcCanvasImpl->m_cTimes[1] = system_clock::now();

	if (false == pcCanvasImpl->m_bPointColudData) {
		pcCanvasImpl->Delivery().progress.AddLog(Signal::Progress::Status::Succeed, L"Stage 3/3 : Performing Initial Update");
	}
	else {
		pcCanvasImpl->Delivery().progress.AddLog(Signal::Progress::Status::Succeed, L"Stage 2/2 : Performing Initial Update");
	}

	// pcViewImpl->ViewReady();

	//cModelSegmentKey.ForcedClose();

	// #3DF_Debug: Z://Test.hsf
#ifdef _DEBUG
	 GetFrontView().SaveHsfFile(L"Z://Test.hsf", this);
#endif
	 // GetFrontView().SaveHsfFile(L"Z://Test.hsf", this);

	// HC_Define_System_Options("update control=thread");

// 	pcBaseView->SetSuppressUpdateTick(false);
// 	pcBaseView->SetSuppressUpdate(false);

	bool bHasInitialView = pcBaseView->HasInitialView();

	pcBaseView->GetModel()->SetFileLoadComplete(true);
	pcBaseView->GetModel()->SetFirstFitComplete(true);

	// pcBaseView->SetGeometryChanged();

	if (false == bHasInitialView) {
		pcBaseView->FitWorld();		// fit the camera to the scene extents
		pcBaseView->CameraPositionChanged(true);
	}

	pcBaseView->SetZoomLimit();

	//pcCanvasImpl->m_pcModel->UpdateModelHandedness();

	pcBaseView->SetRenderMode(pcBaseView->GetRenderMode(), true);

	pcBaseView->SetViewDirection(H3DF::ViewDirection::Mode::px_py_pz);
	
	// HOOPS 메모리 정리
	HC_Relinquish_Memory();

	pcBaseView->SetSuppressUpdate(false);

	// pcImpl->Delivery().view.SetValidation()을 통해서 Update가 되므로 별도로 ForceUpdate할 필요가 없음.
	//pcBaseView->ForceUpdate();

/*
	char chBuffer[MVO_BUFFER_SIZE];
	HC_Open_Segment("/");
		HC_Show_Net_Heuristics(chBuffer);
	HC_Close_Segment();

	HC_Open_Segment("/");
		HC_Show_Net_Rendering_Options(chBuffer);
	HC_Close_Segment();

	HC_Open_Segment("/");
		HC_Show_Net_Driver_Options(chBuffer);
	HC_Close_Segment();
*/

//    	
//  
   	pcCanvasImpl->Delivery().view.SetValidation();

	// Update에서 처리
	//pcImpl->Delivery().mainFrame.HideProgress();

	//pcBaseView->ForceUpdate();

	LogManager::Log(LOGMANAGER_3DF_LOG_ID, L"Update Complete");
}

void H3DF::Canvas::ThreadFileOpen(Canvas * pcCanvas, CString strFilePathName, H3DF::CADModel & cInCADModel)
{
	pcCanvas->FileOpen(strFilePathName, cInCADModel);
	return;
}

H3DF::View & H3DF::Canvas::GetFrontView() const
{
	CanvasImpl * pcImpl = static_cast<CanvasImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	if (pcImpl->m_vcViewArray.empty()) {
		DEBUG_STOP;
	}

	return pcImpl->m_vcViewArray.front();
}

H3DF::View & H3DF::Canvas::GetFrontView()
{
	CanvasImpl * pcImpl = static_cast<CanvasImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	if (pcImpl->m_vcViewArray.empty()) {
		DEBUG_STOP;
	}

	return pcImpl->m_vcViewArray.front();
}

WindowKey & H3DF::Canvas::GetWindowKey() const
{
	CanvasImpl * pcImpl = static_cast<CanvasImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->m_cWindowKey;
}

WindowKey & H3DF::Canvas::GetWindowKey()
{
	CanvasImpl * pcImpl = static_cast<CanvasImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->m_cWindowKey;
}

Model & H3DF::Canvas::GetModel() const
{
	CanvasImpl * pcImpl = static_cast<CanvasImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return *pcImpl->m_pcModel;
}

void H3DF::Canvas::Update() const
{
	if (H3DF::Type::None == GetWindowKey().Type()) {
		DEBUG_RETURN;
	}

	WindowKeyImpl * pcWindowImpl = (WindowKeyImpl *)GetWindowKey().GetImpl();
	DEBUG_VALID(pcWindowImpl);

	BaseView * pcBaseView = pcWindowImpl->GetBaseView();
	DEBUG_VALID(pcBaseView);

	if (pcBaseView->GetViewActive() && false == pcBaseView->GetSuppressUpdate())
	{
		HC_Control_Update_By_Key(pcBaseView->GetViewKey(), "redraw everything");
		pcBaseView->GetConstantFrameRateObject()->SetActivityType(GeneralActivity);

		if (false == pcBaseView->GetFirstUpdate()) {
			pcBaseView->ForceUpdate();
		}
		else {
			pcBaseView->Update();
		}
	}

	CanvasImpl * pcCanvasImpl = static_cast<CanvasImpl *>(m_pcImpl);
	DEBUG_VALID(pcCanvasImpl);

	if (false == pcCanvasImpl->m_bInitUpdate) {

		pcCanvasImpl->m_cTimes[2] = system_clock::now();
		auto cMilliSec1 = duration_cast<milliseconds>(pcCanvasImpl->m_cTimes[2] - pcCanvasImpl->m_cTimes[1]);
		CString strMessage;

		if (false == pcCanvasImpl->m_bPointColudData) {
			strMessage.Format(L"Stage 3/3 : Complete [%s]", Utility::GetTimeSpanString(cMilliSec1));
			pcCanvasImpl->Delivery().progress.AddLog(Signal::Progress::Status::Succeed, strMessage);
		}
		else {
			strMessage.Format(L"Stage 2/2 : Complete [%s]", Utility::GetTimeSpanString(cMilliSec1));
			pcCanvasImpl->Delivery().progress.AddLog(Signal::Progress::Status::Succeed, strMessage);
		}

		auto cMilliSec2 = duration_cast<milliseconds>(pcCanvasImpl->m_cTimes[2] - pcCanvasImpl->m_cTimes[0]);
		strMessage.Format(L"Total Load Time : [%s]", Utility::GetTimeSpanString(cMilliSec2));
		pcCanvasImpl->Delivery().progress.AddLog(Signal::Progress::Status::Succeed, strMessage);


		pcCanvasImpl->Delivery().mainFrame.HideProgress();
		pcCanvasImpl->m_bInitUpdate = true;
	}
}

void H3DF::Canvas::Update(Json::Object & cInObject) const
{
	if (H3DF::Type::None == GetWindowKey().Type()) {
		DEBUG_RETURN;
	}

	WindowKeyImpl * pcWindowImpl = (WindowKeyImpl *) GetWindowKey().GetImpl();
	DEBUG_VALID(pcWindowImpl);

	if (false == pcWindowImpl->IsInitNavigationCube()) {
		Json::Array & cArray = cInObject.GetArray(SKW_RECT);
		int nLeft = cArray[0]->ToInteger();
		int nTop = cArray[1]->ToInteger();
		int nRight = cArray[2]->ToInteger();
		int nBottom = cArray[3]->ToInteger();

		pcWindowImpl->InitNavigationCube(nRight, nBottom);
	}

	BaseView * pcBaseView = pcWindowImpl->GetBaseView();
	DEBUG_VALID(pcBaseView);

	if (pcBaseView->GetViewActive() && !pcBaseView->GetSuppressUpdate())
	{
		HC_Control_Update_By_Key(pcBaseView->GetViewKey(), "redraw everything");
		pcBaseView->GetConstantFrameRateObject()->SetActivityType(GeneralActivity);

		//		pcCanvas->GetIntRectangle(&rectangle);
		// 		m_pHView->Notify(HSignalPaint, &rectangle);
		// 		m_pHView->ResetIdleTime();

		if (false == pcBaseView->GetFirstUpdate()) {
			pcBaseView->ForceUpdate();
		}
		else {
			pcBaseView->Update();
		}
	}

	CanvasImpl * pcCanvasImpl = static_cast<CanvasImpl *>(m_pcImpl);
	DEBUG_VALID(pcCanvasImpl);

	if (false == pcCanvasImpl->m_bInitUpdate) {
		pcCanvasImpl->Delivery().mainFrame.HideProgress();
		pcCanvasImpl->m_bInitUpdate = true;
	}
}

void H3DF::Canvas::Update(Json::Object & cInObject, Window::UpdateType eInType, H3DF::Time dInTimeLimit) const
{
	Update(cInObject);
}

SegmentKey H3DF::Canvas::GetConstructionKey()
{
	if (H3DF::Type::None == GetWindowKey().Type()) {
		DEBUG_STOP;
		return {};
	}

	WindowKeyImpl * pcWindowImpl = (WindowKeyImpl *) GetWindowKey().GetImpl();
	DEBUG_VALID(pcWindowImpl);

	BaseView * pcBaseView = pcWindowImpl->GetBaseView();
	DEBUG_VALID(pcBaseView);

	HC_KEY nKey = pcBaseView->GetConstructionKey();

	SegmentKey cConstructionKey = pcBaseView->GetConstructionKey();
	cConstructionKey.GetImpl()->SetType(H3DF::Type::ConstructionKey);

	return cConstructionKey;
}

SegmentKey const H3DF::Canvas::GetConstructionKey() const
{
	if (H3DF::Type::None == GetWindowKey().Type()) {
		DEBUG_STOP;
		return {};
	}

	WindowKeyImpl * pcWindowImpl = (WindowKeyImpl *) GetWindowKey().GetImpl();
	DEBUG_VALID(pcWindowImpl);

	BaseView * pcBaseView = pcWindowImpl->GetBaseView();
	DEBUG_VALID(pcBaseView);

	SegmentKey cConstructionKey = pcBaseView->GetConstructionKey();
	cConstructionKey.GetImpl()->SetType(H3DF::Type::ConstructionKey);

	return cConstructionKey;
}

SegmentKey H3DF::Canvas::GetSceneKey()
{
	if (H3DF::Type::None == GetWindowKey().Type()) {
		DEBUG_STOP;
		return {};
	}

	WindowKeyImpl * pcWindowImpl = (WindowKeyImpl *) GetWindowKey().GetImpl();
	DEBUG_VALID(pcWindowImpl);

	BaseView * pcBaseView = pcWindowImpl->GetBaseView();
	DEBUG_VALID(pcBaseView);

	SegmentKey cGetSceneKey = pcBaseView->GetSceneKey();
	cGetSceneKey.GetImpl()->SetType(H3DF::Type::SceneKey);

	return cGetSceneKey;
}

SegmentKey const H3DF::Canvas::GetSceneKey() const
{
	if (H3DF::Type::None == GetWindowKey().Type()) {
		DEBUG_STOP;
		return {};
	}

	WindowKeyImpl * pcWindowImpl = (WindowKeyImpl *) GetWindowKey().GetImpl();
	DEBUG_VALID(pcWindowImpl);

	BaseView * pcBaseView = pcWindowImpl->GetBaseView();
	DEBUG_VALID(pcBaseView);

	SegmentKey cGetSceneKey = pcBaseView->GetSceneKey();
	cGetSceneKey.GetImpl()->SetType(H3DF::Type::SceneKey);

	return cGetSceneKey;
}

SegmentKey H3DF::Canvas::GetOverwriteKey()
{
	if (H3DF::Type::None == GetWindowKey().Type()) {
		DEBUG_STOP;
		return {};
	}

	WindowKeyImpl * pcWindowImpl = (WindowKeyImpl *) GetWindowKey().GetImpl();
	DEBUG_VALID(pcWindowImpl);

	BaseView * pcBaseView = pcWindowImpl->GetBaseView();
	DEBUG_VALID(pcBaseView);

	SegmentKey cOverwriteKey = pcBaseView->GetOverwriteKey();
	cOverwriteKey.GetImpl()->SetType(H3DF::Type::OverwriteKey);

	return cOverwriteKey;
}

SegmentKey const H3DF::Canvas::GetOverwriteKey() const
{
	if (H3DF::Type::None == GetWindowKey().Type()) {
		DEBUG_STOP;
		return {};
	}

	WindowKeyImpl * pcWindowImpl = (WindowKeyImpl *) GetWindowKey().GetImpl();
	DEBUG_VALID(pcWindowImpl);

	BaseView * pcBaseView = pcWindowImpl->GetBaseView();
	DEBUG_VALID(pcBaseView);

	SegmentKey cOverwriteKey = pcBaseView->GetOverwriteKey();
	cOverwriteKey.GetImpl()->SetType(H3DF::Type::OverwriteKey);

	return cOverwriteKey;
}

void H3DF::Canvas::InvalidateSceneBounding()
{
	if (H3DF::Type::None == GetWindowKey().Type()) {
		DEBUG_STOP;
		return;
	}

	WindowKeyImpl * pcWindowImpl = (WindowKeyImpl *) GetWindowKey().GetImpl();
	DEBUG_VALID(pcWindowImpl);

	BaseView * pcBaseView = pcWindowImpl->GetBaseView();
	DEBUG_VALID(pcBaseView);

	pcBaseView->InvalidateSceneBounding();
}

void H3DF::Canvas::SuppressUpdate(bool bSuppress)
{
	if (H3DF::Type::None == GetWindowKey().Type()) {
		DEBUG_RETURN;
	}

	WindowKeyImpl * pcWindowImpl = (WindowKeyImpl *) GetWindowKey().GetImpl();
	DEBUG_VALID(pcWindowImpl);

	pcWindowImpl->GetBaseView()->SetSuppressUpdate(bSuppress);
}

bool H3DF::Canvas::GetSuppressUpdate()
{
	if (H3DF::Type::None == GetWindowKey().Type()) {
		DEBUG_STOP;
		return false;
	}

	WindowKeyImpl * pcWindowImpl = (WindowKeyImpl *) GetWindowKey().GetImpl();
	DEBUG_VALID(pcWindowImpl);

	return pcWindowImpl->GetBaseView()->GetSuppressUpdate();
}

bool H3DF::Canvas::GetSuppressUpdateTick()
{
	if (H3DF::Type::None == GetWindowKey().Type()) {
		DEBUG_STOP;
		return false;
	}

	WindowKeyImpl * pcWindowImpl = (WindowKeyImpl *) GetWindowKey().GetImpl();
	DEBUG_VALID(pcWindowImpl);

	return pcWindowImpl->GetBaseView()->GetSuppressUpdateTick();
}

void H3DF::Canvas::Resize(int cx, int cy)
{
	WindowKeyImpl * pcWindowImpl = (WindowKeyImpl *) GetWindowKey().GetImpl();
	DEBUG_VALID(pcWindowImpl);

	pcWindowImpl->Resize(cx, cy);
}

//== Keyboard 관련 함수 ==============================================================================
bool H3DF::Canvas::Char(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	return GetFrontView().Char(nChar, nRepCnt, nFlags);
}

bool H3DF::Canvas::KeyboardInput(Json::Object& input)
{
	return GetFrontView().KeyboardInput(input);
}


#undef TheKernel
#undef ThePreset
