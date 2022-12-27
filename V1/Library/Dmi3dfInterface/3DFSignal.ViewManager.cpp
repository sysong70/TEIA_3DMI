#include "stdafx.h"
#include "3DFSignal.ViewManager.h"

#include "3DFSignal.Interface.h"
#include "3DFSignal.Manager.h"

#include "../Signal/Signal.h"
#include <Common_Define.h>

#include "3DF/3DF.Model.h"
#include "3DF/3DF.View.h"
#include "3DF/3DF.Segment.h"

#include "Import/DLL.3DF.Interface.h"

#include <HIOUtilityHsf.h>
#include <HConstantFrameRate.h>

USING_3DF_NAMESPACE
USING_3DF_SIGNAL_NAMESPACE

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
			//OnDestructView(nViewId);
			break;

		case Signal::View::Action::OnPaint:
			Paint(nViewId, cInObject);
			break;

		case Signal::View::Action::OnResize:
		{
			int nX = cInObject.GetInteger(SKW_X);
			int nY = cInObject.GetInteger(SKW_Y);

			// Resize(nViewId, nX, nY);
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
}

//== View 관련 함수 ==================================================================================

// 1. View 초기화, 전달받은 View Id를 이용해서 초기화 작업 실시하고 전달된 정보에 파일 정보가 있으면 File Open을 실시한다.
void ViewManager::Initialize(int nViewId, Json::Object & cInObject)
{
	m_pcHoopsModel = new Model();
	if(nullptr == m_pcHoopsModel) {
		DEBUG_RETURN;
	}

	HWND hWnd = (HWND) cInObject.GetDwordPtr(SKW_HWND);

	CString strHoopsDriver = L"dx11";

	View * pcHoopsView = new View(m_pcHoopsModel, nullptr, H_ASCII_TEXT(strHoopsDriver), nullptr,
		reinterpret_cast<void *>(hWnd), nullptr); // reinterpret_cast<void *>(pcPalette));

	if(nullptr == pcHoopsView) {
		DEBUG_RETURN;
	}

	Wrapper().m_mpcHoopsView[nViewId] = pcHoopsView;

	pcHoopsView->Init();

	CString strFilePathName = cInObject.GetString(SKW_FILEPATH);

 	CString strErrorMessage;

	// Segement를 Model용으로 구성한다.
	SegmentKey cModelSegmentKey = m_pcHoopsModel->GetSegmentKey();
	cModelSegmentKey.ConfigureSegmentModel();

	DLL::_3DF::Interface cInterfaace;
	cInterfaace._3DFImportFile(strFilePathName, cModelSegmentKey, strErrorMessage);

	SaveHsfFile(L"Z://Test.hsf", pcHoopsView);

	//pcHoopsView->SetSmoothTransition(true);
	pcHoopsView->ZoomToExtents();
	pcHoopsView->ForceUpdate();

	Signal::Delivery delivery;
	delivery.ViewId = nViewId;
	delivery.SetSender(Wrapper().m_pc3dfInterface->GetSignalCallback());
	delivery.view.SetValidation();
}

void ViewManager::Paint(int nViewId, Json::Object & cInObject)
{
	_3DF::View * pcView = Wrapper().m_mpcHoopsView[nViewId];
	if(nullptr == pcView) {
		DEBUG_RETURN;
	}

	// execute a HOOPS update if we have a valid HBaseView object
	if(pcView && pcView->GetViewActive() && !pcView->GetSuppressUpdate())
	{
		HC_Control_Update_By_Key(pcView->GetViewKey(), "redraw everything");
		pcView->GetConstantFrameRateObject()->SetActivityType(GeneralActivity);

//		pcView->GetIntRectangle(&rectangle);
// 		m_pHView->Notify(HSignalPaint, &rectangle);
// 		m_pHView->ResetIdleTime();

		if(false == pcView->GetFirstUpdate()) {
			pcView->ForceUpdate();
		}
		else {
			pcView->Update();
		}

	}
}

// == Mouse Function ===============================================================================

// 1. Mouse Signal 처리 함수
bool ViewManager::ExecuteMouseSignal(int nViewId, int nAction, Json::Object & cInObject)
{
	_3DF::View * pcView = Wrapper().m_mpcHoopsView[nViewId];

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
bool ViewManager::LButtonDown(_3DF::View * pcView, int nFlags, int x, int y)
{
	assert(pcView);
 	return pcView->LButtonDown(nFlags, x, y);
}

bool ViewManager::LButtonUp(_3DF::View * pcView, int nFlags, int x, int y)
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
bool ViewManager::MButtonDown(_3DF::View * pcView, int nFlags, int x, int y)
{
	assert(pcView);
	return true;
	//return pcView->MButtonDown(nFlags, x, y);
}

bool ViewManager::MButtonUp(_3DF::View * pcView, int nFlags, int x, int y)
{
	assert(pcView);
	return true;
	//return pcView->MButtonUp(nFlags, x, y);
}

// 4. Right Button 처리 함수
bool ViewManager::RButtonUp(_3DF::View * pcView, int nFlags, int x, int y)
{
	assert(pcView);
	return pcView->RButtonUp(nFlags, x, y);
}


bool ViewManager::RButtonDown(_3DF::View * pcView, int nFlags, int x, int y)
{
	assert(pcView);
	return pcView->RButtonDown(nFlags, x, y);
}

// 5. Mouse Move 처리 함수
bool ViewManager::MouseMove(_3DF::View * pcView, int nFlags, int x, int y)
{
	assert(pcView);
	return pcView->MouseMove(nFlags, x, y);
}

// 6. Mouse Wheel 처리 함수
bool ViewManager::MouseWheel(_3DF::View * pcView, int nFlags, int zDelta, int x, int y, Json::Object & cInObject)
{
	assert(pcView);
	return pcView->MouseWheel(nFlags, zDelta, x, y, cInObject);
}


void ViewManager::SaveHsfFile(CString strFilePathName, View * pcHoopsView)
{
	HIOUtilityHsf cUtilityHsf;

	HC_KEY nModelKey = pcHoopsView->GetModelKey();

	HOutputHandlerOptions cOptions;
	HFileOutputResult eResult = cUtilityHsf.FileOutputByKey(strFilePathName, nModelKey, &cOptions);

	int i = 0;
}