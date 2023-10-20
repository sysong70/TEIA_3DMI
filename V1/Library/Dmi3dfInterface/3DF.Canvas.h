#pragma once

#include <Json.h>

#include "3DF/3DF.h"

#include "3DF/Facility.AppOptions.h"
#include "3DF/Window.h"

#include "Sprocket.h"

#include <HSelectionSet.h>

class HBaseModel;
class HBaseOperator;

namespace H3DF
{
	namespace Operator
	{
		class CameraSelect;
		class CameraPan;
		class SelectArea;
		class KinematicTest;
	}

	//== Camera 관련 Class ==============================================================================

	class CameraPos
	{
	public:
		CameraPos();

		Point p, t, u;
		float w, h;
		bool bActive;
	};

	class BaseView;
	class NavigationCube;

	class API_3DF Canvas : public Sprocket
	{
	public:
		Canvas();
		Canvas(Canvas const & cInThat);
		virtual ~Canvas();

		void Destruct();

		void Set(Canvas const & cInThat);
		Canvas const & operator = (Canvas const & cInThat);

		H3DF::Type ObjectType() const { return H3DF::Type::Canvas; };

		void AttachViewAsLayout(View const & cInView);

		H3DF::View GetFrontView() const;

		void Update() const;
		void Update(Window::UpdateType eInType, H3DF::Time dInTimeLimit = -1.0) const;

		void Resize(int cx, int cy);

		//==========================================================================================


		//Canvas(Canvas const & cInThat);

		Canvas(HBaseModel * pcBaseModel, void * pcWindowHandle);

		/*
			Canvas(HBaseModel * pcBaseModel, const char * pchAlias = nullptr, const char * pchDriverType = nullptr,
				const char * pchInstanceName = nullptr, void * pcWindowHandle = nullptr, void * pcColorMap = nullptr);
		*/

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
		//static void event_checker(HIC_Rendition const * nr);

		void SetShowCollisions(bool sc) { m_bShowCollisions = sc; }

		void SetupViews();

		void EnableFrameRate(bool onoff = true);

		void SetSceneFont(CString csFontName, CString csFontSize, CString csFontUnits);

		// MVO event handlers
		static bool signal_selected(int signal, void * signal_data, void * user_data);
		bool OnSignalSelected();

		static bool signal_deselected_all(int signal, void * signal_data, void * user_data);
		bool OnSignalDeSelectedAll();

		//HSelectLevel m_eSelectLevel = HSelectEntity;

		//== Command 관련 함수 ===========================================================================
	public:
		void CancelCommands();

		//== Mouse 관련 함수 =============================================================================
	public:
		bool LButtonUp(int nFlags, int x, int y);
		bool LButtonDown(int nFlags, int x, int y);

		/*	bool MButtonUp(H3DF::View * pcView, int nFlags, int x, int y);
			bool MButtonDown(H3DF::View * pcView, int nFlags, int x, int y); */

		bool RButtonUp(int nFlags, int x, int y);
		bool RButtonDown(int nFlags, int x, int y);

		bool MouseMove(int nFlags, int x, int y);

		bool MouseWheel(int nFlags, int zDelta, int x, int y, int nLeft, int nTop);

		//== Keyboard 관련 함수 ==========================================================================
		bool Char(UINT nChar, UINT nRepCnt, UINT nFlags);

	protected:
		DWORD MouseMapFlags(DWORD state);

	public:
		//:Ken - 20230607
		bool KeyboardInput(Json::Object & input);

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
		H3DF::BaseView * GetBaseView() { return m_pcBaseView; }

		int ViewId() { return m_nViewId; }
		void SetViewId(int nViewId);

		//== Model 관련 함수 =============================================================================
	public:
		SegmentKey GetModelKey();

	protected:
		H3DF::Model * m_pcModel = nullptr; // 삭제 예정

	protected:
		H3DF::BaseView * m_pcBaseView = nullptr;
		H3DF::WindowKey * m_pcWindow = nullptr;

		int m_nViewId = -1;

	private:
		Operator::CameraSelect * m_pcCameraOrbitSelect;
		Operator::SelectArea * m_pcSelectArea;
		Operator::KinematicTest * m_pcKinematicTest;

		NavigationCube * m_pcNaviCube = nullptr;
		bool m_bInitNaviCube = false;

	private:
		//HPoint		m_pos, m_tar;
		bool		m_bhidden_exists;
		bool		m_bUseLOD;
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
}