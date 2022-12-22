#include "StdAfx.h"

#include <hc.h>
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

#include "3DF.View.h"
#include "3DF.Segment.h"
#include "3DF.Selection.h"

#include "3DF.View.OpCameraPan.h"

#include "../Signal/Signal.h"

USING_3DF_NAMESPACE

//== Camera 관련 Class ==============================================================================

CameraPos::CameraPos() {
	w = 0.0f;
	h = 0.0f;
	bActive = false;
}

View::View(HBaseModel * pcBaseModel, const char * pchAlias, const char * pchDriverType,
	const char * pchInstanceName, void * pcWindowHandle, void * pcColorMap)
	: HBaseView(pcBaseModel, pchAlias, pchDriverType, pchInstanceName, pcWindowHandle, pcColorMap)
{
	m_bOocSelection = false;
	m_bDeepSelection = false;
}

View::~View()
{
	if(nullptr != m_pnSweetenKeyList) {
		delete[] m_pnSweetenKeyList;
	}

	ClearClashList();
	delete_vlist(m_pcClashList);
}

//== Hoops 설정 함수 =================================================================================

void View::Init()
{
	SetSuppressUpdate(true);

	// call base's init function first to get the default HOOPS hierarchy for the view
	HBaseView::Init();
	GetModel()->GetEventManager()->RegisterHandler((HAnimationListener *) this, HAnimationListener::GetType(), HLISTENER_PRIORITY_NORMAL);

	GetModel()->SetStaticModel(true);

	m_pSelection = new SelectionControl(this);
	m_pSelection->Init();
	m_pSelection->SetAllowSubentityDeselection(true);

	// app-specific scene Defaults
	_3DF::SegmentKey cViewSegment(m_ViewKey);
	cViewSegment.GetSelectabilityControl().SetEverything(false);

	// set up some scene defaults
	_3DF::SegmentKey cSceneSegment(m_SceneKey);
	cViewSegment.SetRenderingOptions("no color interpolation, color index interpolation");
	cViewSegment.SetVisibility("lights = (faces = on, edges = off), markers = off, faces=on, edges=off, lines=off, text = on");

	// windowspace (overlay) defaults
	_3DF::SegmentKey cWindowSpaceSegment(m_WindowspaceKey);
	cWindowSpaceSegment.SetColorByIndex("geometry", 3);
	cWindowSpaceSegment.SetColorByIndex("window contrast", 1);
	cWindowSpaceSegment.SetColorByIndex("windows", 1);
	cWindowSpaceSegment.SetVisibility("markers=on");
	cWindowSpaceSegment.SetMarkerSymbol("+");
	cWindowSpaceSegment.GetSelectabilityControl().SetEverything(false);

	SetGpu("Default");

	SetDoubleBuffering(true);

	SetDisplayListType(DisplayListOff);

	// if(!CAppSettings::bLightScaling) CAppSettings::bLightScaling = true
	SetLightScaling(0);

	SetDriverOption();

	SetViewAxis();

	SetViewMode(HViewIsoFrontRightTop);		// fit the camera to the scene extents

	// 배경화면 설정
	COLORREF nWindowBackgroundColor = RGB(59, 68, 83);
	SetWindowBackGroundColor(nWindowBackgroundColor, nWindowBackgroundColor);

	// Setting Framerate Mode
	SetFramerateMode(FramerateOff);
	SetCullingThreshold(2);

	SetPolygonHandednessMode(HandednessLeft);

	SetDefaultOperator();

	// View 설정이 끝나고 나면 
	// File Import 시작
	//ImportExchangeFile(nViewId, strFilePathName);

	SetSuppressUpdate(false);
}

void View::SetGpu(CString strGpu)
{
	char gpu_to_use[256];
	strcpy(gpu_to_use, (char const *) H_UTF8(strGpu).encodedText());
	if(strcmp(gpu_to_use, "Default") != 0)
	{
		SegmentKey cSegment(GetViewKey());
		HC_Set_Driver_Options(H_FORMAT_TEXT("gpu preference = specific = %s", gpu_to_use));
	}
}

void View::SetDriverOption()
{
	char chDriverOpts[MVO_BUFFER_SIZE];

	//if(CAppSettings::bAntiAliasing)
	int nAntialiasingLevel = 4; //  CAppSettings::AntialiasingLevel
	//sprintf(chDriverOpts, "%s, anti-alias=%d ", chDriverOpts, nAntialiasingLevel);
	sprintf(chDriverOpts, "anti-alias=%d ", nAntialiasingLevel);

	HC_Open_Segment_By_Key(GetViewKey()); {
		HC_Set_Driver_Options(chDriverOpts);
		//if(CAppSettings::bAntiAliasing)
		HC_Set_Rendering_Options("anti-alias = (screen = on)");
	} HC_Close_Segment();
}

void View::SetViewAxis()
{
	char text[4096];
	HVector front, top;
	CString strViewAxis = " 1  0  0  0  1  0  0  0  1";
	strcpy(text, H_ASCII_TEXT(strViewAxis));
	sscanf(text, "%f %f %f %f %f %f", &front.x, &front.y, &front.z,
		&top.x, &top.y, &top.z);

	HBaseView::SetViewAxis(&front, &top);

	HBaseView::SetAxisMode(AxisOn);
}

void View::SetWindowBackGroundColor(COLORREF nNewTopColor, COLORREF nNewBottomColor, bool bEmitMessage)
{
	HPoint nWindowTopColor;
	nWindowTopColor.Set(
		static_cast<float>(GetRValue(nNewTopColor)) / 255.0f,
		static_cast<float>(GetGValue(nNewTopColor)) / 255.0f,
		static_cast<float>(GetBValue(nNewTopColor)) / 255.0f);

	HPoint nWindowBottomColor;
	nWindowBottomColor.Set(
		static_cast<float>(GetRValue(nNewBottomColor)) / 255.0f,
		static_cast<float>(GetGValue(nNewBottomColor)) / 255.0f,
		static_cast<float>(GetBValue(nNewBottomColor)) / 255.0f);

	HBaseView::SetWindowColor(nWindowTopColor, nWindowBottomColor, bEmitMessage);
}

//== Mouse 관련 함수 =============================================================================

bool View::LButtonUp(int nFlags, int x, int y)
{
	HEventInfo cEvent(this);
	cEvent.SetPoint(HE_LButtonUp, x, y, MouseMapFlags(nFlags));
	HLISTENER_EVENT(HMouseListener, GetEventManager(), OnLButtonUp(cEvent));
	return true;
}

bool View::LButtonDown(int nFlags, int x, int y)
{
	SetOperator(m_pcCameraManipulate);

	HEventInfo cEvent(this);
	cEvent.SetPoint(HE_LButtonDown, x, y, MouseMapFlags(nFlags));
	HLISTENER_EVENT(HMouseListener, GetEventManager(), OnLButtonDown(cEvent));
	return true;
}

bool View::RButtonUp(int nFlags, int x, int y)
{
	HEventInfo cEvent(this);
	cEvent.SetPoint(HE_RButtonUp, x, y, MouseMapFlags(nFlags));
	HLISTENER_EVENT(HMouseListener, GetEventManager(), OnRButtonUp(cEvent));
	return true;
}

bool View::RButtonDown(int nFlags, int x, int y)
{
	SetOperator(m_pcCameraManipulate);

	HEventInfo cEvent(this);
	cEvent.SetPoint(HE_RButtonDown, x, y, MouseMapFlags(nFlags));
	HLISTENER_EVENT(HMouseListener, GetEventManager(), OnRButtonDown(cEvent));
	return true;
}

bool View::MouseMove(int nFlags, int x, int y)
{
	HEventInfo	cEvent(this);
	cEvent.SetPoint(HE_MouseMove, x, y, MouseMapFlags(nFlags));
	HLISTENER_EVENT(HMouseListener, GetEventManager(), OnMouseMove(cEvent));
	return true;
}

// Mouse Wheel 대응
bool View::MouseWheel(int nFlags, int zDelta, int x, int y, Json::Object & cInObject)
{
	Json::Array & cArray = cInObject.GetArray(SKW_RECT);

	int nLeft = cArray[0]->ToInteger();
	int nTop = cArray[1]->ToInteger();

	HEventInfo	cEvent(this);
	cEvent.SetPoint(HE_MouseWheel, x - nLeft, y - nTop, MouseMapFlags(nFlags));
	cEvent.SetMouseWheelDelta(zDelta);
	HLISTENER_EVENT(HMouseListener, GetEventManager(), OnMouseWheel(cEvent));

	return true;
}

DWORD View::MouseMapFlags(DWORD state)
{
	DWORD nFlag = 0;

	/*map the mfc events state to MVO*/
	if(state & MK_LBUTTON) nFlag |= MVO_LBUTTON;
	if(state & MK_RBUTTON) nFlag |= MVO_RBUTTON;
	if(state & MK_MBUTTON) nFlag |= MVO_MBUTTON;
	if(state & MK_SHIFT) nFlag |= MVO_SHIFT;
	if(state & MK_CONTROL) nFlag |= MVO_CONTROL;

	return nFlag;
}

//== Operator 관련 함수 ==============================================================================

void View::SetDefaultOperator()
{
	m_pcCameraManipulate = new HOpCameraManipulate(this, 0, 1, new HOpCameraOrbit(this), new OpCameraPan(this));
// 		, new HSOpCameraPan(m_pHView),
// 		new HSOpCameraZoom(m_pHView), 0, false))
}

void View::LocalSetOperator(HBaseOperator * pcNewOperator)
{
	HBaseOperator * pcOperator = GetOperator();
	SetOperator(pcNewOperator);
	delete pcOperator;

/*
	HBaseOperator * op = GetSolidView()->GetOperator();
	if(op && strcmp(op->GetName(), "HOpCameraWalk") == 0)
		m_WalkSpeed = ((HOpCameraWalk *) op)->GetSpeed();

	GetSolidView()->SetOperator(NewOperator);
	delete op;

	if(m_pDlgKeyframeEditor)
		m_pDlgKeyframeEditor->UpdateButtonStates();
*/
}

//== Clash 관련 함수 =================================================================================

void View::ClearClashList()
{
	if(nullptr != m_pcClashList)
	{
		START_LIST_ITERATION(ClashItem, m_pcClashList); {
			delete temp;
		}END_LIST_ITERATION(m_pcClashList);
		delete_vlist(m_pcClashList);
	}

	m_pcClashList = new_vlist(malloc, free);
}