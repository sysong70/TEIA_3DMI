#pragma once

#include "../3DF/3DF.h"

#include <HBaseView.h>
#include <HUtility.h>

#include <Json.h>

#include "../3DF/NavigationCube.h"
#include "../3DF/Portfolio.h"

namespace H3DF
{
	namespace Operator
	{
		class CameraControl;
		class CameraPan;
		class SelectArea;
	}

	class NavigationCube;

	class BaseView : public HBaseView
	{
	public:
		BaseView(HBaseModel * model,
			const char * alias = 0,
			const char * driver_type = 0,
			const char * instance_name = 0,
			void * window_handle = 0,
			void * colormap = 0,
			void * clip_override = 0,
			void * window_handle_2 = 0,
			const char * driver_path = 0);

		void UpdateInternal(bool antialias = false, bool force_update = false) override;

		void SetViewDirection(H3DF::ViewDirection::Mode mode, bool bFitWorld = true);

	public:
		void SetNavigationCube(NavigationCube * pcNaviCube);

	private:
		NavigationCube * m_pcNaviCube = nullptr;

		H3DF::ViewDirection::Mode m_eViewMode = H3DF::ViewDirection::Mode::Unknown;
	};

	class ViewImpl : public Impl
	{
	public:
		ViewImpl();
		virtual ~ViewImpl();

		void Copy(const ViewImpl * pcInThat);

		bool Init(H3DF::Model * pcInModel, const char * pchInDriverType, const char * pchInInstanceName, H3DF::WindowHandle nInWindowHandle);

		void ViewReady();

		H3DF::BaseView * GetBaseView() { return m_pcBaseView; }

		SegmentKey GetSegmentKey() { return m_cKey; }
		SegmentKey GetModelKey() { return m_cModelKey; }

		Model & GetAttachedModel() {return *m_pcModel;}

		void SetWindowBackGroundColor(COLORREF nNewTopColor, COLORREF nNewBottomColor, bool bEmitMessage = true);

		void InitNavigationCube(int nWidth, int nHeight);
		bool IsInitNavigationCube();

		void Resize(int x, int y);

		NavigationCube & GetNavigationCube() { return m_cNaviCube; }

		//== Visual Effects 관련 함수 ================================================================
	public:
		bool GetSimpleShadow();
		void SetSimpleShadow(bool bFlag); 

		bool GetSimpleReflection();
		void SetSimpleReflection(bool bFlag);

		//== Keyboard 관련 함수 ======================================================================
		bool Char(UINT nChar, UINT nRepCnt, UINT nFlags);
		bool KeyboardInput(Json::Object & cInObject);

		//== Select 관련 함수 ========================================================================
	public:
		void DeSelectAll();
		void SetSubentitySelectLevel();

	protected:
		DWORD MouseMapFlags(DWORD nState);

	protected:
		static bool GetKeyState(unsigned int key, int & flags);
		void SetMarkupColor(COLORREF new_color, bool emit_message = true);
		void SetShadowColor(RGBAColor cInColor);
		static void event_checker(HIC_Rendition const * nr);

		void SetViewAxis();
		void SetTransparency();
		void SetSelectOption();

		void SetShowCollisions(bool sc) { m_bShowCollisions = sc; }

		void SetSceneFont(CString csFontName, CString csFontSize, CString csFontUnits);

		//== Operator 관련 함수 ======================================================================
	protected:
		void SetDefaultOperator();
		void LocalSetOperator(HBaseOperator * pcNewOperator);

	public:
		BaseView * m_pcBaseView = nullptr;
		H3DF::WindowKey * m_pcWindow = nullptr;
		char * m_pchName = nullptr;
		H3DF::Rendering::Mode m_eRenderingMode = H3DF::Rendering::Mode::Default;

		SegmentKey m_cKey;

		H3DF::Model * m_pcModel = nullptr;
		SegmentKey m_cModelKey;

		PortfolioKey m_cPortfolioKey;

		bool m_bShowCollisions = false;

		Operator::CameraControl * m_pcCameraSelect = nullptr;
		Operator::SelectArea * m_pcSelectArea = nullptr;

		NavigationCube m_cNaviCube;

		bool m_bSimpleShadowFlag = false;
		bool m_bSimpleReflection = false;
	};
}