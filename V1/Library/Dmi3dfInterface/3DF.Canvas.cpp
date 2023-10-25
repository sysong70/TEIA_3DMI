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
#include "Private/Canvas.Private.h"

#include "3DF.View.h"
#include "Private/View.Private.h"

#include "3DF/Window.h"
#include "3DF/Segment.h"
#include "3DF/Private/SegmentPrivate.h"
#include "3DF/Selection.h"
#include "3DF/SelectionSet.h"
#include "3DF/Selectability.h"
#include "3DF/Visibility.h"
#include "3DF/Material.h"
#include "3DF/LineAttribute.h"
#include "3DF/NavigationCube.h"
#include "3DF/3DF.Utility.h"

#include "3DF/Operator.CameraSelect.h"
#include "3DF/Operator.SelectArea.h"
#include "3DF/Operator.ObjectSnap.h"


#include "3DF/Facility.AppOptions.h"

#include "3DF/Operator.KinematicTest.h"

#include <Common_Define.h>
#include <Path.h>

#include "../Signal/Signal.h"

#include <chrono>

#include "Import/DLL.Interface.h"

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
	CanvasPrivate * pcImpl = new CanvasPrivate();
	if (nullptr == pcImpl) {
		assert(false);
	}

	m_pcImpl = pcImpl;

	//----- Model 생성 및 초기화 -----
	pcImpl->m_pcModel = new H3DF::Model();
	if (nullptr == pcImpl->m_pcModel) {
		assert(false);
	}

	pcImpl->m_pcModel->Init();
}

H3DF::Canvas::Canvas(Canvas const & cInThat)
{
	m_pcImpl = new CanvasPrivate();
	Set(cInThat);
}

void H3DF::Canvas::Destruct()
{
	CanvasPrivate * pcImpl = new CanvasPrivate();
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
	CanvasPrivate * pcImpl = (CanvasPrivate *)m_pcImpl;
	CanvasPrivate * pcInThatImpl = (CanvasPrivate *)cInThat.m_pcImpl;
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
	CanvasPrivate * pcCanvasImpl = static_cast<CanvasPrivate *>(m_pcImpl);
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

	ViewPrivate * pcViewImpl = (ViewPrivate *)pcView->GetImpl();
	if (nullptr == pcViewImpl) {
		DEBUG_RETURN;
	}

	char * pchName = pcViewImpl->m_pchName;

	// pcViewImpl에 포함되어 있는 HBaseView를 생성하고 초기화 한다.
	pcViewImpl->Init(pcModel, H_ASCII_TEXT(TheKenel.General.Display.Driver), pchName, nWindowHandle);

	pcCanvasImpl->m_vpcViewArray.push_back(pcView);
}

void H3DF::Canvas::FileOpen(Json::Object & cInObject, Signal::Delivery & cInstance)
{
	CString strFilePathName = cInObject.GetString(SKW_FILEPATH);
	if (true == strFilePathName.IsEmpty()) {
		return;
	}

	// Progress dialog 나타내기
	cInstance.mainFrame.ShowProgress();

	system_clock::time_point cTime1 = system_clock::now();

	cInstance.progress.SetMessage(strFilePathName);
	cInstance.progress.AddLog(Signal::Progress::Status::Succeed, "Stage 1/3 : Import and Tessellation");

	ViewPrivate * pcViewImpl = (ViewPrivate *)GetFrontView().GetImpl();
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
	for (auto pre : EXTENSIONS) {
		if (pre == ext) {
			bPointColudData = true;
		}
	}

	// HC_Define_System_Options("update control=thread=off");

	CString strErrorMessage;

	CanvasPrivate * pcCanvasImpl = static_cast<CanvasPrivate *>(m_pcImpl);

	SegmentKey cModelSegmentKey = pcCanvasImpl->m_pcModel->GetSegmentKey();

	if (false == bPointColudData) {
		SegmentKey cViewKey(pcViewImpl->GetBaseView()->GetViewKey());
		SegmentKeyPrivate::LocalOpen(cViewKey);
		HC_Set_Driver_Options("eye dome lighting = off");
		SegmentKeyPrivate::LocalClose(cViewKey);

		DLL::H3DF::Interface cInterfaace;
		cInterfaace.TDFImportFile(strFilePathName, cModelSegmentKey, cInstance, strErrorMessage);
	}
	else {
		// LoadPointCloudFile(strFilePathName, m_pcCanvas);
	}

	system_clock::time_point cTime2 = system_clock::now();

	cInstance.progress.AddLog(Signal::Progress::Status::Succeed, L"Stage 3/3 : Performing Initial Update");

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

	pcViewImpl->GetBaseView()->SetViewMode(H3DF::ViewMode::px_py_pz);

	pcViewImpl->ViewReady();
	pcViewImpl->GetBaseView()->ExhaustiveUpdate();

	pcViewImpl->GetBaseView()->SetSuppressUpdateTick(false);

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
	strMessage.Format(L"Stage 3/3 : Complete [%s]", Utility::GetTimeSpanString(cMilliSec1));
	cInstance.progress.AddLog(Signal::Progress::Status::Succeed, strMessage);

	auto cMilliSec2 = duration_cast<milliseconds>(cTime3 - cTime1);
	strMessage;
	strMessage.Format(L"Total Load Time : [%s]", Utility::GetTimeSpanString(cMilliSec2));
	cInstance.progress.AddLog(Signal::Progress::Status::Succeed, strMessage);

	cInstance.mainFrame.HideProgress();
}

H3DF::View & H3DF::Canvas::GetFrontView() const
{
	CanvasPrivate * pcImpl = static_cast<CanvasPrivate *>(m_pcImpl);
	if (nullptr == pcImpl) {
		assert(false);
	}

	if (pcImpl->m_vpcViewArray.empty()) {
		assert(false);
	}

	return *pcImpl->m_vpcViewArray.front();
}

void H3DF::Canvas::Update(Json::Object & cInObject) const
{
	CanvasPrivate * pcImpl = static_cast<CanvasPrivate *>(m_pcImpl);
	if (nullptr == pcImpl) {
		DEBUG_RETURN;
	}

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

//== Mouse 관련 함수 =============================================================================
bool H3DF::Canvas::LButtonDown(int nFlags, int x, int y)
{
	return GetFrontView().LButtonDown(nFlags, x, y);
}

bool H3DF::Canvas::LButtonUp(int nFlags, int x, int y)
{
	return GetFrontView().LButtonUp(nFlags, x, y);
}

bool H3DF::Canvas::RButtonDown(int nFlags, int x, int y)
{
	return GetFrontView().RButtonDown(nFlags, x, y);
}

bool H3DF::Canvas::RButtonUp(int nFlags, int x, int y)
{
	return GetFrontView().RButtonUp(nFlags, x, y);
}

bool H3DF::Canvas::MouseMove(int nFlags, int x, int y)
{
	return GetFrontView().MouseMove(nFlags, x, y);
}

// Mouse Wheel 대응
bool H3DF::Canvas::MouseWheel(int nFlags, int zDelta, int x, int y, int nLeft, int nTop)
{
	return GetFrontView().MouseWheel(nFlags, zDelta, x, y, nLeft, nTop);
}

//== Keyboard 관련 함수 ==============================================================================
bool H3DF::Canvas::Char(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	return GetFrontView().Char(nChar, nRepCnt, nFlags);
}

//== Command 관련 함수 ===========================================================================

void H3DF::Canvas::CancelCommands()
{
	CanvasPrivate * pcImpl = static_cast<CanvasPrivate *>(m_pcImpl);
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
