#include "StdAfx.h"

#include <hc.h>
#include <HTools.h>
#include <HBaseModel.h>
#include <HEventManager.h>
#include <HBhvBehaviorManager.h>
#include <HEventManager.h>
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
#include "Impl/ViewImpl.h"

#include "3DF.Model.h"

#include "3DF/Window.h"
#include "3DF/Segment.h"
#include "3DF/Impl/SegmentImpl.h"
#include "3DF/Selection.h"
#include "3DF/SelectionSet.h"
#include "3DF/Visibility.h"
#include "3DF/Material.h"
#include "3DF/LineAttribute.h"
#include "3DF/NavigationCube.h"
#include "3DF/3DF.Utility.h"

#include "3DF/3DF.Operator.CameraControl.h"
#include "3DF/Operator.SelectArea.h"

#include "3DF/Facility.AppOptions.h"

#include "3DF/Operator.KinematicTest.h"

#include <Common_Define.h>
#include <Path.h>
#include <WStr.h>

#include "../Signal/Signal.h"

#include <chrono>
#include <thread>

#include "Import/DLL.Interface.h"

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

void H3DF::Canvas::Destruct()
{
	CanvasImpl * pcImpl = new CanvasImpl();
	if (nullptr == pcImpl) {
		assert(false);
	}

	if (nullptr != pcImpl->m_pcModel) {
		delete pcImpl->m_pcModel;
		pcImpl->m_pcModel = nullptr;
	}

	for(auto pcView : pcImpl->m_vpcViewArray) {
		pcView->Destruct();
	}
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

	// 새롭게 View를 생성시키고 입력받은 View정보를 복사한다.
	// 값을 Pointer 형태로 가지고 있어야 처리하기가 편하다.
	View * pcView = new View(cInView);

	ViewImpl * pcViewImpl = (ViewImpl *)pcView->GetImpl();
	if (nullptr == pcViewImpl) {
		DEBUG_RETURN;
	}

	char * pchName = pcViewImpl->m_pchName;

	// setlocale(LC_ALL, "ko_KR.utf8");

	// pcViewImpl에 포함되어 있는 HBaseView를 생성하고 초기화 한다.
	pcViewImpl->Init(pcModel, Utility::ToChar(TheKenel.General.Display.Driver), pchName, nWindowHandle);

	pcCanvasImpl->m_vpcViewArray.push_back(pcView);
	pcCanvasImpl->m_pcFrontView = pcCanvasImpl->m_vpcViewArray.front();
}

void H3DF::Canvas::FileOpen(Json::Object & cInObject, Signal::Delivery & cDelivery)
{
#ifdef DEBUG
	LogManager::CreateLog(LOGMANAGER_3DF_LOG_ID, L"Z://3DF_Log.txt");
	LogManager::SetWriteTimeLog(LOGMANAGER_3DF_LOG_ID, true);
#endif // DEBUG

// 	LogManager::CreateLog(3, L"Z://3DF_Log.txt");
// 	LogManager::SetWriteTimeLog(true);

// 	CString strErrorMessage1;
// 	DLL::H3DF::Interface cInterfaace;
// 	cInterfaace.TDFInitializeA3DLibrary(strErrorMessage1);
/*

	cDelivery.mainFrame.ShowProgress();

	std::thread cFileOpenThread(ThreadFileOpen, std::ref(*this), std::ref(cInObject), std::ref(cDelivery));
	
	if(true == cFileOpenThread.joinable()) {
		cFileOpenThread.join();
	}

	return;
*/

	CString strFilePathName = cInObject.GetString(SKW_FILEPATH);
	if (true == strFilePathName.IsEmpty()) {
		return;
	}

	LogManager::Log(LOGMANAGER_3DF_LOG_ID, L"Open File: " + strFilePathName);


	ViewImpl * pcViewImpl = (ViewImpl *)GetFrontView().GetImpl();
	if (nullptr == pcViewImpl) { DEBUG_RETURN; }

	// 업데이트 강제 중지
	pcViewImpl->GetBaseView()->SetSuppressUpdate(true);
	//pcViewImpl->GetBaseView()->SetSuppressUpdateTick(true);

	const CString EXTENSIONS[] = {
		L"PTS", L"PTX", L"XYZ", // Point Cloud
	};

	CString ext = Path::GetExtension(strFilePathName);
	ext.MakeUpper();

	bool bPointColudData = false;
	for (auto & pre : EXTENSIONS) {
		if (pre == ext) {
			bPointColudData = true;
		}
	}

	bool bHsfFile = false;
	if (false == bPointColudData) {
		if (L"HSF" == ext) {
			bHsfFile = true;
		}
	}

	// Progress dialog 나타내기
	cDelivery.mainFrame.ShowProgress();
	system_clock::time_point cTime1 = system_clock::now();
	cDelivery.progress.SetMessage(strFilePathName);

	if (true == bPointColudData) {
		cDelivery.progress.AddLog(Signal::Progress::Status::Succeed, "Stage 1/2 : Loading point cloud data");
	}
	else if (true == bHsfFile) {
		cDelivery.progress.AddLog(Signal::Progress::Status::Succeed, "Stage 1/2 : Loading stream file");
	}
	else {
		cDelivery.progress.AddLog(Signal::Progress::Status::Succeed, "Stage 1/3 : Import and Tessellation");
	}
	// HC_Define_System_Options("update control=thread=off");

	CString strErrorMessage;

	CanvasImpl * pcCanvasImpl = static_cast<CanvasImpl *>(m_pcImpl);

	SegmentKey cModelSegmentKey = pcCanvasImpl->m_pcModel->GetSegmentKey();

	//----- File을 실제로 읽어 드리는 부분 -----
	if (true == bPointColudData) {
		LogManager::Log(LOGMANAGER_3DF_LOG_ID, L"Load Point Cloud File Start");

		GetFrontView().LoadPointCloudFile(strFilePathName);

		LogManager::Log(LOGMANAGER_3DF_LOG_ID, L"Load Point Cloud File End");
	}
	else if (true == bHsfFile) {
		HC_Open_Segment_By_Key(pcViewImpl->GetBaseView()->GetModel()->GetModelKey()); {
			TK_Status read_status = HTK_Read_Stream_File(strFilePathName, pcViewImpl->GetBaseView()->GetModel()->GetStreamFileTK());
		} HC_Close_Segment();
	}
	else {
		SegmentKey cViewKey(pcViewImpl->GetBaseView()->GetViewKey());
		SegmentKeyImpl::LocalOpen(cViewKey); {
			HC_Set_Driver_Options("eye dome lighting = off");
		} SegmentKeyImpl::LocalClose(cViewKey);

		DLL::H3DF::Interface cInterfaace;
		cInterfaace.TDFImportFile(strFilePathName, cModelSegmentKey, cDelivery, strErrorMessage);
	}

	system_clock::time_point cTime2 = system_clock::now();

	if (false == bPointColudData) {
		cDelivery.progress.AddLog(Signal::Progress::Status::Succeed, L"Stage 3/3 : Performing Initial Update");
	}
	else {
		cDelivery.progress.AddLog(Signal::Progress::Status::Succeed, L"Stage 2/2 : Performing Initial Update");
	}

	pcViewImpl->ViewReady();

	//cModelSegmentKey.ForcedClose();

	// #3DF_Debug: Z://Test.hsf
#ifdef _DEBUG
	 //GetFrontView().SaveHsfFile(L"Z://Test.hsf", this);
#endif
	 GetFrontView().SaveHsfFile(L"Z://Test.hsf", this);

	// HC_Define_System_Options("update control=thread");

// 	pcViewImpl->GetBaseView()->SetSuppressUpdateTick(false);
// 	pcViewImpl->GetBaseView()->SetSuppressUpdate(false);

	bool bHasInitialView = pcViewImpl->GetBaseView()->HasInitialView();

	pcViewImpl->GetBaseView()->GetModel()->SetFileLoadComplete(true);
	pcViewImpl->GetBaseView()->GetModel()->SetFirstFitComplete(true);

	// pcViewImpl->GetBaseView()->SetGeometryChanged();

	if (false == bHasInitialView) {
		pcViewImpl->GetBaseView()->FitWorld();		// fit the camera to the scene extents
		pcViewImpl->GetBaseView()->CameraPositionChanged(true);
	}

	pcViewImpl->GetBaseView()->SetZoomLimit();

	//pcCanvasImpl->m_pcModel->UpdateModelHandedness();

	pcViewImpl->GetBaseView()->SetRenderMode(pcViewImpl->GetBaseView()->GetRenderMode(), true);

	pcViewImpl->GetBaseView()->SetViewDirection(H3DF::ViewDirection::Mode::px_py_pz);

	// HC_Relinquish_Memory();

	//HC_Control_Update_By_Key(pcViewImpl->GetBaseView()->GetViewKey(), "refresh");

	// ExhaustiveUpdate() 내부에서 FoceUpdate를 여러번 호출하기 때문에, Supress 시키도록 한다.
	// pcViewImpl->GetBaseView()->ExhaustiveUpdate();

	//pcViewImpl->GetBaseView()->SetSuppressUpdateTick(false);
	pcViewImpl->GetBaseView()->SetSuppressUpdate(false);

	/*
		HC_Open_Segment_By_Key(pcHoopsView->GetSceneKey()); {
			HC_Set_Visibility("lines = on");
		}HC_Close_Segment();

		pcHoopsView->SetGeometryChanged();
	*/

	pcViewImpl->GetBaseView()->ForceUpdate();

	//pcHoopsView->SetSmoothTransition(true);
	//pcHoopsView->ZoomToExtents();
	// Temp
	//pcHoopsView->ForceUpdate();

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
	system_clock::time_point cTime3 = system_clock::now();
	auto cMilliSec1 = duration_cast<milliseconds>(cTime3 - cTime2);
	CString strMessage;

	if (false == bPointColudData) {
		strMessage.Format(L"Stage 3/3 : Complete [%s]", Utility::GetTimeSpanString(cMilliSec1));
		cDelivery.progress.AddLog(Signal::Progress::Status::Succeed, strMessage);
	}
	else {
		strMessage.Format(L"Stage 2/2 : Complete [%s]", Utility::GetTimeSpanString(cMilliSec1));
		cDelivery.progress.AddLog(Signal::Progress::Status::Succeed, strMessage);
	}

	auto cMilliSec2 = duration_cast<milliseconds>(cTime3 - cTime1);
	strMessage.Format(L"Total Load Time : [%s]", Utility::GetTimeSpanString(cMilliSec2));
	cDelivery.progress.AddLog(Signal::Progress::Status::Succeed, strMessage);

	cDelivery.mainFrame.HideProgress();

	cDelivery.view.SetValidation();

	LogManager::Log(LOGMANAGER_3DF_LOG_ID, L"Update Complete");
}

void H3DF::Canvas::ThreadFileOpen(const Canvas & cCanvas, Json::Object & cInObject, Signal::Delivery & cDelivery)
{
	CString strFilePathName = cInObject.GetString(SKW_FILEPATH);
	if (true == strFilePathName.IsEmpty()) {
		return;
	}

	ViewImpl * pcViewImpl = (ViewImpl *)cCanvas.GetFrontView().GetImpl();
	if (nullptr == pcViewImpl) { DEBUG_RETURN; }

	// 업데이트 강제 중지
	pcViewImpl->GetBaseView()->SetSuppressUpdate(true);
	pcViewImpl->GetBaseView()->SetSuppressUpdateTick(true);

	const CString EXTENSIONS[] = {
		L"PTS", L"PTX", L"XYZ", // Point Cloud
	};

	CString ext = Path::GetExtension(strFilePathName);
	ext.MakeUpper();

	bool bPointColudData = false;
	for (auto & pre : EXTENSIONS) {
		if (pre == ext) {
			bPointColudData = true;
		}
	}

	bool bHsfFile = false;
	if (false == bPointColudData) {
		if (L"HSF" == ext) {
			bHsfFile = true;
		}
	}

	//std::this_thread::sleep();

	// Progress dialog 나타내기
	//cDelivery.mainFrame.ShowProgress();
	system_clock::time_point cTime1 = system_clock::now();
	cDelivery.progress.SetMessage(strFilePathName);

	if (true == bPointColudData) {
		cDelivery.progress.AddLog(Signal::Progress::Status::Succeed, "Stage 1/2 : Loading point cloud data");
	}
	else if(true == bHsfFile) {
		cDelivery.progress.AddLog(Signal::Progress::Status::Succeed, "Stage 1/2 : Loading stream file");
	}
	else {
		cDelivery.progress.AddLog(Signal::Progress::Status::Succeed, "Stage 1/3 : Import and Tessellation");
	}
	// HC_Define_System_Options("update control=thread=off");

	CString strErrorMessage;

	CanvasImpl * pcCanvasImpl = static_cast<CanvasImpl *>(cCanvas.m_pcImpl);

	SegmentKey cModelSegmentKey = pcCanvasImpl->m_pcModel->GetSegmentKey();

	//----- File을 실제로 읽어 드리는 부분 -----
	if (true == bPointColudData)
	{
		//LoadPointCloudFile(strFilePathName, m_pcCanvas);
	}
	else if (true == bHsfFile) {
		HC_Open_Segment_By_Key(pcViewImpl->GetBaseView()->GetModel()->GetModelKey()); {
			TK_Status read_status = HTK_Read_Stream_File(strFilePathName, pcViewImpl->GetBaseView()->GetModel()->GetStreamFileTK());
		} HC_Close_Segment();
	}
	else {
		SegmentKey cViewKey(pcViewImpl->GetBaseView()->GetViewKey());
		SegmentKeyImpl::LocalOpen(cViewKey); {
			HC_Set_Driver_Options("eye dome lighting = off");
		} SegmentKeyImpl::LocalClose(cViewKey);

		DLL::H3DF::Interface cInterfaace;
		cInterfaace.TDFImportFile(strFilePathName, cModelSegmentKey, cDelivery, strErrorMessage);
	}

	system_clock::time_point cTime2 = system_clock::now();

	if (false == bPointColudData) {
		cDelivery.progress.AddLog(Signal::Progress::Status::Succeed, L"Stage 3/3 : Performing Initial Update");
	}
	else {
		cDelivery.progress.AddLog(Signal::Progress::Status::Succeed, L"Stage 2/2 : Performing Initial Update");
	}

	pcViewImpl->ViewReady();

	//cModelSegmentKey.ForcedClose();

	// #3DF_Debug: Z://Test.hsf
#ifdef _DEBUG
	//SaveHsfFile(L"Z://Test.hsf", m_pcCanvas);
#endif

	//HC_Define_System_Options("update control=thread");

 	pcViewImpl->GetBaseView()->SetSuppressUpdateTick(false);
 	pcViewImpl->GetBaseView()->SetSuppressUpdate(false);

	bool bHasInitialView = pcViewImpl->GetBaseView()->HasInitialView();

	pcViewImpl->GetBaseView()->GetModel()->SetFileLoadComplete(true);
	pcViewImpl->GetBaseView()->GetModel()->SetFirstFitComplete(true);

	pcViewImpl->GetBaseView()->SetGeometryChanged();

	if (false == bHasInitialView) {
		pcViewImpl->GetBaseView()->FitWorld();		// fit the camera to the scene extents
		if (pcViewImpl->GetBaseView()->GetModel()->GetContainsDouble()) {
			HC_Convert_Precision(pcViewImpl->GetBaseView()->GetSceneKey(), "double, camera");
		}

		pcViewImpl->GetBaseView()->CameraPositionChanged(true);
	}

	pcViewImpl->GetBaseView()->SetZoomLimit();

	pcCanvasImpl->m_pcModel->UpdateModelHandedness();

	pcViewImpl->GetBaseView()->SetRenderMode(pcViewImpl->GetBaseView()->GetRenderMode(), true);

	pcViewImpl->GetBaseView()->SetViewDirection(H3DF::ViewDirection::Mode::px_py_pz);

	// ExhaustiveUpdate() 내부에서 FoceUpdate를 여러번 호출하기 때문에, Supress 시키도록 한다.
	pcViewImpl->GetBaseView()->ExhaustiveUpdate();

	pcViewImpl->GetBaseView()->SetSuppressUpdateTick(false);
	pcViewImpl->GetBaseView()->SetSuppressUpdate(false);

	pcViewImpl->GetBaseView()->ForceUpdate();

	system_clock::time_point cTime3 = system_clock::now();
	auto cMilliSec1 = duration_cast<milliseconds>(cTime3 - cTime2);
	CString strMessage;

	if (false == bPointColudData) {
		strMessage.Format(L"Stage 3/3 : Complete [%s]", Utility::GetTimeSpanString(cMilliSec1));
		cDelivery.progress.AddLog(Signal::Progress::Status::Succeed, strMessage);
	}
	else {
		strMessage.Format(L"Stage 2/2 : Complete [%s]", Utility::GetTimeSpanString(cMilliSec1));
		cDelivery.progress.AddLog(Signal::Progress::Status::Succeed, strMessage);
	}

	auto cMilliSec2 = duration_cast<milliseconds>(cTime3 - cTime1);
	strMessage.Format(L"Total Load Time : [%s]", Utility::GetTimeSpanString(cMilliSec2));
	cDelivery.progress.AddLog(Signal::Progress::Status::Succeed, strMessage);

	cDelivery.mainFrame.HideProgress();

	cDelivery.view.SetValidation();
}

H3DF::View & H3DF::Canvas::GetFrontView() const
{
	CanvasImpl * pcImpl = static_cast<CanvasImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	DEBUG_VALID(pcImpl->m_pcFrontView);
	return *pcImpl->m_pcFrontView;
/*

	if (pcImpl->m_vpcViewArray.empty()) {
		assert(false);
	}

	return *pcImpl->m_vpcViewArray.front();*/
}

Model & H3DF::Canvas::GetModel() const
{
	CanvasImpl * pcImpl = static_cast<CanvasImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return *pcImpl->m_pcModel;
}

void H3DF::Canvas::Update() const
{
	CanvasImpl * pcImpl = static_cast<CanvasImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	for (const auto pcView : pcImpl->m_vpcViewArray) {
		pcView->Update();
	}
}

void H3DF::Canvas::Update(Json::Object & cInObject) const
{
	CanvasImpl * pcImpl = static_cast<CanvasImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	for (const auto pcView : pcImpl->m_vpcViewArray) {
		pcView->Update(cInObject);
	}
}

void H3DF::Canvas::Update(Json::Object & cInObject, Window::UpdateType eInType, H3DF::Time dInTimeLimit) const
{
	Update(cInObject);
}

void H3DF::Canvas::Resize(int cx, int cy)
{
	View & cView = GetFrontView();
	cView.Resize(cx, cy);
}

//== Keyboard 관련 함수 ==============================================================================
bool H3DF::Canvas::Char(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	return GetFrontView().Char(nChar, nRepCnt, nFlags);
}

//== Command 관련 함수 ===========================================================================

void H3DF::Canvas::CancelCommands()
{
	CanvasImpl * pcImpl = static_cast<CanvasImpl *>(m_pcImpl);
	if (nullptr == pcImpl) {
		DEBUG_RETURN;
	}

	for (const auto pcView : pcImpl->m_vpcViewArray) {
		pcView->CancelCommands();
	}
}

H3DF::Canvas::~Canvas()
{
	return;
	// HC_Relinquish_Memory();
}

bool H3DF::Canvas::KeyboardInput(Json::Object& input)
{
	return GetFrontView().KeyboardInput(input);
}


#undef TheKernel
#undef ThePreset
