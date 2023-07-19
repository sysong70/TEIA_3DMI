#pragma once

#include <Json.h>

#include "3DF.h"
#include "3DF.BaseView.h"
#include "3DF.Facility.AppOptions.h"

#include "3DF.NavigationCube.h"

#include <HSelectionSet.h>

OPEN_3DF_NAMESPACE

namespace Operator
{
	class CameraSelect;
	class CameraPan;
	class SelectArea;
}

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
	void InitNavigationCube(int nWidth, int nHeight);
	bool IsInitNavigationCube() { return m_bInitNaviCube; }

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

	//== Command 관련 함수 ===========================================================================
public:
	void CancelCommands();
	void Resize(int cx, int cy);

	//== Mouse 관련 함수 =============================================================================
public:
	bool LButtonUp(int nFlags, int x, int y);
	bool LButtonDown(int nFlags, int x, int y);
/*

	bool MButtonUp(TDF::View * pcView, int nFlags, int x, int y);
	bool MButtonDown(TDF::View * pcView, int nFlags, int x, int y);
*/
	bool RButtonUp(int nFlags, int x, int y);
	bool RButtonDown(int nFlags, int x, int y);

	bool MouseMove(int nFlags, int x, int y);

	bool MouseWheel(int nFlags, int zDelta, int x, int y, Json::Object & cInObject);
protected:
	DWORD MouseMapFlags(DWORD state);

public:

	//:Ken - 20230607
	bool KeyboardInput(Json::Object& input);

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
	Facility::KernelOption m_cPreference;

	//== 운영 변수 ===================================================================================
public:
	TDF::BaseView * GetBaseView() { return m_pcBaseView; }
	
	int ViewId() { return m_nViewId; }
	void SetViewId(int nViewId);

protected:
	TDF::BaseView * m_pcBaseView = nullptr;
	TDF::WindowKey * m_pcWindow = nullptr;

	int m_nViewId = -1;

private:
	Operator::CameraSelect * m_pcCameraOrbitSelect;
	Operator::SelectArea * m_pcSelectArea;

	NavigationCube m_cNaviCube;
	bool m_bInitNaviCube = false;

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
	long		m_nCookieSelected;					// cookie for HSignalSelected signal subscription
	long		m_nCookieDeSelectedAll;				// cookie for HSignalDeSelectedAll signal subscription

	// keys to HOOPS entities that need to be sweetened.  Should be private but public for the moment
	HC_KEY * m_pnSweetenKeyList = nullptr;	

	void ClearClashList();
	struct vlist_s * m_pcClashList;
};

CLOSE_3DF_NAMESPACE