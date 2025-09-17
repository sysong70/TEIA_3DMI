#pragma once

#include "../3DF.h"
#include "../Object.h"

#include <hc.h>
#include <HBaseView.h>
#include <HUtility.h>

#include "../Segment.h"
#include "SegmentImpl.h"

class HSelectionSet;
#include "../Selection.h"

#include "../NavigationCube.h"
#include "../Portfolio.h"

namespace H3DF
{
	namespace Operator
	{
		class CameraControl;
		class SelectArea;
	}

	class NavigationCube;

	// HBaseView를 상속받는 BaseView 클래스 
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

		void SetModel(H3DF::Model * model);

	public:
		void SetNavigationCube(NavigationCube * pcNaviCube);

	private:
		NavigationCube * m_pcNaviCube = nullptr;

		H3DF::ViewDirection::Mode m_eViewMode = H3DF::ViewDirection::Mode::Unknown;
	};

	class WindowKeyImpl : public SegmentKeyImpl
	{
	public:
		WindowKeyImpl();
		virtual ~WindowKeyImpl();

		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<WindowKeyImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy(const WindowKeyImpl * pcInThat);

		void ViewReady();

		bool Init(H3DF::Model * pcInModel, const char * pchInDriverType, CStringA strInInstanceName, H3DF::WindowHandle nInWindowHandle);

		void SetWindowBackGroundColor(COLORREF nNewTopColor, COLORREF nNewBottomColor, bool bEmitMessage = true);

		static void SetSelectionControl(WindowKey const & cInWindow);
		static void SetHighlightControl(WindowKey const & cInWindow);

		NavigationCube & GetNavigationCube();

		void InitNavigationCube(int nWidth, int nHeight);
		bool IsInitNavigationCube();

		void Resize(int x, int y);

	protected:
		void SetGpu(CString strGpu);
		void SetDriverOption();
		void SetAntiAliasOption();
		void SetTransparency();
		void SetupFrameRateMode();
		void SetViewAxis();

		void SetMarkupColor(COLORREF new_color, bool emit_message = true);
		void SetShadowColor(RGBAColor cInColor);
		static void event_checker(HIC_Rendition const * nr);

		void SetShowCollisions(bool sc) { m_bShowCollisions = sc; }

		void SetSceneFont(CString csFontName, CString csFontSize, CString csFontUnits);

		//== Operator 관련 함수 ======================================================================
	protected:
		void SetDefaultOperator();
		void LocalSetOperator(HBaseOperator * pcNewOperator);

	public:
		HC_KEY GetSceneKey();
		const HC_KEY GetSceneKey() const;

		H3DF::BaseView * GetBaseView();
		H3DF::BaseView * m_pcBaseView = nullptr;

		HSelectionSet * m_pcSelectionSet = nullptr;

		SelectionControl * m_pcSelection = nullptr;
		HighlightControl * m_pcHighlight = nullptr;
		SelectionOptionsControl * m_pcSelectionOptions = nullptr;
		SelectionOptionsKit m_cSelectionOptionsKit;

		H3DF::Model * m_pcModel = nullptr;
		SegmentKey m_cModelKey;

		NavigationCube * m_pcNaviCube = nullptr;

		bool m_bShowCollisions = false;

		int m_nViewId = -1;

		Operator::CameraControl * m_pcCameraSelect = nullptr;
		Operator::SelectArea * m_pcSelectArea = nullptr;

		HC_KEY * GetSelectBufferKey(int nCount);

	private:
		int m_nSelectBufferKeyCount = 64;
		HC_KEY * m_pnSelectBufferKey = nullptr;
	};
}