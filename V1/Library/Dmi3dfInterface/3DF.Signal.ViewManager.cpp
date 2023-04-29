#include "stdafx.h"
#include "3DF.Signal.ViewManager.h"

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
using namespace std::chrono;

USING_3DF_NAMESPACE

//== 전달 받은 명령어 분기 =============================================================================

void ViewManager::ExecuteSignal(Json::Object & cInObject)
{
	int nAction = cInObject.GetInteger(SKW_ACTION);
	int nViewId = cInObject.GetInteger(SKW_VIEWID);

	switch((Signal::View::Action) nAction)
	{
		case Signal::View::Action::OnConstruct:
			break;

			// CView Windows에서 OnInitialize 함수에서 전달 받음.
		case Signal::View::Action::OnInitialize:
			Initialize(nViewId, cInObject);
			break;

		case Signal::View::Action::OnDestruct:
			Destruct(nViewId);
			break;

		case Signal::View::Action::OnPaint:
			Paint(nViewId, cInObject);
			break;

		case Signal::View::Action::OnResize:
		{
			int nX = cInObject.GetInteger(SKW_X);
			int nY = cInObject.GetInteger(SKW_Y);
			Resize(nViewId, nX, nY);
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

		case Signal::View::Action::OnCancel:
			CancelCommands(nViewId);
			break;

		default:
			assert(false);
			break;
	}
}

//== View 관련 함수 ==================================================================================

// 1. View 초기화, 전달받은 View Id를 이용해서 초기화 작업 실시하고 전달된 정보에 파일 정보가 있으면 File Open을 실시한다.
void ViewManager::Initialize(int nViewId, Json::Object & cInObject)
{
	m_pcHoopsModel = new Model();
	if(nullptr == m_pcHoopsModel) {
		DEBUG_RETURN;
	}
	m_pcHoopsModel->Init();

	HWND hWnd = (HWND) cInObject.GetDwordPtr(SKW_HWND);

	Facility::Preference cPreference;

	// HBaseView 관련 사항을 구성하는 부분
	TDF::Canvas * pcCanvas = new TDF::Canvas(m_pcHoopsModel, reinterpret_cast<void *>(hWnd));

	if(nullptr == pcCanvas) {
		DEBUG_RETURN;
	}

	Wrapper().m_mpcCanvas[nViewId] = pcCanvas;
	pcCanvas->SetViewId(nViewId);

	pcCanvas->Init();

	CString strFilePathName = cInObject.GetString(SKW_FILEPATH);

 	CString strErrorMessage;

	// Segement를 Model용으로 구성한다.
	SegmentKey cModelSegmentKey = m_pcHoopsModel->GetSegmentKey();
	cModelSegmentKey.ConfigureSegmentModel();

	//cModelSegmentKey.ForcedOpen();

	
	// Progress dialog 나타내기
	Connector::GetInstance(nViewId).mainFrame.ShowProgress();

	system_clock::time_point cTime1 = system_clock::now();
	
	Connector::GetInstance(nViewId).progress.SetMessage(strFilePathName);
	Connector::GetInstance(nViewId).progress.AddLog(Signal::Progress::Status::Succeed, "Stage 1/3 : Import and Tessellation");

	pcCanvas->GetBaseView()->SetSuppressUpdate(true);
	pcCanvas->GetBaseView()->SetSuppressUpdateTick(true);

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
		SegmentKey cViewKey(pcCanvas->GetBaseView()->GetViewKey());
		SegmentKeyPrivate::LocalOpen(cViewKey);
		HC_Set_Driver_Options("eye dome lighting = off");
		SegmentKeyPrivate::LocalClose(cViewKey);

		DLL::TDF::Interface cInterfaace;
		cInterfaace._3DFImportFile(strFilePathName, cModelSegmentKey, Connector::GetInstance(nViewId), strErrorMessage);
	}
	else {
		LoadPointCloudFile(strFilePathName, pcCanvas);
	}

	system_clock::time_point cTime2 = system_clock::now();

	Connector::GetInstance(nViewId).progress.AddLog(Signal::Progress::Status::Succeed, L"Stage 3/3 : Performing Initial Update");

	//cModelSegmentKey.ForcedClose();

	// #3DF_Debug: Z://Test.hsf
#ifdef _DEBUG
 	SaveHsfFile(L"Z://Test.hsf", pcCanvas);
#endif

	//HC_Define_System_Options("update control=thread");

	pcCanvas->GetBaseView()->SetSuppressUpdateTick(false);
	pcCanvas->GetBaseView()->SetSuppressUpdate(false);

	bool m_has_initial_view = pcCanvas->GetBaseView()->HasInitialView();
	pcCanvas->GetBaseView()->GetModel()->SetFileLoadComplete(true);
	pcCanvas->GetBaseView()->GetModel()->SetFirstFitComplete(true);

	pcCanvas->GetBaseView()->SetGeometryChanged();

	if (!m_has_initial_view) {
		pcCanvas->GetBaseView()->FitWorld();		// fit the camera to the scene extents
		if (pcCanvas->GetBaseView()->GetModel()->GetContainsDouble()) {
			HC_Convert_Precision(pcCanvas->GetBaseView()->GetSceneKey(), "double, camera");
		}

		pcCanvas->GetBaseView()->CameraPositionChanged(true);
	}

	pcCanvas->GetBaseView()->SetZoomLimit();
	m_pcHoopsModel->UpdateModelHandedness();

	pcCanvas->GetBaseView()->SetRenderMode(pcCanvas->GetBaseView()->GetRenderMode(), true);

	pcCanvas->ViewReady();
	pcCanvas->GetBaseView()->ExhaustiveUpdate();

	pcCanvas->GetBaseView()->SetSuppressUpdateTick(false);

/*
	HC_Open_Segment_By_Key(pcHoopsView->GetSceneKey()); {
		HC_Set_Visibility("lines = on");
	}HC_Close_Segment();

	pcHoopsView->SetGeometryChanged();
*/

	pcCanvas->GetBaseView()->ForceUpdate();

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
	Connector::GetInstance(nViewId).progress.AddLog(Signal::Progress::Status::Succeed, strMessage);

	auto cMilliSec2 = duration_cast<milliseconds>(cTime3 - cTime1);
	strMessage;
	strMessage.Format(L"Total Load Time : [%s]", Utility::GetTimeSpanString(cMilliSec2));
	Connector::GetInstance(nViewId).progress.AddLog(Signal::Progress::Status::Succeed, strMessage);

	Connector::GetInstance(nViewId).mainFrame.HideProgress();

	Connector::GetInstance(nViewId).view.SetValidation();

	
}

void ViewManager::Destruct(int nViewId)
{
	Canvas * pcHoopsView = Wrapper().m_mpcCanvas[nViewId];
	if(nullptr != pcHoopsView) {

		Model * pcModel = (Model *) pcHoopsView->GetBaseView()->GetModel();

		delete pcHoopsView;
		Wrapper().m_mpcCanvas[nViewId] = nullptr;

		if(nullptr != pcModel) {
			delete pcModel;
		}
	}
}

void ViewManager::Paint(int nViewId, Json::Object & cInObject)
{
	TDF::Canvas * pcView = Wrapper().m_mpcCanvas[nViewId];
	if(nullptr == pcView) {
		DEBUG_RETURN;
	}

	// execute a HOOPS update if we have a valid HBaseView object
	if (pcView && pcView->GetBaseView()->GetViewActive() && !pcView->GetBaseView()->GetSuppressUpdate())
	{
		HC_Control_Update_By_Key(pcView->GetBaseView()->GetViewKey(), "redraw everything");
		pcView->GetBaseView()->GetConstantFrameRateObject()->SetActivityType(GeneralActivity);

//		pcView->GetIntRectangle(&rectangle);
// 		m_pHView->Notify(HSignalPaint, &rectangle);
// 		m_pHView->ResetIdleTime();

		if(false == pcView->GetBaseView()->GetFirstUpdate()) {
			pcView->GetBaseView()->ForceUpdate();
		}
		else {
			pcView->GetBaseView()->Update();

		}
	}
}

void ViewManager::Resize(int nViewId, int x, int y)
{
	TDF::Canvas * pcView = Wrapper().m_mpcCanvas[nViewId];
	assert(pcView);

	pcView->GetBaseView()->SetXYSizeOverride(x, y);
	//m_pHView->Notify( HSignalResize );
}

//== Command 관련 함수 ===========================================================================

// 명령어 취소 함수, Select된 Object도 취소됨.
void ViewManager::CancelCommands(int nViewId)
{
	TDF::Canvas * pcView = Wrapper().m_mpcCanvas[nViewId];
	pcView->CancelCommands();
}

// == Action Function ==============================================================================

// 1. Action Signal 처리 함수
bool ViewManager::ExecuteMouseSignal(int nViewId, int nAction, Json::Object & cInObject)
{
	TDF::Canvas * pcView = Wrapper().m_mpcCanvas[nViewId];

	int nFlag = cInObject.GetInteger(SKW_FLAG);
	int x = cInObject.GetInteger(SKW_X);
	int y = cInObject.GetInteger(SKW_Y);

	switch((Signal::View::Action) nAction)
	{
		case Signal::View::Action::OnMouseMove:
			return MouseMove(pcView, nFlag, x, y);
			break;

		case Signal::View::Action::OnLButtonDown:
			return LButtonDown(pcView, nFlag, x, y);
			break;

		case Signal::View::Action::OnLButtonUp:
			return LButtonUp(pcView, nFlag, x, y);
			break;

		case Signal::View::Action::OnMButtonDown:
			return MButtonDown(pcView, nFlag, x, y);
			break;

		case Signal::View::Action::OnMButtonUp:
			return MButtonUp(pcView, nFlag, x, y);
			break;

		case Signal::View::Action::OnRButtonDown:
			return RButtonDown(pcView, nFlag, x, y);
			break;

		case Signal::View::Action::OnRButtonUp:
			return RButtonUp(pcView, nFlag, x, y);
			break;

		case Signal::View::Action::OnMouseWheel:
		{
			int zDelta = cInObject.GetInteger(SKW_DELTA, -120);
			return MouseWheel(pcView, nFlag, zDelta, x, y, cInObject);
		}
		break;

		default:
			assert(false);
			break;
	}

	return false;
}

// 2. Left Button 처리 함수
bool ViewManager::LButtonDown(TDF::Canvas * pcView, int nFlags, int x, int y)
{
	assert(pcView);
 	return pcView->LButtonDown(nFlags, x, y);
}

bool ViewManager::LButtonUp(TDF::Canvas * pcView, int nFlags, int x, int y)
{
	assert(pcView);
	return pcView->LButtonUp(nFlags, x, y);
/*
	DmiHpsView * pcHpsView = m_mapcModelHandlerMap[nId]->GetHpsView();
	CHECK_DWORD_PTR(pcHpsView);

	HPS::View cView = pcView->GetCanvas().GetFrontView();

	HPS::CameraControl cCameraControl = cView.GetSegmentKey().GetCameraControl();
*/

	//return pcView->LButtonUp(nFlags, x, y);
}

// 3. Middle Button 처리 함수
bool ViewManager::MButtonDown(TDF::Canvas * pcView, int nFlags, int x, int y)
{
	assert(pcView);
	return true;
	//return pcView->MButtonDown(nFlags, x, y);
}

bool ViewManager::MButtonUp(TDF::Canvas * pcView, int nFlags, int x, int y)
{
	assert(pcView);
	return true;
	//return pcView->MButtonUp(nFlags, x, y);
}

// 4. Right Button 처리 함수
bool ViewManager::RButtonUp(TDF::Canvas * pcView, int nFlags, int x, int y)
{
	assert(pcView);
	return pcView->RButtonUp(nFlags, x, y);
}

bool ViewManager::RButtonDown(TDF::Canvas * pcView, int nFlags, int x, int y)
{
	assert(pcView);
	return pcView->RButtonDown(nFlags, x, y);
}

// 5. Mouse Move 처리 함수
bool ViewManager::MouseMove(TDF::Canvas * pcView, int nFlags, int x, int y)
{
	assert(pcView);
	return pcView->MouseMove(nFlags, x, y);
}

// 6. Mouse Wheel 처리 함수
bool ViewManager::MouseWheel(TDF::Canvas * pcView, int nFlags, int zDelta, int x, int y, Json::Object & cInObject)
{
	assert(pcView);
	return pcView->MouseWheel(nFlags, zDelta, x, y, cInObject);
}

void ViewManager::SaveHsfFile(CString strFilePathName, Canvas * pcHoopsView)
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

void ViewManager::LoadPointCloudFile(CString strFilePathName, Canvas * pcHoopsView)
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