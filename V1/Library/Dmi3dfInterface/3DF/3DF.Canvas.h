#pragma once

#include <Json.h>

#include "HBaseView.h"
#include "HUtility.h"

#include "3DF.h"
#include "3DF.Facility.Preference.h"

#include <HSelectionSet.h>

OPEN_3DF_NAMESPACE

class OpCameraOrbitSelect;
class OpCameraPan;
class OpSelectArea;

//== Camera 관련 Class ==============================================================================

class CameraPos
{
public:
	CameraPos();

	HPoint p, t, u;
	float w, h;
	bool bActive;
};

class Canvas // : public HBaseView, public HAnimationListener
{
public:
	Canvas(HBaseModel * pcBaseModel, void * pcWindowHandle);

/*
	Canvas(HBaseModel * pcBaseModel, const char * pchAlias = nullptr, const char * pchDriverType = nullptr,
		const char * pchInstanceName = nullptr, void * pcWindowHandle = nullptr, void * pcColorMap = nullptr);
*/

	virtual ~Canvas();

	//== Hoops 설정 함수 =============================================================================
	void Init();
	void ViewReady();

protected:
	void SetGpu(CString strGpu);
	void SetDriverOption();
	void SetTransparency();
	void SetViewAxis();
	void SetSelectOption();

	void SetWindowBackGroundColor(COLORREF nNewTopColor, COLORREF nNewBottomColor, bool bEmitMessage = true);

	static bool GetKeyState(unsigned int key, int & flags);
	void SetMarkupColor(COLORREF new_color, bool emit_message = true);
	void SetShadowColor(COLORREF new_color);
	static void event_checker(HIC_Rendition const * nr);

	void SetShowCollisions(bool sc) { m_bShowCollisions = sc; }

	void SetupViews();

	void EnableFrameRate(bool onoff = true);

	void SetSceneFont(CString csFontName, CString csFontSize, CString csFontUnits);

	// MVO event handlers
	static bool signal_selected(int signal, void * signal_data, void * user_data);
	bool OnSignalSelected();

	static bool signal_deselected_all(int signal, void * signal_data, void * user_data);
	bool OnSignalDeSelectedAll();

	HSelectLevel m_eSelectLevel = HSelectEntity;

	bool CAppSet_OcclusionCulling = false;
	int CAppSet_OcclusionThreshold = 50;
	COLORREF CAppSet_FakeHLRColor = RGB(255, 255, 255);
	ProjMode CAppSet_ProjectionMode = ProjOrthographic;
	bool CAppSet_bSmoothTransition = true;
	bool CAppSet_DisableEditing = true;
	bool CAppSet_bWorldHandedness = true;
	bool CAppSet_bGrayScaleSelection = false;	// no gray scale selection
	bool CAppSet_bUseDefinedHighlighting = false;
	bool CAppSet_bInvisibleSelection = false;
	bool CAppSet_bDisplaceSelection = false;	// no gray scale selection
	int CAppSet_SelectionColorTransparency = 0;
	COLORREF CAppSet_PolygonSelectionColor = RGB(255, 128, 0);
	COLORREF CAppSet_LineSelectionColor = RGB(255, 128, 0);
	COLORREF CAppSet_MarkerSelectionColor = RGB(255, 128, 0);
	COLORREF CAppSet_MarkupColor = RGB(255, 0, 0);
	COLORREF CAppSet_ShadowColor = RGB(45, 45, 45);
	int	CAppSet_MarkupWeight = 400;	// Markup line weight * 100 (to store float CAppSet_as int)
	int	CAppSet_ShadowRes = 64;
	int	CAppSet_ShadowBlur = 4;
	bool CAppSet_bCiByValue = true;// color interpolation, by value
	bool CAppSet_bCiByColormapIndex = true;	// color interpolation, by colormap index
	bool CAppSet_bCiIsolines = false;	// color interpolation, isolines only
	HSelectionHighlightMode CAppSet_HighlightMode = HighlightQuickmoves;
	float CAppSet_TransparencyLevel = 0.9f;
	CString CAppSet_csRefSelType = "Spriting";
	HRenderMode CAppSet_RenderMode = HRenderGouraud;
	bool CAppSet_ShowCollisions = false;
	HShadowMode CAppSet_ShadowMode = HShadowNone;
	bool CAppSet_bShadowMap = false;
	int CAppSet_HardCutoff = 10;
	bool CAppSet_bSplatRendering = false;	//use splat rendering
	bool CAppSet_bFastMarkers = true;
	int CAppSet_nSMResolution = 2048;
	int CAppSet_nSMSamples = 4;
	bool CAppSet_Jitter = false;

	bool CAppSet_bReflectionPlane = false;
	float CAppSet_ReflectionOpacity = 0.5;
	bool CAppSet_bReflectionFading = true;
	bool CAppSet_ReflectionUseAttenuation = false;
	float CAppSet_ReflectionHither = 0.f;
	float CAppSet_ReflectionYon = 1.f;
	bool CAppSet_ReflectionUseBlur = false;
	int CAppSet_ReflectionBlur = 1;
	bool CAppSet_UseLightVector = false;
	HPoint CAppSet_LightVector = HPoint(0.0f, 0.0f, 1.0f);
	bool CAppSet_IgnoreTransparency = false;
	bool CAppSet_StaticModel = true;
	bool CAppSet_LMVModel = true;
	bool CAppSet_bRestoreAnnotations = false;
	DisplayListType CAppSet_DisplayList = DisplayListSegment;
	int	CAppSet_DynamicAdjustment = 1;

	float CAppSet_ShadowOpacity = 1.f;


	CString CAppSet_FontName = "sans serif";
	CString CAppSet_FontSize = "14";
	CString CAppSet_FontUnits = "pts";

	bool CAppSet_bHideOverlappedText = false;	//hide overlapped text?
	bool CAppSet_ViewDependentShadowMap = true;

	CString CAppSet_LineWeight = "1 pixels";
	bool CAppSet_StereoMode = false;
	int CAppSet_StereoSeparation = 30000;

	CString CAppSet_GoochColorMap = "blue, red";

	COLORREF CAppSet_GoochColor1 = RGB(255.0, 105.0, 180.0);
	COLORREF CAppSet_GoochColor2 = RGB(60.0, 179.0, 113.0);
	COLORREF CAppSet_GoochColor3 = RGB(255.0, 255.0, 255.0);
	COLORREF CAppSet_GoochColor4 = RGB(255.0, 255.0, 255.0);
	COLORREF CAppSet_GoochColor5 = RGB(255.0, 255.0, 255.0);
	COLORREF CAppSet_GoochColor6 = RGB(255.0, 255.0, 255.0);

	float CAppSet_GoochWeight = 0.5f;
	float CAppSet_GoochColorHigh = 2.0f;

	int CAppSet_NearCameraLimit = 0;
	int CAppSet_Budget = 512;
	int CAppSet_ContinuedBudget = 0;
	int CAppSet_MaxDeviation = 0;
	int CAppSet_MaxAngle = 200000;
	int CAppSet_MaxLength = 1000;
	bool CAppSet_bViewIndependent = true;

	bool CAppSet_HemisphericAmbient = false;
	COLORREF CAppSet_AmbientTopColor = RGB(1.0000 * 255, 1.0000 * 255, 1.0000 * 255);
	COLORREF CAppSet_AmbientBottomColor = RGB(1.0000 * 255, 1.0000 * 255, 1.0000 * 255);
	bool CAppSet_UseAmbientUpVector = false;
	HPoint CAppSet_AmbientUpVector = HPoint(0.0f, 1.0f, 0.0f);
	bool CAppSet_bUseGreeking = false;	//use text greeking
	CString CAppSet_GreekingMode = "Box";
	CString CAppSet_GreekingUnits = "px";
	int	CAppSet_GreekingLimit = 6000;	// Greeking limit * 1000 (to store float as int)

	int CAppSet_LightCount = 1;

	CString CAppSet_TransparencyStyle = "blended";
	CString CAppSet_TransparencySorting = "depth peeling";
	CString CAppSet_TransparencyDepthPeelingLayers = "3";
	bool CAppSet_PixelOIT = false;
	bool CAppSet_DepthWriting= false;

	//== Command 관련 함수 ===========================================================================
public:
	void CancelCommands();

	//== Mouse 관련 함수 =============================================================================
public:
	bool LButtonUp(int nFlags, int x, int y);
	bool LButtonDown(int nFlags, int x, int y);
/*

	bool MButtonUp(_3DF::View * pcView, int nFlags, int x, int y);
	bool MButtonDown(_3DF::View * pcView, int nFlags, int x, int y);
*/
	bool RButtonUp(int nFlags, int x, int y);
	bool RButtonDown(int nFlags, int x, int y);

	bool MouseMove(int nFlags, int x, int y);

	bool MouseWheel(int nFlags, int zDelta, int x, int y, Json::Object & cInObject);
protected:
	DWORD MouseMapFlags(DWORD state);

	//== Operator 관련 함수 ==========================================================================
protected:
	void SetDefaultOperator();
	void LocalSetOperator(HBaseOperator * pcNewOperator);

	//== Select 관련 함수 ============================================================================
public:
	void DeSelectAll();
	void SetSubentitySelectLevel();

	//== 환경 변수 관련 함수 ==========================================================================
protected:
	Facility::Preference m_cPreference;

	//== 운영 변수 ===================================================================================
public:
	HBaseView * GetBaseView() { return m_pcBaseView; }

protected:
	HBaseView * m_pcBaseView = nullptr;
	_3DF::WindowKey * m_pcWindow = nullptr;

private:
	OpCameraOrbitSelect * m_pcCameraOrbitSelect;
	OpCameraPan * m_pcCameraPan;
	OpSelectArea * m_pcSelectArea;

private:
	HPoint		m_pos, m_tar;
	bool		m_bhidden_exists;
	bool		m_bUseLOD;
	ProjMode	m_ProjTmp;
	CameraPos	cameras[10];
	bool		m_collab_mode;
	bool		m_bShowCollisions;
	bool		m_bOocSelection;
	bool		m_bDeepSelection;
	long m_nCookieSelected;					// cookie for HSignalSelected signal subscription
	long m_nCookieDeSelectedAll;				// cookie for HSignalDeSelectedAll signal subscription

	// keys to HOOPS entities that need to be sweetened.  Should be private but public for the moment
	HC_KEY * m_pnSweetenKeyList = nullptr;	

	void ClearClashList();
	struct vlist_s * m_pcClashList;
};

CLOSE_3DF_NAMESPACE