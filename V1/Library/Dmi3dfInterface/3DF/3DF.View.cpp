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
#include <HOpCameraZoom.h>
#include <HOpMoveHandle.h>
#include <HUndoManager.h>
#include <hic.h>
#include <HConstantFrameRate.h>

#include "3DF.View.h"
#include "3DF.Segment.h"
#include "3DF.Selection.h"

#include "3DF.Selectability.h"
#include "3DF.Visibility.h"
#include "3DF.MaterialMapping.h"

#include "3DF.OpCameraOrbitSelect.h"
#include "3DF.OpCameraPan.h"
#include "3DF.OpSelectArea.h"

#include "../Signal/Signal.h"

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

	m_pcCameraOrbitSelect = nullptr;
	m_pcCameraPan = nullptr;
	m_pcSelectArea = nullptr;

	m_nCookieSelected = 0;
	m_nCookieDeSelectedAll = 0;
}

View::~View()
{
	HC_Relinquish_Memory();

	if(nullptr != m_pnSweetenKeyList) {
		delete[] m_pnSweetenKeyList;
	}

	if (nullptr != m_pcCameraOrbitSelect) {
		delete m_pcCameraOrbitSelect;
	}

	if (nullptr != m_pcCameraPan) {
		delete m_pcCameraPan;
	}

	if (nullptr != m_pcSelectArea) {
		delete m_pcSelectArea;
	}

// 	ClearClashList();
// 	delete_vlist(m_pcClashList);
}

//== Hoops 설정 함수 =================================================================================

void View::Init()
{
	char chDriverOpts[MVO_BUFFER_SIZE], chRenderingOpts[MVO_BUFFER_SIZE] = { 0 };

	// call base's init function first to get the default HOOPS hierarchy for the view
	HBaseView::Init();

	GetModel()->GetEventManager()->RegisterHandler((HAnimationListener *)this, HAnimationListener::GetType(), HLISTENER_PRIORITY_NORMAL);

	m_pSelection = new SelectionControl(this);
	m_pSelection->Init();
	m_pSelection->SetAllowSubentityDeselection(true);

	m_pMarkupManager = new HMarkupManager(this);

	HC_Open_Segment_By_Key(m_ViewKey); {
		HC_Set_Selectability("everything = off");
	}HC_Close_Segment();

	// set up some scene defaults
	HC_Open_Segment_By_Key(m_SceneKey); {
		HC_Set_Rendering_Options("no color interpolation, color index interpolation");
		HC_Set_Visibility("lights = (faces = on, edges = off), markers = off, faces=on, edges=off, lines=off, text = on");
	}HC_Close_Segment();

	// windowspace (overlay) defaults
	HC_Open_Segment_By_Key(m_WindowspaceKey); {
		HC_Set_Color_By_Index("geometry", 3);
		HC_Set_Color_By_Index("window contrast", 1);
		HC_Set_Color_By_Index("windows", 1);
		HC_Set_Visibility("markers=on");
		HC_Set_Marker_Symbol("+");
		HC_Set_Selectability("off");
	}HC_Close_Segment();

	SetZoomLimit();

	char chGpuToUse[256];
	strcpy(chGpuToUse, "Default");
	if (strcmp(chGpuToUse, "Default") != 0) {
		HC_Open_Segment_By_Key(GetViewKey()); {
			HC_Set_Driver_Options(H_FORMAT_TEXT("gpu preference = specific = %s", chGpuToUse));
		}HC_Close_Segment();
	}

	// do all the setup with no updates
	SetSuppressUpdate(true);

	SetAxisManipulateOperator(new HOpCameraManipulate(this, 0, 1, new HOpCameraOrbit(this), new HOpCameraPan(this), new HOpCameraZoom(this), 0, false));

	HOpMoveHandle * handleoperator = new HOpMoveHandle(this, this, false);
	SetHandleOperator(handleoperator);
	GetEventManager()->RegisterHandler((HObjectManipulationListener *)handleoperator, HObjectManipulationListener::GetType(), HLISTENER_PRIORITY_NORMAL);
	GetEventManager()->RegisterHandler((HJoyStickListener *)this, HJoyStickListener::GetType(), HLISTENER_PRIORITY_NORMAL);

	SetKeyStateCallback(GetKeyState);
	GetModel()->GetBhvBehaviorManager()->SetUpdateCamera(true); // bool CAppSet_tings::UpdateCamera = true;

	long debug_flags = DEBUG_NO_WINDOWS_HOOK | DEBUG_STARTUP_CLEAR_BLACK;

	// use soft ogl if set
	if (false) //bool CAppSet_tings::bDriverForceSoftware = false;
		debug_flags |= DEBUG_FORCE_SOFTWARE;

	sprintf(chDriverOpts, "debug = %u", debug_flags);

	// set anti-aliasing if set
	if (true) // bool CAppSet_tings::bAntiAliasing = true;
		sprintf(chDriverOpts, "%s, anti-alias=%d ", chDriverOpts, 4); // int CAppSet_tings::AntialiasingLevel = 4;

	if (false) // bool CAppSet_tings::bDriverDisplayStats = false;
		sprintf(chDriverOpts, "%s, display stats, display time stats, display memory stats", chDriverOpts);

	if (false) // bool CAppSet_tings::StereoMode = false;
		sprintf(chDriverOpts, "%s, stereo", chDriverOpts);

	sprintf(chDriverOpts, "%s, quick moves preference = %s", chDriverOpts, "Default"); // CString CAppSet_tings::csQuickMovesType = "Default";

	HCLOCALE(sprintf(chDriverOpts,
		"%s, ambient occlusion = (%s, strength = %f, quality = %s), fast silhouette edges = (%s, tolerance = %f, %s heavy exterior)",
		chDriverOpts, (CAppSet_UseFastAmbient ? "on" : "off"), CAppSet_FastAmbientStrength,
		(CAppSet_HQAmbientOcclusion ? "nicest" : "fast"),
		(CAppSet_UseFastSilhouette ? "on" : "off"), CAppSet_FastSilhouetteTolerance,
		(CAppSet_HeavyExteriorSilhouette ? "" : "no")));

	SetDoubleBuffering(CAppSet_DoubleBuffer);

	HC_Open_Segment_By_Key(GetViewKey()); {
		HC_Set_User_Index(H_VIEW_POINTER_INDEX, this);  /* This is used in the event_checker for constant framerate. */
		HC_Set_Driver_Options(chDriverOpts);
		HCLOCALE(sprintf(chDriverOpts, "bloom = (%s, strength=%f, blur=%d, shape=%s)",
			(CAppSet_UseBloom ? "on" : "off"),
			CAppSet_BloomStrength,
			CAppSet_BloomBlur,
			(CAppSet_BloomShape == RadialBloom ? "radial" : "star")));
		HC_Set_Driver_Options(chDriverOpts);
		// antialiasing needs rendering option in addition to driver option
		if (CAppSet_bAntiAliasing) {
			HC_Set_Rendering_Options("anti-alias = (screen = on)");
		}
		HC_Set_Driver_Options("special events, update interrupts");
		HC_Control_Update(".", "redraw everything");
	} HC_Close_Segment();

	bool CAppSet_bLightScaling = true;
	int CAppSet_LightScaleFactor = 100000;;

	if (!CAppSet_bLightScaling) {
		SetLightScaling(0);
	}
	else {
		SetLightScaling(CAppSet_LightScaleFactor / 100000.f);
	}

		
	SetLightFollowsCamera(CAppSet_LightFollowsCamera);
	//SetLightCount(LightCount); //defer until after camera is all set up
	// SetDeepSelectionMode(DeepSelection); OCC를 사용할 때 대응하는 함수
	SetVisibilitySelectionMode(CAppSet_VisibilitySelection);
	SetDynamicHighlighting(CAppSet_DynamicHighlighting);
	SetDetailSelection(CAppSet_DetailSelection);
	SetRelatedSelectionLimit(CAppSet_RelatedSelectionLimit);
	SetTransparentSelectionBoxMode(CAppSet_bUseSelectBox);
	SetRespectSelectionCulling(CAppSet_SelectionRespectCulling);
	SetFastFitWorld(true);
	SetForceFastHiddenLine(CAppSet_HiddenLineMode == FastHiddenLine);
	SetSpritingMode(CAppSet_Spriting);
	SetAllowInteractiveCutGeometry(CAppSet_UpdateCutGeometry);
	SetAllowInteractiveShadows(CAppSet_UpdateShadows);
	SetBackplaneCulling(CAppSet_bBackplaneCulling);
	SetOcclusionCullingMode(CAppSet_OcclusionCulling, true, CAppSet_OcclusionThreshold);

	SetDisplayListType(DisplayListOff);

	if (CAppSet_UseFramerate)
	{
		//if (!pDoc->IsFileReadDeferedForView() || CurrentFramerateMode == FramerateFixed)
		if (CAppSet_CurrentFramerateMode == FramerateFixed)
		{
			SetFramerateMode(CAppSet_CurrentFramerateMode, CAppSet_FramerateTime, CAppSet_MaxThreshold, UINT2bool(CAppSet_UseLods), CAppSet_DetailSteps, CAppSet_HardCutoff);
		}
	}
	else if (CAppSet_CullingThresholdSet)
	{
		SetFramerateMode(FramerateOff);
		SetCullingThreshold(CAppSet_CullingThreshold);
	}
	else
	{
		SetFramerateMode(FramerateOff);
		SetCullingThreshold(0);
	}

	SetSmoothTransition(false);
	SetShadowRenderingMode(CAppSet_ShadowRenderingMode);
	SetViewAxis();
	SetTransparency();

	SetViewMode(HViewIsoFrontRightTop);		// fit the camera to the scene extents
	SetAxisMode(CAppSet_bDisplayAxisTriad ? AxisOn : AxisOff);

	// 배경화면 설정
	COLORREF nWindowBackgroundColor = RGB(59, 68, 83);
	SetWindowBackGroundColor(nWindowBackgroundColor, nWindowBackgroundColor);
	//SetWindowColor(WindowBackgroundTopColor, WindowBackgroundBottomColor);

	HPoint FakeHLRColor;
	FakeHLRColor.Set(
		static_cast<float>(GetRValue(CAppSet_FakeHLRColor)) / 255.0f,
		static_cast<float>(GetGValue(CAppSet_FakeHLRColor)) / 255.0f,
		static_cast<float>(GetBValue(CAppSet_FakeHLRColor)) / 255.0f
	);

	SetFakeHLRColor(FakeHLRColor);
	SetProjMode(CAppSet_ProjectionMode);
	SetSmoothTransition(CAppSet_bSmoothTransition);
	SetSmoothTransitionDuration(0.5f);
	GetUndoManager()->Flush();			//don't care about this initial camera change
	SetDisplayHandlesOnDblClk(!CAppSet_DisableEditing);

	//SetCoordinateSystemHandedness(bWorldHandedness ? HandednessRight : HandednessLeft, true);
	SetHandedness(CAppSet_bWorldHandedness ? HandednessRight : HandednessLeft, true);

	// The state of world today with polygon handedness is
	// 1. Since we are using display lists by default, we want this setting.
	// 2. We will have it only on the view key. If required, model could have it's own
	// Rajesh B (11-Apr-2003)
	SetPolygonHandednessMode(HandednessLeft);

	HPixelRGBA cHighlightSelectColor;
	cHighlightSelectColor.Set(255, 0, 0);
	GetHighlightSelection()->SetSelectionFaceColor(cHighlightSelectColor);
	GetHighlightSelection()->SetSelectionEdgeColor(cHighlightSelectColor);
	GetHighlightSelection()->SetSelectionMarkerColor(cHighlightSelectColor);

	//GetHighlightSelection()->SetGrayScale(true);
	GetHighlightSelection()->SetGrayScale(CAppSet_bGrayScaleSelection);
	GetHighlightSelection()->SetUseDefinedHighlight(CAppSet_bUseDefinedHighlighting);
	GetHighlightSelection()->SetAllowDisplacement(CAppSet_bDisplaceSelection);
	GetHighlightSelection()->UpdateHighlightStyle();

	// set the selection color
	HSelectionSet * sel_set = GetSelection();
	assert(sel_set);
	HPixelRGBA sel_col;
	int sel_alpha = (int)(CAppSet_SelectionColorTransparency * 2.56f);		// settings is a %, scale it to 256
	sel_col.Set(
		GetRValue(CAppSet_PolygonSelectionColor),
		GetGValue(CAppSet_PolygonSelectionColor),
		GetBValue(CAppSet_PolygonSelectionColor),
		(unsigned char)sel_alpha);
	sel_set->SetSelectionFaceColor(sel_col);

	sel_col.Set(
		GetRValue(CAppSet_LineSelectionColor),
		GetGValue(CAppSet_LineSelectionColor),
		GetBValue(CAppSet_LineSelectionColor),
		(unsigned char)sel_alpha);
	sel_set->SetSelectionEdgeColor(sel_col);

	sel_col.Set(
		GetRValue(CAppSet_MarkerSelectionColor),
		GetGValue(CAppSet_MarkerSelectionColor),
		GetBValue(CAppSet_MarkerSelectionColor),
		(unsigned char)sel_alpha);
	sel_set->SetSelectionMarkerColor(sel_col);

	// set markup color and weight
	SetMarkupColor(CAppSet_MarkupColor);

	SetShadowColor(CAppSet_ShadowColor);

	GetMarkupManager()->SetMarkupWeight(CAppSet_MarkupWeight / 100.0f);
	SetShadowResolution(CAppSet_ShadowRes);
	SetShadowBlurring(CAppSet_ShadowBlur);

	// set the color index interpolation settings
	SetColorInterpolation(CAppSet_bCiByValue);
	SetColorIndexInterpolation(CAppSet_bCiByColormapIndex, CAppSet_bCiIsolines);

	GetSelection()->SetGrayScale(CAppSet_bGrayScaleSelection);
	GetSelection()->SetUseDefinedHighlight(CAppSet_bUseDefinedHighlighting);
	GetSelection()->SetAllowDisplacement(CAppSet_bDisplaceSelection);
	GetSelection()->SetHighlightMode(CAppSet_HighlightMode);

	GetHighlightSelection()->SetHighlightMode(CAppSet_HighlightMode);
	GetSelection()->SetHighlightTransparency(CAppSet_TransparencyLevel);

	if (CAppSet_csRefSelType == "Spriting")
		GetSelection()->SetReferenceSelectionType(RefSelSpriting);
	else if (CAppSet_csRefSelType == "Off")
		GetSelection()->SetReferenceSelectionType(RefSelOff);
	else
		GetSelection()->SetReferenceSelectionType(RefSelDefault);

	GetSelection()->UpdateHighlightStyle();

	// set the rendermode
	SetRenderMode(CAppSet_RenderMode, true);

	SetEventCheckerCallback(event_checker);

	//we need to adjust the axis window outside the mvo class as the calculation of the window
	//extents is mfc specific

	// initialize the QueryDialog and AdvancedQueryDialog
/* // Remark
	m_query_dialog = new CQueryDialog();
	m_query_dialog->Create(IDD_QUERYDIALOG);
	m_query_dialog->SetText("No entities currently under cursor.");
	m_advanced_query_dialog = new CQueryDialog();
	m_advanced_query_dialog->Create(IDD_QUERY_ADVANCED_DLG);
	m_advanced_query_dialog->SetText("No entities currently under cursor.");
*/

	
	SetShowCollisions(CAppSet_ShowCollisions);

/* // Remark
	HStreamFileToolkit * tk = GetModel()->GetStreamFileTK();
	if (tk)
	{
		// NOTE: I am setting these opcode handlers here, even if it is not a stream file. Who knows if we are in a
		// collaborative session and someone loads an hsf file - Rajesh B
		// install our custom TK_Initial_View opcode handler so that we set the initial view appropriately
		tk->SetPrewalkHandler(new PartviewerHSFExtras(this));
		tk->SetOpcodeHandler(TKE_View, new PartviewerHSFExtras(this));

		if (!HDB::GetMaterialLibrary())
			HDB::SetMaterialLibrary(new HMaterialLibrary);
		tk->SetOpcodeHandler(TKE_Material, new HTK_PartviewerMaterialHandler(this, HDB::GetMaterialLibrary()));

		// set defaults on stream toolkit
		int sflags = 0;

		if (!CAppSet_tings::bCompressVertices)
			sflags |= TK_Full_Resolution_Vertices;
		else
			tk->SetNumVertexBits(CAppSet_tings::NumVertexBits);

		if (!CAppSet_tings::bCompressNormals)
			sflags |= TK_Full_Resolution_Normals;
		else
			tk->SetNumNormalBits(CAppSet_tings::NumNormalBits);

		if (!CAppSet_tings::bCompressParameters)
			sflags |= TK_Full_Resolution_Parameters;
		else
			tk->SetNumParameterBits(CAppSet_tings::NumParameterBits);


		if (CAppSet_tings::bExportDictionary == true)
			sflags |= TK_Generate_Dictionary;


		if (CAppSet_tings::bEnableInstancing == false)
			sflags |= TK_Disable_Instancing;

		if (CAppSet_tings::bCompressConnectivity == true)
			sflags |= TK_Connectivity_Compression;

		if (CAppSet_tings::bSaveLogFile == true)
			tk->SetLogging(true);
		else
			tk->SetLogging(false);

		// This is a paradox here, the flags passed as argument to Write_Stream function make
		// no sense if we use our own toolkit. These flags need to be set on the toolkit itself
		tk->SetWriteFlags(sflags);

		// Anything <= 0 for export version means use the default.
		if (CAppSet_tings::HsfExportVersion > 0)
			tk->SetTargetVersion(CAppSet_tings::HsfExportVersion);
	}*/

	
// 	if (pDoc->IsFileReadDeferedForView())
// 		LoadFile(pDoc->filename, hmodel->GetStreamFileTK());
// 	else
	{
/*
		SetLineAntialiasing(CAppSet_LineAntialiasing);
		SetTextAntialiasing(CAppSet_TextAntialiasing);

		EmitSegment(GetModel()->GetModelKey(), true);

		SetupViews();

		if (CAppSet_UseFramerate && CAppSet_CurrentFramerateMode == FramerateTarget)
			EnableFrameRate();

		ViewReady();
*/
	}

	SetSceneFont(CAppSet_FontName, CAppSet_FontSize, CAppSet_FontUnits);

	// subscribe to selection events
// 	m_nCookieSelected = SetSignalNotify(HSignalSelected, CSolidHoopsView::signal_selected, this);
// 	m_nCookieDeSelectedAll = SetSignalNotify(HSignalDeSelectedAll, CSolidHoopsView::signal_deselected_all, this);

	// Remark
// 	CSolidHoopsFrame * frame = (CSolidHoopsFrame *)AfxGetMainWnd();
// 	frame->GetBhvToolbar()->m_wndBhvSlider.SetPos(0);


	HC_Open_Segment_By_Key(GetCuttingPlanesKey()); {
		HC_Open_Segment("plane2"); {
			HC_Rotate_Object(90, 0, 0);
		}HC_Close_Segment();

		HC_Open_Segment("plane3"); {
			HC_Rotate_Object(0, 0, 90);
		}HC_Close_Segment();
	}HC_Close_Segment();

	//apply hiding of overlapped text (or not)
	SetHideOverlappedText(CAppSet_bHideOverlappedText);

	HC_Open_Segment_By_Key(GetShadowMapSegmentKey()); {
		if (CAppSet_bShadowMap) {
			sprintf(chRenderingOpts, "shadow map=(on, resolution=%d, samples=%d, %s jitter, %s)",
				CAppSet_nSMResolution, CAppSet_nSMSamples,
				(CAppSet_Jitter ? "" : "no"),
				(CAppSet_ViewDependentShadowMap ? "view dependent" : "view independent"));
		}
		else {
			sprintf(chRenderingOpts, "no shadow map");
		}
		HC_Set_Rendering_Options(chRenderingOpts);
	} HC_Close_Segment();

	HC_Open_Segment_By_Key(GetSceneKey()); {
		HC_Set_Variable_Edge_Weight(H_ASCII_TEXT(CAppSet_LineWeight));
		HC_Set_Variable_Line_Weight(H_ASCII_TEXT(CAppSet_LineWeight));

		//apply stereo mode
		if (CAppSet_StereoMode) {
			HCLOCALE(sprintf(chRenderingOpts, "stereo, stereo separation = %f", CAppSet_StereoSeparation / 10000.f));
			HC_Set_Rendering_Options(chRenderingOpts);
		}

		HCLOCALE(sprintf(chRenderingOpts, "simple shadow = (opacity = %f)", CAppSet_ShadowOpacity));
		HC_Set_Rendering_Options(chRenderingOpts);

		char gooch_color_map[4096];
		HCLOCALE(sprintf(gooch_color_map, "(R=%f G=%f B=%f), (R=%f G=%f B=%f), (R=%f G=%f B=%f), (R=%f G=%f B=%f), (R=%f G=%f B=%f), (R=%f G=%f B=%f)",
			GetRValue(CAppSet_GoochColor1) / 255.0f,
			GetGValue(CAppSet_GoochColor1) / 255.0f,
			GetBValue(CAppSet_GoochColor1) / 255.0f,
			GetRValue(CAppSet_GoochColor2) / 255.0f,
			GetGValue(CAppSet_GoochColor2) / 255.0f,
			GetBValue(CAppSet_GoochColor2) / 255.0f,
			GetRValue(CAppSet_GoochColor3) / 255.0f,
			GetGValue(CAppSet_GoochColor3) / 255.0f,
			GetBValue(CAppSet_GoochColor3) / 255.0f,
			GetRValue(CAppSet_GoochColor4) / 255.0f,
			GetGValue(CAppSet_GoochColor4) / 255.0f,
			GetBValue(CAppSet_GoochColor4) / 255.0f,
			GetRValue(CAppSet_GoochColor5) / 255.0f,
			GetGValue(CAppSet_GoochColor5) / 255.0f,
			GetBValue(CAppSet_GoochColor5) / 255.0f,
			GetRValue(CAppSet_GoochColor6) / 255.0f,
			GetGValue(CAppSet_GoochColor6) / 255.0f,
			GetBValue(CAppSet_GoochColor6) / 255.0f));
		CAppSet_GoochColorMap = gooch_color_map;

		HC_Open_Segment("./overwrite/lights/gooch_color_map_segment"); {
			HC_Set_Color_Map(H_ASCII_TEXT(CAppSet_GoochColorMap));
		}HC_Close_Segment();

		HCLOCALE(sprintf(chRenderingOpts, "gooch options = (diffuse weight = %f, color range=(0.0, %f), color map segment = `./overwrite/lights/gooch_color_map_segment`)",
			CAppSet_GoochWeight, CAppSet_GoochColorHigh));
		HC_Set_Rendering_Options(chRenderingOpts);

		//set camera near limit
		HC_Set_Camera_Near_Limit(CAppSet_NearCameraLimit / 100000.f);

		//Apply curve geometry options
		char curve_opt[4096];
		HCLOCALE(sprintf(curve_opt, "general curve = (budget = %d, continued budget = %d, maximum deviation = %f, maximum angle = %f, maximum length = %f, %s view independent)",
			CAppSet_Budget, CAppSet_ContinuedBudget, CAppSet_MaxDeviation / 10000.f,
			CAppSet_MaxAngle / 10000.f, CAppSet_MaxLength / 10000.f, CAppSet_bViewIndependent ? "" : "no"));

		HC_Set_Rendering_Options(curve_opt);

		SetReflectionPlane(CAppSet_bReflectionPlane, CAppSet_ReflectionOpacity,
			CAppSet_bReflectionFading, CAppSet_ReflectionUseAttenuation,
			CAppSet_ReflectionHither, CAppSet_ReflectionYon,
			CAppSet_ReflectionUseBlur, CAppSet_ReflectionBlur);

		char ambient_color[MVO_BUFFER_SIZE];
		if (CAppSet_HemisphericAmbient)
		{
			float r1, g1, b1, r2, g2, b2;
			char ropt[MVO_BUFFER_SIZE];

			//darken all colors by about 75%
			r1 = (GetRValue(CAppSet_AmbientTopColor) >> 2) / 255.0f;
			g1 = (GetGValue(CAppSet_AmbientTopColor) >> 2) / 255.0f;
			b1 = (GetBValue(CAppSet_AmbientTopColor) >> 2) / 255.0f;

			r2 = (GetRValue(CAppSet_AmbientBottomColor) >> 2) / 255.0f;
			g2 = (GetGValue(CAppSet_AmbientBottomColor) >> 2) / 255.0f;
			b2 = (GetBValue(CAppSet_AmbientBottomColor) >> 2) / 255.0f;

			sprintf(ambient_color, "ambient up=(R=%f G=%f B=%f), ambient down=(R=%f G=%f B=%f)",
				r1, g1, b1, r2, g2, b2);

			HC_Set_Color(ambient_color);
			HC_Define_System_Options("disable ambient material");

			if (CAppSet_UseAmbientUpVector) {
				HCLOCALE(sprintf(ropt, "ambient up vector = (%f, %f, %f)",
					CAppSet_AmbientUpVector.x, CAppSet_AmbientUpVector.y,
					CAppSet_AmbientUpVector.z));
				HC_Set_Rendering_Options(ropt);
			}
			else
			{
				HC_Set_Rendering_Options("no ambient up vector");
			}
		}
		else
		{
			float r1, g1, b1;

			//darken all colors by about 75%
			r1 = (GetRValue(CAppSet_AmbientTopColor) >> 2) / 255.0f;
			g1 = (GetGValue(CAppSet_AmbientTopColor) >> 2) / 255.0f;
			b1 = (GetBValue(CAppSet_AmbientTopColor) >> 2) / 255.0f;

			HCLOCALE(sprintf(ambient_color, "ambient=(R=%f G=%f B=%f)", r1, g1, b1));
			HC_Set_Color(ambient_color);
			HC_Set_Rendering_Options("no ambient up vector");
			HC_Define_System_Options("no disable ambient material");
		}

		//Apply Greeking Settings
		char cGreekingSettings[2048] = "no greeking limit";
		if (CAppSet_bUseGreeking)
		{
			CString csGreekingSettings;
			csGreekingSettings.Format(_T("greeking mode= %s, greeking limit= %f %s"),
				CAppSet_GreekingMode, CAppSet_GreekingLimit / 1000.f,
				CAppSet_GreekingUnits);
			strcpy(cGreekingSettings, H_ASCII_TEXT(csGreekingSettings));
		}
		HC_Set_Text_Font(cGreekingSettings);
	} HC_Close_Segment();

/*  // Remark
	if (CAppSettings::ChildStartMaximized)
	{
		WINDOWPLACEMENT wp;
		this->GetParent()->GetWindowPlacement(&wp);
		wp.showCmd = SW_SHOWMAXIMIZED;
		this->GetParent()->SetWindowPlacement(&wp);
	}
*/

	SetLightCount(CAppSet_LightCount);
	SetViewSelectionLevel(HSelectionLevelSegment);

	SetDefaultOperator();

	//LocalSetOperator(new HOpSelectArea(this));

	// do all the setup with no updates
	SetSuppressUpdate(false);
/*
	SetGpu("Default");

	SetDriverOption();

	SetLightScaling(0);

	SetDisplayListType(DisplayListSegment);
	SetDisplayListMode(true);

	// Setting Framerate Mode
	SetFramerateMode(FramerateOff);
	SetCullingThreshold(2);

	SetBackplaneCulling(false);

	SetProjMode(ProjOrthographic);

	SetTransparency();

	SetSmoothTransition(false);

	GetModel()->GetEventManager()->RegisterHandler((HAnimationListener *)this, HAnimationListener::GetType(), HLISTENER_PRIORITY_NORMAL);

	m_pSelection = new SelectionControl(this);
	m_pSelection->Init();
	m_pSelection->SetAllowSubentityDeselection(true);

	SetSelectOption();

	// Zoom Fit에 이상이 생김.
	// SetFastFitWorld(true);

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

	SetViewAxis();

	SetViewMode(HViewIso);		// fit the camera to the scene extents

	SetHandedness(HandednessRight, true);

	// 배경화면 설정
	COLORREF nWindowBackgroundColor = RGB(59, 68, 83);
	SetWindowBackGroundColor(nWindowBackgroundColor, nWindowBackgroundColor);

	SetPolygonHandednessMode(HandednessLeft);

	SetDefaultOperator();

	// View 설정이 끝나고 나면 
	// File Import 시작
	//ImportExchangeFile(nViewId, strFilePathName);

	GetModel()->SetStaticModel(true);
	GetModel()->SetLMVModel(true);

	bool bFlag = SetHandednessFromModel();

	SetSuppressUpdate(false);*/
}

void View::Init_CUR()
{
	// call base's init function first to get the default HOOPS hierarchy for the view
	HBaseView::Init();

	// do all the setup with no updates
	SetSuppressUpdate(true);
	
	SetGpu("Default");

	SetDriverOption();

	SetLightScaling(0);

	SetDisplayListType(DisplayListSegment);
	SetDisplayListMode(true);

	// Setting Framerate Mode
	SetFramerateMode(FramerateOff);
	SetCullingThreshold(2);

	SetBackplaneCulling(false);
	
	SetProjMode(ProjOrthographic);

	SetTransparency();

	SetSmoothTransition(false);

	GetModel()->GetEventManager()->RegisterHandler((HAnimationListener *) this, HAnimationListener::GetType(), HLISTENER_PRIORITY_NORMAL);
	
	m_pSelection = new SelectionControl(this);
	m_pSelection->Init();
	m_pSelection->SetAllowSubentityDeselection(true);

	SetSelectOption();

	// Zoom Fit에 이상이 생김.
	// SetFastFitWorld(true);

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

	SetViewAxis();

	SetViewMode(HViewIso);		// fit the camera to the scene extents

	SetHandedness(HandednessRight, true);

	// 배경화면 설정
	COLORREF nWindowBackgroundColor = RGB(59, 68, 83);
	SetWindowBackGroundColor(nWindowBackgroundColor, nWindowBackgroundColor);

	SetPolygonHandednessMode(HandednessLeft);

	SetDefaultOperator();

	// View 설정이 끝나고 나면 
	// File Import 시작
	//ImportExchangeFile(nViewId, strFilePathName);

	GetModel()->SetStaticModel(true);
	GetModel()->SetLMVModel(true);

	bool bFlag = SetHandednessFromModel();

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

	long debug_flags = DEBUG_NO_WINDOWS_HOOK | DEBUG_STARTUP_CLEAR_BLACK;
	sprintf(chDriverOpts, "debug = %u", debug_flags);

	//if(CAppSet_tings::bAntiAliasing)
	int nAntialiasingLevel = 4; //  CAppSet_tings::AntialiasingLevel
	sprintf(chDriverOpts, "%s, anti-alias=%d ", chDriverOpts, nAntialiasingLevel);
	//sprintf(chDriverOpts, "anti-alias=%d ", nAntialiasingLevel);

	// CAppSet_tings::csQuickMovesType
	sprintf(chDriverOpts, "%s, quick moves preference = %s", chDriverOpts, "Default");

	HC_Open_Segment_By_Key(GetViewKey()); {
		HC_Set_Driver_Options(chDriverOpts);
		HC_Set_Rendering_Options("anti-alias = (screen = on)");

		HC_Set_Driver_Options("special events, update interrupts");
		HC_Control_Update(".", "redraw everything");

		//HC_Set_Driver_Options("eye dome lighting = (on, strength=1.0)");

	} HC_Close_Segment();
}

// 투명도 적용 방법 설정
void View::SetTransparency()
{
	char chText[4096];
	char chStyle[4096];
	char chSorting[4096];
	char chLayers[4096];
	bool chFastZsort = false;

	strcpy(chStyle, H_ASCII_TEXT(m_cPreference.Transparency.Style));
	strcpy(chSorting, H_ASCII_TEXT(m_cPreference.Transparency.Sorting));
	strcpy(chLayers, H_ASCII_TEXT(m_cPreference.Transparency.DepthPeelingLayers));

	if(strstr(chSorting, "z-sort")) {
		if (strstr(chSorting, "fast")) {
			chFastZsort = true;
		}
		sprintf(chSorting, "z-sort only");
	}

	sprintf(chText, "style = %s, hsr algorithm = %s, depth peeling options = (layers= %s, algorithm=%s), depth writing = %s",
		chStyle, chSorting, chLayers, m_cPreference.Transparency.PixelOIT ? "pixel" : "buffer", m_cPreference.Transparency.DepthWriting == TRUE ? "on" : "off");

	HBaseView::SetTransparency(chText, chFastZsort);
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

void View::SetSelectOption()
{
	SetDetailSelection(false);
	SetRelatedSelectionLimit(0);
	SetTransparentSelectionBoxMode(true);
	SetRespectSelectionCulling(false);
	SetSpritingMode(true);
	SetDynamicHighlighting(true);
	SetViewSelectionLevel(HSelectionLevelSegment);
	//SetVisibilitySelectionMode(false);

	HSelectionSet * pcSelSet = GetSelection();
	assert(pcSelSet);

	pcSelSet->Init();
	pcSelSet->SetAllowSubentityDeselection(true);

	//these need to be done before the regular selection set

	HPixelRGBA cHighlightSelectColor;
	cHighlightSelectColor.Set(0, 255, 0);
	GetHighlightSelection()->SetSelectionFaceColor(cHighlightSelectColor);
	GetHighlightSelection()->SetSelectionLevel(HSelectLevel::HSelectSegment);
	GetHighlightSelection()->SetGrayScale(false); // CAppSet_tings::CAppSet_bGrayScaleSelection
	GetHighlightSelection()->SetUseDefinedHighlight(false); // CAppSet_tings::CAppSet_bUseDefinedHighlighting
	GetHighlightSelection()->SetInvisible(false); // CAppSet_tings::bInvisibleSelection
	GetHighlightSelection()->SetAllowDisplacement(false); // CAppSet_tings::bDisplaceSelection
	GetHighlightSelection()->SetHighlightMode(HighlightQuickmoves);
	GetHighlightSelection()->UpdateHighlightStyle();

	char qm_pref[MVO_BUFFER_SIZE];
	sprintf(qm_pref, "quick moves preference = %s", "Default");// H_ASCII_TEXT(CAppSet_tings::csQuickMovesType));
	HC_Open_Segment_By_Key(GetViewKey());
	HC_Set_Driver_Options(qm_pref);
	HC_Close_Segment();


	// apply the selection color
	//int sel_alpha = (int)(CAppSet_tings::SelectionColorTransparency * 2.56f);		// settings is a %, scale it to 256
	HPixelRGBA cSelectColor;
	cSelectColor.Set(255, 128, 0);
	pcSelSet->SetSelectionFaceColor(cSelectColor);
	pcSelSet->SetSelectionEdgeColor(cSelectColor);
	pcSelSet->SetSelectionMarkerColor(cSelectColor);

	pcSelSet->SetSelectionLevel(HSelectLevel::HSelectSegment);
	pcSelSet->SetGrayScale(false);
 	pcSelSet->SetUseDefinedHighlight(false);
 	pcSelSet->SetAllowDisplacement(false);
 	pcSelSet->SetHighlightMode(HighlightQuickmoves);

 	pcSelSet->SetHighlightTransparency(0.9f);
 	pcSelSet->SetReferenceSelectionType(RefSelSpriting);

	pcSelSet->UpdateHighlightStyle();

	
/*
	pActiveView->SetDeepSelectionMode(CAppSet_tings::DeepSelection);
	pBaseView->SetVisibilitySelectionMode(CAppSet_tings::VisibilitySelection);
	pBaseView->SetTransparentSelectionBoxMode(CAppSet_tings::bUseSelectBox);
*/

/*
	pBaseView->SetDetailSelection(CAppSet_tings::DetailSelection);
	pBaseView->SetRespectSelectionCulling(CAppSet_tings::SelectionRespectCulling);

	pBaseView->SetSuppressUpdate(false);
	pBaseView->Update();
*/
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

bool View::GetKeyState(unsigned int key, int & flags)
{
	unsigned char state[256];
	flags = 0;
	GetKeyboardState(state);
	if (state[VK_LSHIFT] >= 128)
		flags |= MVO_LEFT_SHIFT;
	if (state[VK_SHIFT] >= 128)
		flags |= MVO_SHIFT;
	if (state[VK_RSHIFT] >= 128)
		flags |= MVO_RIGHT_SHIFT;
	if (state[VK_CONTROL] >= 128)
		flags |= MVO_CONTROL;
	if (state[VK_LMENU] >= 128 || state[VK_RMENU] >= 128)
		flags |= MVO_ALT;
	if (state[key] >= 128)
		return true;
	else
		return false;
}

void View::SetMarkupColor(COLORREF new_color, bool emit_message)
{
	UNREFERENCED(emit_message);

	HPoint new_mkp_color;
	new_mkp_color.Set(
		static_cast<float>(GetRValue(new_color)) / 255.0f,
		static_cast<float>(GetGValue(new_color)) / 255.0f,
		static_cast<float>(GetBValue(new_color)) / 255.0f);

	HC_Open_Segment_By_Key(GetMarkupManager()->GetMarkupKey()); {
		HC_Set_Color_By_Value("everything", "RGB", new_mkp_color.x, new_mkp_color.y, new_mkp_color.z);
	}HC_Close_Segment();

	GetMarkupManager()->SetMarkupColor(new_mkp_color);
}

void View::SetShadowColor(COLORREF new_color)
{
	HPoint new_shd_color;
	new_shd_color.Set(
		static_cast<float>(GetRValue(new_color)) / 255.0f,
		static_cast<float>(GetGValue(new_color)) / 255.0f,
		static_cast<float>(GetBValue(new_color)) / 255.0f);
	HBaseView::SetShadowColor(new_shd_color);
}

void View::event_checker(HIC_Rendition const * nr)
{
	MSG msg;
	View * pCurrentView = (View *)HIC_Show_User_Index(nr, H_VIEW_POINTER_INDEX);
	if (pCurrentView)
	{
		int state = GetAsyncKeyState(VK_LBUTTON);
		if (state & 32768)
		{
			pCurrentView->GetConstantFrameRateObject()->InitiateDelay();

			pCurrentView->SetUpdateInterrupted(true);

			HIC_Abort_Update(nr);
		}

		state = GetAsyncKeyState(VK_MBUTTON);
		if (state & 32768)
		{
			HIC_Abort_Update(nr);
			pCurrentView->SetUpdateInterrupted(true);
		}

// 		if (PeekMessage(&msg, pCurrentView->m_hWnd, WM_MOUSEWHEEL, WM_MOUSEWHEEL, PM_NOREMOVE))
// 		{
// 			HIC_Abort_Update(nr);
// 			SetUpdateInterrupted(true);
// 		}
	}
	int state = GetAsyncKeyState(VK_RBUTTON);
	if (state & 32768)
	{
		pCurrentView->GetConstantFrameRateObject()->InitiateDelay();
		pCurrentView->SetUpdateInterrupted(2);
		HIC_Abort_Update(nr);
	}

}

void View::ViewReady()
{
	HBaseModel * hmodel = GetModel();

	SetSuppressUpdate(true);

	SetSplatRendering(BOOL2bool(CAppSet_bSplatRendering));

	SetFastMarkerDrawing(CAppSet_bFastMarkers);

	HC_Open_Segment_By_Key(GetShadowMapSegmentKey()); {
		char opt[MVO_BUFFER_SIZE];

		sprintf(opt, "shadow map=(%s, resolution=%d, samples=%d, %s jitter)",
			CAppSet_bShadowMap ? "on" : "off",
			CAppSet_nSMResolution, CAppSet_nSMSamples, CAppSet_Jitter ? "" : "no");

		HC_Set_Rendering_Options(opt);
	} HC_Close_Segment();

	HC_Open_Segment_By_Key(GetSceneKey()); {
		if (CAppSet_bShadowMap) {
			HC_Set_Visibility("shadows = (emitting, casting, receiving)");
		}
	
		char opt[MVO_BUFFER_SIZE] = "";
		char refl_opt[MVO_BUFFER_SIZE];

		HCLOCALE(sprintf(opt, "simple reflection=(%s, opacity=%f, fading= %s, ",
			CAppSet_bReflectionPlane ? "on" : "off",
			CAppSet_ReflectionOpacity, CAppSet_bReflectionFading ? "on" : "off"));

		if (CAppSet_ReflectionUseAttenuation) {
			HCLOCALE(sprintf(refl_opt, "attenuation = (hither=%f, yon=%f), ",
				CAppSet_ReflectionHither, CAppSet_ReflectionYon));
		}
		else
			sprintf(refl_opt, "no attenuation, ");
		strcat(opt, refl_opt);

		if (CAppSet_ReflectionUseBlur)
			sprintf(refl_opt, "blur=%d)", CAppSet_ReflectionBlur);
		else
			sprintf(refl_opt, "no blur)");
		strcat(opt, refl_opt);

		HC_Set_Rendering_Options(opt);
	} HC_Close_Segment();

	SetShadowLightDirection(CAppSet_UseLightVector, &CAppSet_LightVector);
	SetShadowIgnoresTransparency(CAppSet_IgnoreTransparency);
	SetShadowMode(CAppSet_ShadowMode);
	SetOcclusionCullingMode(CAppSet_OcclusionCulling, true);
	SetLineAntialiasing(CAppSet_LineAntialiasing);
	SetTextAntialiasing(CAppSet_TextAntialiasing);

	SetTransparency();

	//Turn on static model and display lists last, and in that order
	hmodel->SetStaticModel(CAppSet_StaticModel);

	hmodel->SetLMVModel(CAppSet_LMVModel);

	if (CAppSet_bRestoreAnnotations) {
		SetAnnotationResize(true);
	}

	if (DisplayListOff == CAppSet_DisplayList) {
		SetDisplayListMode(false);
	}
	else {
		SetDisplayListType(CAppSet_DisplayList);
		SetDisplayListMode(true);
	}

	// Check whether this file contains layout, if yes, load them (applicable to dwg files, and hsfs saved from
	// dwg files)
/* // Remark
	bool is_layouts = false;
	is_layouts = HDWGLayoutLoadUtility::CheckForLayoutSegmentInThisModel(GetModelKey());
	if (is_layouts)
	{
		m_pLayoutLoadUtility = new HDWGLayoutLoadUtility(m_pHView);
		m_pLayoutLoadUtility->LoadLayoutList();
	}
*/

	SetHandednessFromModel();

	SetSuppressUpdate(false);
}

void View::SetupViews()
{
	SetRenderMode(CAppSet_RenderMode, true);
	SetShadowMode(CAppSet_ShadowMode);
	SetOcclusionCullingMode(CAppSet_OcclusionCulling, true);
}

void View::EnableFrameRate(bool onoff)
{
	int steps = (CAppSet_DynamicAdjustment ? CAppSet_DetailSteps : 0);
	if (onoff) {
		SetFramerateMode(FramerateTarget, CAppSet_FramerateTime, CAppSet_MaxThreshold, UINT2bool(CAppSet_UseLods), steps);
	}
	else {
		SetFramerateMode(FramerateOff);
	}
}

void View::SetSceneFont(CString csFontName, CString csFontSize, CString csFontUnits)
{
	HC_Open_Segment_By_Key(GetSceneKey());

	// first let's query the user's font size settings
	float size = (float)atof(H_ASCII_TEXT(csFontSize));
	if (size < 0)
		size *= -1;

	char cfname[MVO_BUFFER_SIZE];
	sprintf(cfname, "name = \"%s\"", (const char *)H_ASCII_TEXT(csFontName));
	HC_Set_Text_Font(cfname);

	// set the font size via MVO - to propogate it to the hnet clients
	// hnet removed: do we still need to do this?
	char cfsize[MVO_BUFFER_SIZE];
	HCLOCALE(sprintf(cfsize, "%f %s", size, (const char *)H_ASCII_TEXT(csFontUnits)));

	SetFontSize(cfsize, true);

	HC_Close_Segment();
}

bool View::signal_selected(int signal, void * signal_data, void * user_data)
{
	return true;
/*
	UNREFERENCED(signal);
	UNREFERENCED(signal_data);

	return OnSignalSelected();*/
}
/*!
  Receive the MVO event HSignalDeSelectedAll event here and call the appropriate handler
*/
bool View::signal_deselected_all(int signal, void * signal_data, void * user_data)
{
	return true;
/*
	UNREFERENCED(signal);
	UNREFERENCED(signal_data);

	return OnSignalDeSelectedAll();*/
}

bool View::OnSignalSelected()
{
/*

	HandleSpecialSelection();

	if (m_pDlgClashBrowser)
		m_pDlgClashBrowser->CheckDisplaced();

	if (m_pDlgSegmentBrowser)
		m_pDlgSegmentBrowser->ShowSelectedItems();
	if (m_pDlgModelBrowser)
		m_pDlgModelBrowser->ShowSelectedItems();
*/
	return true;
}

/*!
  HSignalDeSelectedAll MVO event handler. Update any dialog bars we have
  \return bool
*/
bool View::OnSignalDeSelectedAll()
{
/*
	if (m_pDlgClashBrowser)
		m_pDlgClashBrowser->CheckDisplaced();
	if (m_pDlgSegmentBrowser)
		m_pDlgSegmentBrowser->ShowSelectedItems();
	if (m_pDlgModelBrowser)
		m_pDlgModelBrowser->ShowSelectedItems();

*/
	return true;
}


//== Command 관련 함수 ===========================================================================
void View::CancelCommands()
{
	DeSelectAll();
}

//== Mouse 관련 함수 =============================================================================
bool View::LButtonDown(int nFlags, int x, int y)
{
	SetDynamicHighlighting(false);

	// Shift & L Button 이벤트는 Area Select
	if (MK_SHIFT & nFlags) {
		SetOperator(m_pcSelectArea);
	}
	else {
		SetOperator(m_pcCameraOrbitSelect);
	}

	HEventInfo cEvent(this);
	cEvent.SetPoint(HE_LButtonDown, x, y, MouseMapFlags(nFlags));
	HLISTENER_EVENT(HMouseListener, GetEventManager(), OnLButtonDown(cEvent));
	return true;
}

bool View::LButtonUp(int nFlags, int x, int y)
{
	SetDynamicHighlighting(true);

	HEventInfo cEvent(this);
	cEvent.SetPoint(HE_LButtonUp, x, y, MouseMapFlags(nFlags));
	HLISTENER_EVENT(HMouseListener, GetEventManager(), OnLButtonUp(cEvent));

	HBaseOperator * op = GetCurrentOperator();

	if (op) {
		if (op->Capture()) {

		}
	}

	return true;
}

bool View::RButtonDown(int nFlags, int x, int y)
{
	SetDynamicHighlighting(false);

	SetOperator(m_pcCameraPan);

	HEventInfo cEvent(this);
	cEvent.SetPoint(HE_RButtonDown, x, y, MouseMapFlags(nFlags));
	HLISTENER_EVENT(HMouseListener, GetEventManager(), OnRButtonDown(cEvent));
	return true;
}

bool View::RButtonUp(int nFlags, int x, int y)
{
	SetDynamicHighlighting(true);

	HEventInfo cEvent(this);
	cEvent.SetPoint(HE_RButtonUp, x, y, MouseMapFlags(nFlags));
	HLISTENER_EVENT(HMouseListener, GetEventManager(), OnRButtonUp(cEvent));
	return true;
}

bool View::MouseMove(int nFlags, int x, int y)
{
	// Control을 누른경우 Face 단위로 선택이 됨.

	if (nFlags & MK_CONTROL) {
		// select on arbitrary subentities(face, edge, or vertex)
		GetHighlightSelection()->SetSelectionLevel(HSelectLevel::HSelectEntity);
	}
	else {
		GetHighlightSelection()->SetSelectionLevel(HSelectLevel::HSelectSegment);
	}

	HEventInfo cEvent(this);
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
	//m_pcCameraManipulate = new HOpCameraManipulate(this, 0, 1, new OpCameraOrbitSelect(this), new OpCameraPan(this));
// 		, new HSOpCameraPan(m_pHView),
// 		new HSOpCameraZoom(m_pHView), 0, false))

	m_pcCameraOrbitSelect = new OpCameraOrbitSelect(this);
	m_pcCameraPan = new OpCameraPan(this);
	m_pcSelectArea = new OpSelectArea(this);

	//LocalSetOperator(m_pcCameraManipulate);
}

void View::LocalSetOperator(HBaseOperator * pcNewOperator)
{
	HBaseOperator * pcOperator = GetOperator();
	SetOperator(pcNewOperator);

	if (nullptr != pcOperator) {
		delete pcOperator;
	}

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

//== Select 관련 함수 ================================================================================

// 선택된 Entity 선택 해제
void View::DeSelectAll()
{
	if (0 < GetSelection()->GetSize()) {
		GetSelection()->DeSelectAll();
		ForceUpdate();
	}
}

void View::SetSubentitySelectLevel()
{
	HSelectionSet * pcSelection = GetSelection();

	if (HSelectLevel::HSelectSubentity != pcSelection->GetSelectionLevel()) {
		pcSelection->DeSelectAll();
		Update();
	}
	pcSelection->SetSelectionLevel(HSelectLevel::HSelectSubentity);
	SetViewSelectionLevel(HSelectionLevelEntity);
	Update();
}

//== Clash 관련 함수 =================================================================================

void View::ClearClashList()
{
	if(nullptr != m_pcClashList)
	{
		delete_vlist(m_pcClashList);
	}

	m_pcClashList = new_vlist(malloc, free);
}