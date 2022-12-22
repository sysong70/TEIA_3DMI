#pragma once

#include <Json.h>

#include "HBaseView.h"
#include "HUtility.h"

#include "3DF.h"

class HOpCameraManipulate;

OPEN_3DF_NAMESPACE

//== Camera 관련 Class ==============================================================================

class CameraPos
{
public:
	CameraPos();

	HPoint p, t, u;
	float w, h;
	bool bActive;
};

class View : public HBaseView, public HAnimationListener
{
public:
	View(HBaseModel * pcBaseModel, const char * pchAlias = nullptr, const char * pchDriverType = nullptr,
		const char * pchInstanceName = nullptr, void * pcWindowHandle = nullptr, void * pcColorMap = nullptr);
	virtual ~View();

	void Init() override;

protected:
	void SetGpu(CString strGpu);
	void SetDriverOption();
	void SetViewAxis();
	void SetWindowBackGroundColor(COLORREF nNewTopColor, COLORREF nNewBottomColor, bool bEmitMessage = true);

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

	//== Operator 관련 함수 ==============================================================================
protected:
	void SetDefaultOperator();
	void LocalSetOperator(HBaseOperator * pcNewOperator);

private:
	HOpCameraManipulate * m_pcCameraManipulate = nullptr;

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

	// keys to HOOPS entities that need to be sweetened.  Should be private but public for the moment
	HC_KEY * m_pnSweetenKeyList = nullptr;	

	void ClearClashList();
	struct vlist_s * m_pcClashList;
};

CLOSE_3DF_NAMESPACE