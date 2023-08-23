#include "stdafx.h"
#include "3DF.Component.View.h"

#include "3DF.Signal.Interface.h"
#include "3DF.Signal.Manager.h"
#include "3DF.Signal.Connector.h"

#include <Common_Define.h>
#include <Path.h>

#include "3DF/3DF.Canvas.h"

#include "3DF/3DF.Model.h"
#include "3DF/3DF.Segment.h"
#include "3DF/Private/3DF.SegmentPrivate.h"

#include "3DF/3DF.Utility.h"

#include "Import/DLL.3DF.Interface.h"

#include <HIOUtilityHsf.h>
#include <HConstantFrameRate.h>
#include <HIOUtilityPointCloud.h>

#include <chrono>

using namespace H3DF;
using namespace std::chrono;

//== 전달 받은 명령어 분기 =============================================================================

//== View 관련 함수 ==================================================================================

// 1. View 초기화, 전달받은 View Id를 이용해서 초기화 작업 실시하고 전달된 정보에 파일 정보가 있으면 File Open을 실시한다.
void Component::View::Initialize(Json::Object & cInObject, Signal::Delivery & cInstance)
{
	m_pcHoopsModel = new Model();
	if(nullptr == m_pcHoopsModel) {
		DEBUG_RETURN;
	}
	m_pcHoopsModel->Init();

	HWND hWnd = (HWND) cInObject.GetDwordPtr(SKW_HWND);

	// HBaseView 관련 사항을 구성하는 부분
	m_pcCanvas = new H3DF::Canvas(m_pcHoopsModel, reinterpret_cast<void *>(hWnd));

	if(nullptr == m_pcCanvas) {
		DEBUG_RETURN;
	}

	m_pcCanvas->Init();

	CString strFilePathName = cInObject.GetString(SKW_FILEPATH);

 	CString strErrorMessage;

	// Segement를 Model용으로 구성한다.
 	SegmentKey cModelSegmentKey = m_pcHoopsModel->GetSegmentKey();
// 	cModelSegmentKey.ConfigureSegmentModel();

	//cModelSegmentKey.ForcedOpen();

	// Progress dialog 나타내기
	cInstance.mainFrame.ShowProgress();

	system_clock::time_point cTime1 = system_clock::now();
	
	cInstance.progress.SetMessage(strFilePathName);
	cInstance.progress.AddLog(Signal::Progress::Status::Succeed, "Stage 1/3 : Import and Tessellation");

	m_pcCanvas->GetBaseView()->SetSuppressUpdate(true);
	m_pcCanvas->GetBaseView()->SetSuppressUpdateTick(true);

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

	if (false == bPointColudData) {
		SegmentKey cViewKey(m_pcCanvas->GetBaseView()->GetViewKey());
		SegmentKeyPrivate::LocalOpen(cViewKey);
		HC_Set_Driver_Options("eye dome lighting = off");
		SegmentKeyPrivate::LocalClose(cViewKey);

		DLL::H3DF::Interface cInterfaace;
		cInterfaace.TDFImportFile(strFilePathName, cModelSegmentKey, cInstance, strErrorMessage);
	}
	else {
		LoadPointCloudFile(strFilePathName, m_pcCanvas);
	}

	system_clock::time_point cTime2 = system_clock::now();

	cInstance.progress.AddLog(Signal::Progress::Status::Succeed, L"Stage 3/3 : Performing Initial Update");

	//cModelSegmentKey.ForcedClose();

	// #3DF_Debug: Z://Test.hsf
#ifdef _DEBUG
 	SaveHsfFile(L"Z://Test.hsf", m_pcCanvas);
#endif

	//HC_Define_System_Options("update control=thread");

	m_pcCanvas->GetBaseView()->SetSuppressUpdateTick(false);
	m_pcCanvas->GetBaseView()->SetSuppressUpdate(false);

	bool m_has_initial_view = m_pcCanvas->GetBaseView()->HasInitialView();
	m_pcCanvas->GetBaseView()->GetModel()->SetFileLoadComplete(true);
	m_pcCanvas->GetBaseView()->GetModel()->SetFirstFitComplete(true);

	m_pcCanvas->GetBaseView()->SetGeometryChanged();

	if (!m_has_initial_view) {
		m_pcCanvas->GetBaseView()->FitWorld();		// fit the camera to the scene extents
		if (m_pcCanvas->GetBaseView()->GetModel()->GetContainsDouble()) {
			HC_Convert_Precision(m_pcCanvas->GetBaseView()->GetSceneKey(), "double, camera");
		}

		m_pcCanvas->GetBaseView()->CameraPositionChanged(true);
	}

	m_pcCanvas->GetBaseView()->SetZoomLimit();
	m_pcHoopsModel->UpdateModelHandedness();

	m_pcCanvas->GetBaseView()->SetRenderMode(m_pcCanvas->GetBaseView()->GetRenderMode(), true);

	m_pcCanvas->GetBaseView()->SetViewMode(H3DF::ViewMode::px_py_pz);

	m_pcCanvas->ViewReady();
	m_pcCanvas->GetBaseView()->ExhaustiveUpdate();

	m_pcCanvas->GetBaseView()->SetSuppressUpdateTick(false);

/*
	HC_Open_Segment_By_Key(pcHoopsView->GetSceneKey()); {
		HC_Set_Visibility("lines = on");
	}HC_Close_Segment();

	pcHoopsView->SetGeometryChanged();
*/

	m_pcCanvas->GetBaseView()->ForceUpdate();

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

	cInstance.view.SetValidation();
}

void Component::View::Destruct()
{
	if(nullptr != m_pcCanvas) {

		Model * pcModel = (Model *) m_pcCanvas->GetBaseView()->GetModel();

		delete m_pcCanvas;
		m_pcCanvas = nullptr;

		if(nullptr != pcModel) {
			delete pcModel;
		}
	}
}

void Component::View::Paint(Json::Object & cInObject)
{
	if(nullptr == m_pcCanvas) {
		DEBUG_RETURN;
	}

	if (false == m_pcCanvas->IsInitNavigationCube()) {
		Json::Array & cArray = cInObject.GetArray(SKW_RECT);
		int nLeft = cArray[0]->ToInteger();
		int nTop = cArray[1]->ToInteger();
		int nRight = cArray[2]->ToInteger();
		int nBottom = cArray[3]->ToInteger();

		m_pcCanvas->InitNavigationCube(nRight, nBottom);
	}

	//m_pcCanvas->SetClientRect(nWidth, nHeight);

	//m_pcCanvas->SetClientRect(nWidth, nHeight);

	// execute a HOOPS update if we have a valid HBaseView object
	if (m_pcCanvas && m_pcCanvas->GetBaseView()->GetViewActive() && !m_pcCanvas->GetBaseView()->GetSuppressUpdate())
	{
		HC_Control_Update_By_Key(m_pcCanvas->GetBaseView()->GetViewKey(), "redraw everything");
		m_pcCanvas->GetBaseView()->GetConstantFrameRateObject()->SetActivityType(GeneralActivity);

//		pcCanvas->GetIntRectangle(&rectangle);
// 		m_pHView->Notify(HSignalPaint, &rectangle);
// 		m_pHView->ResetIdleTime();

		if(false == m_pcCanvas->GetBaseView()->GetFirstUpdate()) {
			m_pcCanvas->GetBaseView()->ForceUpdate();
		}
		else {
			m_pcCanvas->GetBaseView()->Update();

		}
	}
}

void Component::View::Resize(int x, int y)
{
	assert(m_pcCanvas);

	m_pcCanvas->Resize(x, y);
	m_pcCanvas->GetBaseView()->SetXYSizeOverride(x, y);
	//m_pHView->Notify( HSignalResize );
}

//== Command 관련 함수 ===========================================================================

// 명령어 취소 함수, Select된 Object도 취소됨.
void Component::View::CancelCommands()
{
	m_pcCanvas->CancelCommands();
}

// == Action Function ==============================================================================

// 1. Action Signal 처리 함수
bool Component::View::ExecuteMouseSignal(int nAction, Json::Object & cInObject)
{
	H3DF::Canvas * pcCanvas = m_pcCanvas;

	int nFlag = cInObject.GetInteger(SKW_FLAG);
	int x = cInObject.GetInteger(SKW_X);
	int y = cInObject.GetInteger(SKW_Y);

	switch((Signal::View::Action) nAction)
	{
		case Signal::View::Action::OnMouseMove:
			return MouseMove(pcCanvas, nFlag, x, y);
			break;

		case Signal::View::Action::OnLButtonDown:
			return LButtonDown(pcCanvas, nFlag, x, y);
			break;

		case Signal::View::Action::OnLButtonUp:
			return LButtonUp(pcCanvas, nFlag, x, y);
			break;

		case Signal::View::Action::OnMButtonDown:
			return MButtonDown(pcCanvas, nFlag, x, y);
			break;

		case Signal::View::Action::OnMButtonUp:
			return MButtonUp(pcCanvas, nFlag, x, y);
			break;

		case Signal::View::Action::OnRButtonDown:
			return RButtonDown(pcCanvas, nFlag, x, y);
			break;

		case Signal::View::Action::OnRButtonUp:
			return RButtonUp(pcCanvas, nFlag, x, y);
			break;

		case Signal::View::Action::OnMouseWheel:
		{
			int zDelta = cInObject.GetInteger(SKW_DELTA, -120);
			return MouseWheel(pcCanvas, nFlag, zDelta, x, y, cInObject);
		}
		break;

		default:
			assert(false);
			break;
	}

	return false;
}

// 2. Left Button 처리 함수
bool Component::View::LButtonDown(H3DF::Canvas* pcCanvas, int nFlags, int x, int y)
{
	assert(pcCanvas);
	return pcCanvas->LButtonDown(nFlags, x, y);
}

bool Component::View::LButtonUp(H3DF::Canvas* pcCanvas, int nFlags, int x, int y)
{
	assert(pcCanvas);
	return pcCanvas->LButtonUp(nFlags, x, y);
	/*
		DmiHpsView * pcHpsView = m_mapcModelHandlerMap[nId]->GetHpsView();
		CHECK_DWORD_PTR(pcHpsView);

		HPS::View cView = pcCanvas->GetCanvas().GetFrontView();

		HPS::CameraControl cCameraControl = cView.GetSegmentKey().GetCameraControl();
	*/

	//return pcCanvas->LButtonUp(nFlags, x, y);
}

// 3. Middle Button 처리 함수
bool Component::View::MButtonDown(H3DF::Canvas* pcCanvas, int nFlags, int x, int y)
{
	assert(pcCanvas);
	return true;
	//return pcCanvas->MButtonDown(nFlags, x, y);
}

bool Component::View::MButtonUp(H3DF::Canvas* pcCanvas, int nFlags, int x, int y)
{
	assert(pcCanvas);
	return true;
	//return pcCanvas->MButtonUp(nFlags, x, y);
}

// 4. Right Button 처리 함수
bool Component::View::RButtonUp(H3DF::Canvas* pcCanvas, int nFlags, int x, int y)
{
	assert(pcCanvas);
	return pcCanvas->RButtonUp(nFlags, x, y);
}

bool Component::View::RButtonDown(H3DF::Canvas* pcCanvas, int nFlags, int x, int y)
{
	assert(pcCanvas);
	return pcCanvas->RButtonDown(nFlags, x, y);
}

// 5. Mouse Move 처리 함수
bool Component::View::MouseMove(H3DF::Canvas* pcCanvas, int nFlags, int x, int y)
{
	assert(pcCanvas);
	return pcCanvas->MouseMove(nFlags, x, y);
}

// 6. Mouse Wheel 처리 함수
bool Component::View::MouseWheel(H3DF::Canvas* pcCanvas, int nFlags, int zDelta, int x, int y, Json::Object& cInObject)
{
	assert(pcCanvas);
	return pcCanvas->MouseWheel(nFlags, zDelta, x, y, cInObject);
}

bool Component::View::ExecuteKeyboardSignal(int nAction, Json::Object& cInObject)
{
	if (nullptr == m_pcCanvas) {
		assert(false);
		return false;
	}

	return m_pcCanvas->KeyboardInput(cInObject);
}

void Component::View::SaveHsfFile(CString strFilePathName, Canvas * pcHoopsView)
{
	HIOUtilityHsf cUtilityHsf;

	HC_KEY nModelKey = pcHoopsView->GetBaseView()->GetModelKey();

	HC_Open_Segment_By_Key(nModelKey);

	HOutputHandlerOptions cOptions;
	HStreamFileToolkit * mytool = new HStreamFileToolkit;
	cOptions.ExtendedData(mytool);

	int sflags = 0;
	sflags |= TK_Full_Resolution_Vertices;
	sflags |= TK_Full_Resolution_Normals;
	sflags |= TK_Full_Resolution_Parameters;

	mytool->SetWriteFlags(sflags);

	HFileOutputResult eResult = cUtilityHsf.FileOutputByKey(strFilePathName, nModelKey, &cOptions);

	HC_Close_Segment();

	delete mytool;
}

void Component::View::LoadPointCloudFile(CString strFilePathName, Canvas * pcHoopsView)
{
	SegmentKey cViewKey(pcHoopsView->GetBaseView()->GetViewKey());
	SegmentKeyPrivate::LocalOpen(cViewKey);
	HC_Set_Driver_Options("eye dome lighting = (on, strength=1.0)");
	SegmentKeyPrivate::LocalClose(cViewKey);

	HInputHandlerOptions cOptions;
	cOptions.m_tk = pcHoopsView->GetBaseView()->GetModel()->GetStreamFileTK();
	cOptions.m_pHBaseView = pcHoopsView->GetBaseView();
	
	//cOptions.m_pExtendedData = &cPointCloudOptions;

	//m_point_cloud_options = (HPointCloudOptions *)options->m_pExtendedData;

	SegmentKey cModelKey(pcHoopsView->GetBaseView()->GetModelKey());
	SegmentKey cPointCloudSegment = cModelKey.Subsegment(L"_3dmi_point_cloud");

	HIOUtilityPointCloud cPointCloud;
	
	// 라이브러리를 사용해야 하므로 미리 cPointCloudSegment를 Open하도록 한다.
	SegmentKeyPrivate::LocalOpen(cPointCloudSegment);
	cPointCloud.FileInputByKey(H_ASCII_TEXT(strFilePathName), cPointCloudSegment.KeyValue(), &cOptions);

	HC_UnSet_Marker_Symbol();
	HC_Set_Marker_Size(0.2);

	SegmentKeyPrivate::LocalOpen(cPointCloudSegment);

	// Point Clouse Segment의 하부를 검색해서 색상을 변경함.
	// Library에서 나오는 색상은 기본적으로 Black으로 나옴.
	MaterialMappingKit cMaterialMapping;
	cMaterialMapping.SetVertexColor(RGBAColor(0.75, 0.75, 0.75)); // Gray Color 설정
	Utility::ChangeSubSegmentColor(cPointCloudSegment, cMaterialMapping, true);
}