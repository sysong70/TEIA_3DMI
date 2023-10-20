#pragma once

#include "../3DF/3DF.h"

#include <HBaseView.h>
#include <HUtility.h>

#include "../3DF/NavigationCube.h"

namespace H3DF
{
	namespace Operator
	{
		class CameraSelect;
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

		void SetViewMode(H3DF::ViewMode mode, bool bFitWorld = true);

	public:
		void SetNavigationCube(NavigationCube * pcNaviCube);

	private:
		NavigationCube * m_pcNaviCube = nullptr;

		H3DF::ViewMode m_eViewMode = H3DF::ViewMode::Unknown;
	};

	class ViewPrivate : public PrivateImpl
	{
	public:
		ViewPrivate();
		virtual ~ViewPrivate();

		void Copy(const ViewPrivate * pcInThat);

		bool Init(H3DF::Model * pcInModel, const char * pchInDriverType, const char * pchInInstanceName, H3DF::WindowHandle nInWindowHandle);

		H3DF::BaseView * GetBaseView() { return m_pcBaseView; }

		BaseView * m_pcBaseView = nullptr;
		H3DF::WindowKey * m_pcWindow = nullptr;
		char * m_pchName = nullptr;

	protected:
		static bool GetKeyState(unsigned int key, int & flags);
		void SetMarkupColor(COLORREF new_color, bool emit_message = true);
		void SetShadowColor(COLORREF new_color);
		static void event_checker(HIC_Rendition const * nr);

		void SetViewAxis();
		void SetTransparency();
		void SetSelectOption();

		void SetWindowBackGroundColor(COLORREF nNewTopColor, COLORREF nNewBottomColor, bool bEmitMessage = true);

		void SetShowCollisions(bool sc) { m_bShowCollisions = sc; }

		void SetSceneFont(CString csFontName, CString csFontSize, CString csFontUnits);

		//== Operator 관련 함수 ======================================================================
	protected:
		void SetDefaultOperator();
		void LocalSetOperator(HBaseOperator * pcNewOperator);

	private:
		bool m_bShowCollisions = false;

		Operator::CameraSelect * m_pcCameraOrbitSelect = nullptr;
		Operator::SelectArea * m_pcSelectArea = nullptr;

		NavigationCube m_cNaviCube;
	};
}