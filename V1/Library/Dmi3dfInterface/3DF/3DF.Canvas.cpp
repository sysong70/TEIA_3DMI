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

#include "3DF.Canvas.h"
#include "3DF.Window.h"
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

Canvas::Canvas(HBaseModel * pcBaseModel, void * pcWindowHandle)
{
	m_pcBaseView = new HBaseView(pcBaseModel, nullptr, H_ASCII_TEXT(m_cPreference.General.Display.Driver), nullptr,
		reinterpret_cast<void *>(pcWindowHandle), nullptr);

	m_pcWindow = new WindowKey(m_pcBaseView);

	m_bOocSelection = false;
	m_bDeepSelection = false;

	m_pcCameraOrbitSelect = nullptr;
	m_pcCameraPan = nullptr;
	m_pcSelectArea = nullptr;

	m_nCookieSelected = 0;
	m_nCookieDeSelectedAll = 0;
}

Canvas::~Canvas()
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

void Canvas::Init()
{
	char chDriverOpts[MVO_BUFFER_SIZE], chRenderingOpts[MVO_BUFFER_SIZE] = { 0 };

	// call base's init function first to get the default HOOPS hierarchy for the Canvas
	m_pcBaseView->Init();

	m_pcBaseView->GetModel()->GetEventManager()->RegisterHandler((HAnimationListener *)GetBaseView(), HAnimationListener::GetType(), HLISTENER_PRIORITY_NORMAL);

	_3DF::DmiSelectionControl * pcSelection = new _3DF::DmiSelectionControl(m_pcBaseView);
	pcSelection->SetAllowSubentityDeselection(true);

	m_pcBaseView->SetSelection(pcSelection);

	HMarkupManager * pcMarkupManager = new HMarkupManager(m_pcBaseView);
	m_pcBaseView->SetMarkupManager(pcMarkupManager);

	HC_Open_Segment_By_Key(m_pcBaseView->GetViewKey()); {
		HC_Set_Selectability("everything = off");
	} HC_Close_Segment();

	// set up some scene defaults
	HC_Open_Segment_By_Key(m_pcBaseView->GetSceneKey()); {
		HC_Set_Rendering_Options("no color interpolation, color index interpolation");
		HC_Set_Visibility("lights = (faces = on, edges = off), markers = off, faces=on, edges=off, lines=on, text = on");
	} HC_Close_Segment();

	// windowspace (overlay) defaults
	HC_Open_Segment_By_Key(m_pcBaseView->GetWindowspaceKey()); {
		HC_Set_Color_By_Index("geometry", 3);
		HC_Set_Color_By_Index("window contrast", 1);
		HC_Set_Color_By_Index("windows", 1);
		HC_Set_Visibility("markers=on");
		HC_Set_Marker_Symbol("+");
		HC_Set_Selectability("off");
	} HC_Close_Segment();

	char chGpuToUse[256];
	strcpy(chGpuToUse, (char const *)H_UTF8(m_cPreference.General.Display.Gpu).encodedText());
	if (strcmp(chGpuToUse, "Default") != 0) {
		HC_Open_Segment_By_Key(m_pcBaseView->GetViewKey()); {
			HC_Set_Driver_Options(H_FORMAT_TEXT("gpu preference = specific = %s", chGpuToUse));
		} HC_Close_Segment();
	}

	// do all the setup with no updates
	m_pcBaseView->SetSuppressUpdate(true);

	m_pcBaseView->SetAxisManipulateOperator(new HOpCameraManipulate(GetBaseView(), 0, 1, new HOpCameraOrbit(GetBaseView()), new HOpCameraPan(GetBaseView()), new HOpCameraZoom(GetBaseView()), 0, false));

	HOpMoveHandle * handleoperator = new HOpMoveHandle(GetBaseView(), GetBaseView(), false);
	m_pcBaseView->SetHandleOperator(handleoperator);

	m_pcBaseView->GetEventManager()->RegisterHandler((HObjectManipulationListener *)handleoperator, HObjectManipulationListener::GetType(), HLISTENER_PRIORITY_NORMAL);
	m_pcBaseView->GetEventManager()->RegisterHandler((HJoyStickListener *)GetBaseView(), HJoyStickListener::GetType(), HLISTENER_PRIORITY_NORMAL);

	m_pcBaseView->SetKeyStateCallback(GetKeyState);
	m_pcBaseView->GetModel()->GetBhvBehaviorManager()->SetUpdateCamera(m_cPreference.Interaction.Animation.UpdateCamera);

	long nDebugFlags = DEBUG_NO_WINDOWS_HOOK | DEBUG_STARTUP_CLEAR_BLACK;

	// use soft ogl if set
	if (true == m_cPreference.General.Display.DriverForceSoftware) {
		nDebugFlags |= DEBUG_FORCE_SOFTWARE;
	}

	sprintf(chDriverOpts, "debug = %u", nDebugFlags);

	// set anti-aliasing if set
	if (true == m_cPreference.Appearance.AntiAliasing.Use) {
		sprintf(chDriverOpts, "%s, anti-alias=%d ", chDriverOpts, m_cPreference.Appearance.AntiAliasing.Level);
	}

	if (true == m_cPreference.General.Display.DriverDisplayStats) {
		sprintf(chDriverOpts, "%s, display stats, display time stats, display memory stats", chDriverOpts);
	}

	if (true == m_cPreference.General.Display.StereoMode) {
		sprintf(chDriverOpts, "%s, stereo", chDriverOpts);
	}

	sprintf(chDriverOpts, "%s, quick moves preference = %s", chDriverOpts, H_ASCII_TEXT(m_cPreference.Selection.Highlight.QuickMovesType));


	HCLOCALE(sprintf(chDriverOpts,
		"%s, ambient occlusion = (%s, strength = %f, quality = %s), fast silhouette edges = (%s, tolerance = %f, %s heavy exterior)", chDriverOpts, 
		(m_cPreference.Effects.FrameBuffer.UseAmbient ? "on" : "off"), m_cPreference.Effects.FrameBuffer.AmbientStrength,
		(m_cPreference.Effects.FrameBuffer.HighQualityAmbient ? "nicest" : "fast"),
		(m_cPreference.Effects.FrameBuffer.UseFastSilhouette ? "on" : "off"), m_cPreference.Effects.FrameBuffer.FastSilhouetteTolerance,
		(m_cPreference.Effects.FrameBuffer.HeavyExteriorSilhouette ? "" : "no")));

	m_pcBaseView->SetDoubleBuffering(m_cPreference.General.Display.DoubleBuffer);


	HC_Open_Segment_By_Key(m_pcBaseView->GetViewKey()); {
		HC_Set_User_Index(H_VIEW_POINTER_INDEX, GetBaseView());  // This is used in the event_checker for constant framerate.
		HC_Set_Driver_Options(chDriverOpts);
		HCLOCALE(sprintf(chDriverOpts, "bloom = (%s, strength=%f, blur=%d, shape=%s)",
			(m_cPreference.Lighting.Bloom.Use ? "on" : "off"),
			m_cPreference.Lighting.Bloom.Strength,
			m_cPreference.Lighting.Bloom.Blur,
			(m_cPreference.Lighting.Bloom.Shape == RadialBloom ? "radial" : "star")));
		HC_Set_Driver_Options(chDriverOpts);
		// antialiasing needs rendering option in addition to driver option
		if (true == m_cPreference.Appearance.AntiAliasing.Use) {
			HC_Set_Rendering_Options("anti-alias = (screen = on)");
		}
		HC_Set_Driver_Options("special events, update interrupts");
		HC_Control_Update(".", "redraw everything");
	} HC_Close_Segment();


	int CAppSet_LightScaleFactor = 100000;;

	if (false == m_cPreference.Lighting.Light.Scaling) {
		m_pcBaseView->SetLightScaling(0);
	}
	else {
		m_pcBaseView->SetLightScaling(m_cPreference.Lighting.Light.ScaleFactor / 100000.f);
	}
		
	m_pcBaseView->SetLightFollowsCamera(m_cPreference.Lighting.Light.FollowsCamera);
	//SetLightCount(LightCount); //defer until after camera is all set up
	// SetDeepSelectionMode(DeepSelection); OCC를 사용할 때 대응하는 함수
	m_pcBaseView->SetVisibilitySelectionMode(m_cPreference.Selection.Behavior.VisibilitySelection);

	m_pcBaseView->SetDynamicHighlighting(m_cPreference.Selection.Behavior.DynamicHighlighting);

	m_pcBaseView->SetDetailSelection(m_cPreference.Selection.Behavior.DetailSelection); // "Honor Line/Edge Weight/Pattern"
	m_pcBaseView->SetRelatedSelectionLimit(m_cPreference.Selection.Behavior.RelatedSelectionLimit);
	m_pcBaseView->SetTransparentSelectionBoxMode(m_cPreference.Selection.Behavior.UseSelectBox); // show a transparent box when selecting areas
	m_pcBaseView->SetRespectSelectionCulling(m_cPreference.Selection.Behavior.RespectCulling); // Respect Culling during selection.
	m_pcBaseView->SetFastFitWorld(true);
	m_pcBaseView->SetForceFastHiddenLine(m_cPreference.Perfromance.Optimization.HiddenLineMode == FastHiddenLine);
	m_pcBaseView->SetSpritingMode(m_cPreference.Interaction.GeometryManipulation.Spriting);
	m_pcBaseView->SetAllowInteractiveCutGeometry(m_cPreference.Interaction.GeometryManipulation.UpdateCutGeometry);
	m_pcBaseView->SetAllowInteractiveShadows(m_cPreference.Interaction.GeometryManipulation.UpdateShadows);

	m_pcBaseView->SetBackplaneCulling(m_cPreference.General.Etc.BackplaneCulling);
//	SetOcclusionCullingMode(m_cPreference.Perfromance.Optimization.OcclusionCulling, true, m_cPreference.Perfromance.Optimization.OcclusionThreshold);

	m_pcBaseView->SetDisplayListType(DisplayListSegment);// DisplayListOff);

	if (true == m_cPreference.Perfromance.FramerateOptimization.UseFramerate)
	{
		//if (!pDoc->IsFileReadDeferedForView() || CurrentFramerateMode == FramerateFixed)
		if (FramerateFixed == m_cPreference.Perfromance.FramerateOptimization.CurrentFramerateMode)
		{
			m_pcBaseView->SetFramerateMode(m_cPreference.Perfromance.FramerateOptimization.CurrentFramerateMode,
				m_cPreference.Perfromance.FramerateOptimization.FramerateTime, m_cPreference.Perfromance.FramerateOptimization.MaxThreshold, 
				UINT2bool(m_cPreference.Perfromance.FramerateOptimization.UseLods), m_cPreference.Perfromance.FramerateOptimization.DetailSteps, 
				m_cPreference.Perfromance.FramerateOptimization.HardCutoff);
		}
	}
	else if (m_cPreference.Perfromance.FramerateOptimization.CullingThresholdSet)
	{
		m_pcBaseView->SetFramerateMode(FramerateOff);
		m_pcBaseView->SetCullingThreshold(m_cPreference.Perfromance.FramerateOptimization.CullingThreshold);
	}
	else
	{
		m_pcBaseView->SetFramerateMode(FramerateOff);
		m_pcBaseView->SetCullingThreshold(0);
	}

	m_pcBaseView->SetSmoothTransition(false);
	m_pcBaseView->SetShadowRenderingMode(m_cPreference.Effects.SimpleShadow.ShadowRenderingMode);
	SetViewAxis();
	SetTransparency();

	m_pcBaseView->SetViewMode(HViewIsoFrontRightTop);		// fit the camera to the scene extents
	m_pcBaseView->SetAxisMode(m_cPreference.General.Rendering.DisplayAxisTriad ? AxisOn : AxisOff);

	// 배경화면 설정
	SetWindowBackGroundColor(m_cPreference.Appearance.BackgroundColor.Top, m_cPreference.Appearance.BackgroundColor.Bottom);
	//SetWindowColor(WindowBackgroundTopColor, WindowBackgroundBottomColor);

	HPoint FakeHLRColor;
	FakeHLRColor.Set(
		static_cast<float>(GetRValue(CAppSet_FakeHLRColor)) / 255.0f,
		static_cast<float>(GetGValue(CAppSet_FakeHLRColor)) / 255.0f,
		static_cast<float>(GetBValue(CAppSet_FakeHLRColor)) / 255.0f
	);

	m_pcBaseView->SetFakeHLRColor(FakeHLRColor);
	m_pcBaseView->SetProjMode(CAppSet_ProjectionMode);
	m_pcBaseView->SetSmoothTransition(CAppSet_bSmoothTransition);
	m_pcBaseView->SetSmoothTransitionDuration(0.5f);
	m_pcBaseView->GetUndoManager()->Flush();			//don't care about this initial camera change
	m_pcBaseView->SetDisplayHandlesOnDblClk(!CAppSet_DisableEditing);

	//SetCoordinateSystemHandedness(bWorldHandedness ? HandednessRight : HandednessLeft, true);
	m_pcBaseView->SetHandedness(CAppSet_bWorldHandedness ? HandednessRight : HandednessLeft, true);

	// The state of world today with polygon handedness is
	// 1. Since we are using display lists by default, we want this setting.
	// 2. We will have it only on the Canvas key. If required, model could have it's own
	// Rajesh B (11-Apr-2003)
	m_pcBaseView->SetPolygonHandednessMode(HandednessLeft);

	HPixelRGBA cHighlightSelectColor;
	cHighlightSelectColor.Set(255, 0, 0);
	m_pcBaseView->GetHighlightSelection()->SetSelectionFaceColor(cHighlightSelectColor);
	m_pcBaseView->GetHighlightSelection()->SetSelectionEdgeColor(cHighlightSelectColor);
	m_pcBaseView->GetHighlightSelection()->SetSelectionMarkerColor(cHighlightSelectColor);

	m_pcBaseView->GetSelection()->SetSelectionEdgeWeight(2.0);
	m_pcBaseView->GetHighlightSelection()->SetSelectionEdgeWeight(2.0);

	m_pcBaseView->GetHighlightSelection()->SetGrayScale(false);// CAppSet_bGrayScaleSelection);
	m_pcBaseView->GetHighlightSelection()->SetUseDefinedHighlight(false);// CAppSet_bUseDefinedHighlighting);
	m_pcBaseView->GetHighlightSelection()->SetAllowDisplacement(false);// CAppSet_bDisplaceSelection);
	m_pcBaseView->GetHighlightSelection()->UpdateHighlightStyle();

	// set the selection color
	HSelectionSet * sel_set = m_pcBaseView->GetSelection();
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

	m_pcBaseView->GetMarkupManager()->SetMarkupWeight(CAppSet_MarkupWeight / 100.0f);
	m_pcBaseView->SetShadowResolution(CAppSet_ShadowRes);
	m_pcBaseView->SetShadowBlurring(CAppSet_ShadowBlur);

	// set the color index interpolation settings
	m_pcBaseView->SetColorInterpolation(CAppSet_bCiByValue);
	m_pcBaseView->SetColorIndexInterpolation(CAppSet_bCiByColormapIndex, CAppSet_bCiIsolines);

	m_pcBaseView->GetSelection()->SetGrayScale(CAppSet_bGrayScaleSelection);
	m_pcBaseView->GetSelection()->SetUseDefinedHighlight(CAppSet_bUseDefinedHighlighting);
	m_pcBaseView->GetSelection()->SetAllowDisplacement(CAppSet_bDisplaceSelection);
	m_pcBaseView->GetSelection()->SetHighlightMode(HighlightQuickmoves);
	m_pcBaseView->GetHighlightSelection()->SetHighlightMode(HighlightQuickmoves);

	m_pcBaseView->GetSelection()->SetHighlightTransparency(CAppSet_TransparencyLevel);

	if (CAppSet_csRefSelType == "Spriting") {
		m_pcBaseView->GetSelection()->SetReferenceSelectionType(RefSelSpriting);
	}
	else if (CAppSet_csRefSelType == "Off") {
		m_pcBaseView->GetSelection()->SetReferenceSelectionType(RefSelOff);
	}
	else {
		m_pcBaseView->GetSelection()->SetReferenceSelectionType(RefSelDefault);
	}

	m_pcBaseView->GetSelection()->UpdateHighlightStyle();

	// set the rendermode
	m_pcBaseView->SetRenderMode(CAppSet_RenderMode, true);

	m_pcBaseView->SetEventCheckerCallback(event_checker);

	char chRenderingOption[MVO_BUFFER_SIZE] = "0";
	char chHeuristics[MVO_BUFFER_SIZE] = "0";
	char chNetHeuristics[MVO_BUFFER_SIZE] = "0";

/*
	HC_KEY nHighlightStyleKey = m_pcBaseView->GetHighlightSelection()->GetHighlightStyle();
	HC_Open_Segment_By_Key(nHighlightStyleKey); {
		HC_Show_Rendering_Options(chRenderingOption);
		HC_Show_Heuristics(chHeuristics);
	} HC_Close_Segment();

	HC_KEY nSelectionSegmentKey = m_pcBaseView->GetHighlightSelection()->GetSelectionSegment();
	HC_Open_Segment_By_Key(nSelectionSegmentKey); {
		HC_Show_Rendering_Options(chRenderingOption);
		HC_Show_Heuristics(chHeuristics);
	} HC_Close_Segment();

	HC_KEY nSceneKey = m_pcBaseView->GetSceneKey();
	HC_Open_Segment_By_Key(nSceneKey); {
		HC_Open_Segment("./overwrite/lights/selection_segment"); {
			HC_Show_Net_Heuristics(chNetHeuristics);
		}HC_Close_Segment();

		HC_Show_Rendering_Options(chRenderingOption);
		HC_Show_Heuristics(chHeuristics);
	} HC_Close_Segment();
*/


// 	SetDefaultOperator();
// 	SetSuppressUpdate(false);
// 
// 	return;

	//we need to adjust the axis window outside the mvo class as the calculation of the window
	//extents is mfc specific

	// initialize the QueryDialog and AdvancedQueryDialog
	// Remark
/*
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
		// install our custom TK_Initial_View opcode handler so that we set the initial Canvas appropriately
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

	
/*
 	if (pDoc->IsFileReadDeferedForView())
 		LoadFile(pDoc->filename, hmodel->GetStreamFileTK());
 	else
	{
		SetLineAntialiasing(CAppSet_LineAntialiasing);
		SetTextAntialiasing(CAppSet_TextAntialiasing);

		EmitSegment(GetModel()->GetModelKey(), true);

		SetupViews();

		if (CAppSet_UseFramerate && CAppSet_CurrentFramerateMode == FramerateTarget)
			EnableFrameRate();

		ViewReady();
	}
*/

	SetSceneFont(CAppSet_FontName, CAppSet_FontSize, CAppSet_FontUnits);

	// subscribe to selection events
// 	m_nCookieSelected = SetSignalNotify(HSignalSelected, CSolidHoopsView::signal_selected, this);
// 	m_nCookieDeSelectedAll = SetSignalNotify(HSignalDeSelectedAll, CSolidHoopsView::signal_deselected_all, this);

	// Remark
// 	CSolidHoopsFrame * frame = (CSolidHoopsFrame *)AfxGetMainWnd();
// 	frame->GetBhvToolbar()->m_wndBhvSlider.SetPos(0);


	HC_Open_Segment_By_Key(m_pcBaseView->GetCuttingPlanesKey()); {
		HC_Open_Segment("plane2"); {
			HC_Rotate_Object(90, 0, 0);
		}HC_Close_Segment();

		HC_Open_Segment("plane3"); {
			HC_Rotate_Object(0, 0, 90);
		}HC_Close_Segment();
	}HC_Close_Segment();

	//apply hiding of overlapped text (or not)
	m_pcBaseView->SetHideOverlappedText(CAppSet_bHideOverlappedText);

	HC_Open_Segment_By_Key(m_pcBaseView->GetShadowMapSegmentKey()); {
		if (CAppSet_bShadowMap) {
			sprintf(chRenderingOpts, "shadow map=(on, resolution=%d, samples=%d, %s jitter, %s)",
				CAppSet_nSMResolution, CAppSet_nSMSamples,
				(CAppSet_Jitter ? "" : "no"),
				(CAppSet_ViewDependentShadowMap ? "Canvas dependent" : "Canvas independent"));
		}
		else {
			sprintf(chRenderingOpts, "no shadow map");
		}
		HC_Set_Rendering_Options(chRenderingOpts);
	} HC_Close_Segment();

	HC_Open_Segment_By_Key(m_pcBaseView->GetSceneKey()); {
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
		HCLOCALE(sprintf(curve_opt, "general curve = (budget = %d, continued budget = %d, maximum deviation = %f, maximum angle = %f, maximum length = %f, %s Canvas independent)",
			CAppSet_Budget, CAppSet_ContinuedBudget, CAppSet_MaxDeviation / 10000.f,
			CAppSet_MaxAngle / 10000.f, CAppSet_MaxLength / 10000.f, CAppSet_bViewIndependent ? "" : "no"));

		HC_Set_Rendering_Options(curve_opt);

		m_pcBaseView->SetReflectionPlane(CAppSet_bReflectionPlane, CAppSet_ReflectionOpacity,
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

	m_pcBaseView->SetLightCount(CAppSet_LightCount);
	m_pcBaseView->SetViewSelectionLevel(HSelectionLevelSegment);

	SetDefaultOperator();

	m_pcWindow->GetSelectionOptionsControl().SetLevel(Selection::Level::Entity);
	//m_pcWindowKey->GetSelectionOptionsControl().SetProximity(0.1);
	m_pcWindow->GetSelectionOptionsControl().SetBias(Selection::Bias::Lines);// .SetBias(Selection::Bias::Markers);

	// do all the setup with no updates
	m_pcBaseView->SetSuppressUpdate(false);
}

void Canvas::SetGpu(CString strGpu)
{
	char gpu_to_use[256];
	strcpy(gpu_to_use, (char const *) H_UTF8(strGpu).encodedText());
	if(strcmp(gpu_to_use, "Default") != 0)
	{
		SegmentKey cSegment(m_pcBaseView->GetViewKey());
		HC_Set_Driver_Options(H_FORMAT_TEXT("gpu preference = specific = %s", gpu_to_use));
	}
}

void Canvas::SetDriverOption()
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

	HC_Open_Segment_By_Key(m_pcBaseView->GetViewKey()); {
		HC_Set_Driver_Options(chDriverOpts);
		HC_Set_Rendering_Options("anti-alias = (screen = on)");

		HC_Set_Driver_Options("special events, update interrupts");
		HC_Control_Update(".", "redraw everything");

		//HC_Set_Driver_Options("eye dome lighting = (on, strength=1.0)");

	} HC_Close_Segment();
}

// 투명도 적용 방법 설정
void Canvas::SetTransparency()
{
	char text[4096];
	char style[4096];
	char sorting[4096];
	char layers[4096];
	bool fast_z_sort = false;

	strcpy(style, H_ASCII_TEXT(m_cPreference.General.Transparency.Style));
	strcpy(sorting, H_ASCII_TEXT(m_cPreference.General.Transparency.Sorting));
	strcpy(layers, H_ASCII_TEXT(m_cPreference.General.Transparency.DepthPeelingLayers));

	if(strstr(sorting, "z-sort")) {
		if (strstr(sorting, "fast")) {
			fast_z_sort = true;
		}
		sprintf(sorting, "z-sort only");
	}

	sprintf(text, "style = %s, hsr algorithm = %s, depth peeling options = (layers= %s, algorithm=%s), depth writing = %s",
		style, sorting, layers, m_cPreference.General.Transparency.PixelOIT ? "pixel" : "buffer", 
		m_cPreference.General.Transparency.DepthWriting == true ? "on" : "off");

	m_pcBaseView->SetTransparency(text, fast_z_sort);
}

void Canvas::SetViewAxis()
{
	char text[4096];
	HVector front, top;
	CString strViewAxis = " 1  0  0  0  1  0  0  0  1";
	strcpy(text, H_ASCII_TEXT(strViewAxis));
	sscanf(text, "%f %f %f %f %f %f", &front.x, &front.y, &front.z,
		&top.x, &top.y, &top.z);

	m_pcBaseView->SetViewAxis(&front, &top);

	m_pcBaseView->SetAxisMode(AxisOn);
}

void Canvas::SetSelectOption()
{
	HPixelRGBA cHighlightSelectColor;
	cHighlightSelectColor.Set(255, 0, 0);
	m_pcBaseView->GetHighlightSelection()->SetSelectionFaceColor(cHighlightSelectColor);
	m_pcBaseView->GetHighlightSelection()->SetSelectionEdgeColor(cHighlightSelectColor);
	m_pcBaseView->GetHighlightSelection()->SetSelectionMarkerColor(cHighlightSelectColor);

	m_pcBaseView->GetHighlightSelection()->SetGrayScale(CAppSet_bGrayScaleSelection);
	m_pcBaseView->GetHighlightSelection()->SetUseDefinedHighlight(CAppSet_bUseDefinedHighlighting);
	m_pcBaseView->GetHighlightSelection()->SetInvisible(CAppSet_bInvisibleSelection);
	m_pcBaseView->GetHighlightSelection()->SetAllowDisplacement(CAppSet_bDisplaceSelection);
	m_pcBaseView->SetDynamicHighlighting(true);
	m_pcBaseView->GetHighlightSelection()->UpdateHighlightStyle();

	char chDriverOpts[MVO_BUFFER_SIZE];
	sprintf(chDriverOpts, "quick moves preference = %s", H_ASCII_TEXT(m_cPreference.Selection.Highlight.QuickMovesType));
	HC_Open_Segment_By_Key(m_pcBaseView->GetViewKey()); {
		HC_Set_Driver_Options(chDriverOpts);
	} HC_Close_Segment();

	// set the selection color
	HSelectionSet * sel_set = m_pcBaseView->GetSelection();
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

	m_pcBaseView->GetSelection()->SetGrayScale(CAppSet_bGrayScaleSelection);
	m_pcBaseView->GetSelection()->SetUseDefinedHighlight(CAppSet_bUseDefinedHighlighting);
	m_pcBaseView->GetSelection()->SetAllowDisplacement(CAppSet_bDisplaceSelection);
	m_pcBaseView->GetSelection()->SetInvisible(CAppSet_bInvisibleSelection);
	m_pcBaseView->GetSelection()->SetHighlightMode(CAppSet_HighlightMode);

	m_pcBaseView->GetHighlightSelection()->SetHighlightMode(CAppSet_HighlightMode);
	m_pcBaseView->GetSelection()->SetHighlightTransparency(CAppSet_TransparencyLevel);

	if (CAppSet_csRefSelType == "Spriting")
		m_pcBaseView->GetSelection()->SetReferenceSelectionType(RefSelSpriting);
	else if (CAppSet_csRefSelType == "Off")
		m_pcBaseView->GetSelection()->SetReferenceSelectionType(RefSelOff);
	else
		m_pcBaseView->GetSelection()->SetReferenceSelectionType(RefSelDefault);

	m_pcBaseView->GetSelection()->UpdateHighlightStyle();
}

void Canvas::SetWindowBackGroundColor(COLORREF nNewTopColor, COLORREF nNewBottomColor, bool bEmitMessage)
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

	m_pcBaseView->SetWindowColor(nWindowTopColor, nWindowBottomColor, bEmitMessage);
}

bool Canvas::GetKeyState(unsigned int key, int & flags)
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

void Canvas::SetMarkupColor(COLORREF new_color, bool emit_message)
{
	UNREFERENCED(emit_message);

	HPoint new_mkp_color;
	new_mkp_color.Set(
		static_cast<float>(GetRValue(new_color)) / 255.0f,
		static_cast<float>(GetGValue(new_color)) / 255.0f,
		static_cast<float>(GetBValue(new_color)) / 255.0f);

	HC_Open_Segment_By_Key(m_pcBaseView->GetMarkupManager()->GetMarkupKey()); {
		HC_Set_Color_By_Value("everything", "RGB", new_mkp_color.x, new_mkp_color.y, new_mkp_color.z);
	}HC_Close_Segment();

	m_pcBaseView->GetMarkupManager()->SetMarkupColor(new_mkp_color);
}

void Canvas::SetShadowColor(COLORREF new_color)
{
	HPoint new_shd_color;
	new_shd_color.Set(
		static_cast<float>(GetRValue(new_color)) / 255.0f,
		static_cast<float>(GetGValue(new_color)) / 255.0f,
		static_cast<float>(GetBValue(new_color)) / 255.0f);
	m_pcBaseView->SetShadowColor(new_shd_color);
}

void Canvas::event_checker(HIC_Rendition const * nr)
{
	//MSG msg;
	Canvas * pCurrentView = (Canvas *)HIC_Show_User_Index(nr, H_VIEW_POINTER_INDEX);
	if (pCurrentView)
	{
		int state = GetAsyncKeyState(VK_LBUTTON);
		if (state & 32768)
		{
			pCurrentView->GetBaseView()->GetConstantFrameRateObject()->InitiateDelay();

			pCurrentView->GetBaseView()->SetUpdateInterrupted(true);

			HIC_Abort_Update(nr);
		}

		state = GetAsyncKeyState(VK_MBUTTON);
		if (state & 32768)
		{
			HIC_Abort_Update(nr);
			pCurrentView->GetBaseView()->SetUpdateInterrupted(true);
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
		pCurrentView->GetBaseView()->GetConstantFrameRateObject()->InitiateDelay();
		pCurrentView->GetBaseView()->SetUpdateInterrupted(2);
		HIC_Abort_Update(nr);
	}

}

void Canvas::ViewReady()
{
	HBaseModel * hmodel = GetBaseView()->GetModel();

	GetBaseView()->SetSuppressUpdate(true);

	GetBaseView()->SetSplatRendering(BOOL2bool(CAppSet_bSplatRendering));

	GetBaseView()->SetFastMarkerDrawing(CAppSet_bFastMarkers);

	HC_Open_Segment_By_Key(GetBaseView()->GetShadowMapSegmentKey()); {
		char opt[MVO_BUFFER_SIZE];

		sprintf(opt, "shadow map=(%s, resolution=%d, samples=%d, %s jitter)",
			CAppSet_bShadowMap ? "on" : "off",
			CAppSet_nSMResolution, CAppSet_nSMSamples, CAppSet_Jitter ? "" : "no");

		HC_Set_Rendering_Options(opt);
	} HC_Close_Segment();


	HC_Open_Segment_By_Key(GetBaseView()->GetSceneKey()); {
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


	GetBaseView()->SetShadowLightDirection(CAppSet_UseLightVector, &CAppSet_LightVector);
	GetBaseView()->SetShadowIgnoresTransparency(CAppSet_IgnoreTransparency);
	GetBaseView()->SetShadowMode(CAppSet_ShadowMode);
	GetBaseView()->SetOcclusionCullingMode(CAppSet_OcclusionCulling, true);
	GetBaseView()->SetLineAntialiasing(m_cPreference.Appearance.AntiAliasing.Line);
	GetBaseView()->SetTextAntialiasing(m_cPreference.Appearance.AntiAliasing.Text);

	SetTransparency();

	//Turn on static model and display lists last, and in that order
	hmodel->SetStaticModel(CAppSet_StaticModel);

	hmodel->SetLMVModel(CAppSet_LMVModel);

	if (CAppSet_bRestoreAnnotations) {
		GetBaseView()->SetAnnotationResize(true);
	}

	if (DisplayListOff == CAppSet_DisplayList) {
		GetBaseView()->SetDisplayListMode(false);
	}
	else {
		GetBaseView()->SetDisplayListType(CAppSet_DisplayList);
		GetBaseView()->SetDisplayListMode(true);
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

	GetBaseView()->SetHandednessFromModel();

	GetBaseView()->SetSuppressUpdate(false);
}

void Canvas::SetupViews()
{
	GetBaseView()->SetRenderMode(CAppSet_RenderMode, true);
	GetBaseView()->SetShadowMode(CAppSet_ShadowMode);
	GetBaseView()->SetOcclusionCullingMode(m_cPreference.Perfromance.Optimization.OcclusionCulling, true);
}

void Canvas::EnableFrameRate(bool onoff)
{
	int nSteps = (CAppSet_DynamicAdjustment ? m_cPreference.Perfromance.FramerateOptimization.DetailSteps : 0);

	if (onoff) {
		GetBaseView()->SetFramerateMode(FramerateTarget, m_cPreference.Perfromance.FramerateOptimization.FramerateTime,
			m_cPreference.Perfromance.FramerateOptimization.MaxThreshold, UINT2bool(m_cPreference.Perfromance.FramerateOptimization.UseLods), nSteps);
	}
	else {
		GetBaseView()->SetFramerateMode(FramerateOff);
	}
}

void Canvas::SetSceneFont(CString csFontName, CString csFontSize, CString csFontUnits)
{
	HC_Open_Segment_By_Key(GetBaseView()->GetSceneKey()); {

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

		GetBaseView()->SetFontSize(cfsize, true);

	} HC_Close_Segment();
}

bool Canvas::signal_selected(int signal, void * signal_data, void * user_data)
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
bool Canvas::signal_deselected_all(int signal, void * signal_data, void * user_data)
{
	return true;
/*
	UNREFERENCED(signal);
	UNREFERENCED(signal_data);

	return OnSignalDeSelectedAll();*/
}

bool Canvas::OnSignalSelected()
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
bool Canvas::OnSignalDeSelectedAll()
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
void Canvas::CancelCommands()
{
	DeSelectAll();
}

//== Mouse 관련 함수 =============================================================================
bool Canvas::LButtonDown(int nFlags, int x, int y)
{
	GetBaseView()->SetDynamicHighlighting(false);

	// Shift & L Button 이벤트는 Area Select
	if (MK_SHIFT & nFlags) {
		GetBaseView()->SetOperator(m_pcSelectArea);
	}
	else {
		GetBaseView()->SetOperator(m_pcCameraOrbitSelect);
	}

	HEventInfo cEvent(GetBaseView());
	cEvent.SetPoint(HE_LButtonDown, x, y, MouseMapFlags(nFlags));
	HLISTENER_EVENT(HMouseListener, GetBaseView()->GetEventManager(), OnLButtonDown(cEvent));
	return true;
}

bool Canvas::LButtonUp(int nFlags, int x, int y)
{
	GetBaseView()->SetDynamicHighlighting(true);

	HEventInfo cEvent(GetBaseView());
	cEvent.SetPoint(HE_LButtonUp, x, y, MouseMapFlags(nFlags));
	HLISTENER_EVENT(HMouseListener, GetBaseView()->GetEventManager(), OnLButtonUp(cEvent));

	HBaseOperator * op = GetBaseView()->GetCurrentOperator();

	if (op) {
		if (op->Capture()) {

		}
	}

	GetBaseView()->SetOperator(m_pcCameraOrbitSelect);

	return true;
}

bool Canvas::RButtonDown(int nFlags, int x, int y)
{
	GetBaseView()->SetDynamicHighlighting(false);

	GetBaseView()->SetOperator(m_pcCameraPan);

	HEventInfo cEvent(GetBaseView());
	cEvent.SetPoint(HE_RButtonDown, x, y, MouseMapFlags(nFlags));
	HLISTENER_EVENT(HMouseListener, GetBaseView()->GetEventManager(), OnRButtonDown(cEvent));
	return true;
}

bool Canvas::RButtonUp(int nFlags, int x, int y)
{
	GetBaseView()->SetDynamicHighlighting(true);

	HEventInfo cEvent(GetBaseView());
	cEvent.SetPoint(HE_RButtonUp, x, y, MouseMapFlags(nFlags));
	HLISTENER_EVENT(HMouseListener, GetBaseView()->GetEventManager(), OnRButtonUp(cEvent));

	GetBaseView()->SetOperator(m_pcCameraOrbitSelect);

	return true;
}

bool Canvas::MouseMove(int nFlags, int x, int y)
{
	// Control을 누른경우 Face 단위로 선택이 됨.
/*
	if (nFlags & MK_CONTROL) {
		// select on arbitrary subentities(face, edge, or vertex)
		GetHighlightSelection()->SetSelectionLevel(HSelectLevel::HSelectEntity);
	}
	else {
		GetHighlightSelection()->SetSelectionLevel(HSelectLevel::HSelectSegment);
	}
*/

	HEventInfo cEvent(GetBaseView());
	cEvent.SetPoint(HE_MouseMove, x, y, MouseMapFlags(nFlags));

/*
	if (MK_LBUTTON & nFlags || MK_RBUTTON & nFlags) {
		GetBaseView()->GetOperator()->OnMouseMove(cEvent);
	}
	else {
		m_pcWindow->OnMouseMove(cEvent);
	}
*/

	HLISTENER_EVENT(HMouseListener, GetBaseView()->GetEventManager(), OnMouseMove(cEvent));

	return true;
}

// Mouse Wheel 대응
bool Canvas::MouseWheel(int nFlags, int zDelta, int x, int y, Json::Object & cInObject)
{
	Json::Array & cArray = cInObject.GetArray(SKW_RECT);

	int nLeft = cArray[0]->ToInteger();
	int nTop = cArray[1]->ToInteger();

	HEventInfo	cEvent(GetBaseView());
	cEvent.SetPoint(HE_MouseWheel, x - nLeft, y - nTop, MouseMapFlags(nFlags));
	cEvent.SetMouseWheelDelta(zDelta);
	HLISTENER_EVENT(HMouseListener, GetBaseView()->GetEventManager(), OnMouseWheel(cEvent));

	return true;
}

DWORD Canvas::MouseMapFlags(DWORD state)
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

void Canvas::SetDefaultOperator()
{
	//m_pcCameraManipulate = new HOpCameraManipulate(this, 0, 1, new OpCameraOrbitSelect(this), new OpCameraPan(this));
// 		, new HSOpCameraPan(m_pHView),
// 		new HSOpCameraZoom(m_pHView), 0, false))
	
	m_pcCameraOrbitSelect = new OpCameraOrbitSelect(m_pcWindow);
	m_pcCameraPan = new OpCameraPan(GetBaseView());
	m_pcSelectArea = new OpSelectArea(GetBaseView());

	GetBaseView()->SetOperator(m_pcCameraOrbitSelect);

	//LocalSetOperator(m_pcCameraManipulate);
}

void Canvas::LocalSetOperator(HBaseOperator * pcNewOperator)
{
	HBaseOperator * pcOperator = GetBaseView()->GetOperator();
	GetBaseView()->SetOperator(pcNewOperator);

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
void Canvas::DeSelectAll()
{
	if (0 < GetBaseView()->GetSelection()->GetSize()) {
		GetBaseView()->GetSelection()->DeSelectAll();
		GetBaseView()->ForceUpdate();
	}
}

void Canvas::SetSubentitySelectLevel()
{
	HSelectionSet * pcSelection = GetBaseView()->GetSelection();

	if (HSelectLevel::HSelectSubentity != pcSelection->GetSelectionLevel()) {
		pcSelection->DeSelectAll();
		GetBaseView()->Update();
	}
	pcSelection->SetSelectionLevel(HSelectLevel::HSelectSubentity);
	GetBaseView()->SetViewSelectionLevel(HSelectionLevelEntity);
	GetBaseView()->Update();
}

//== Clash 관련 함수 =================================================================================

void Canvas::ClearClashList()
{
	if(nullptr != m_pcClashList)
	{
		delete_vlist(m_pcClashList);
	}

	m_pcClashList = new_vlist(malloc, free);
}