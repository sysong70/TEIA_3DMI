#pragma once

#include <Json.h>

#include "../3DF/3DF.h"

#include "../3DF/Facility.AppOptions.h"
#include "../3DF/Window.h"

#include "Sprocket.h"

#include <HSelectionSet.h>

class HBaseModel;
class HBaseOperator;

#include "../3DF.Signal.h"
#include "../Signal/Signal.h"

namespace H3DF
{
	namespace Operator
	{
		class CameraControl;
		class SelectArea;
		class KinematicTest;
	}

	//== Camera 관련 Class ==========================================================================

	class CameraPos
	{
	public:
		CameraPos();

		Point p, t, u;
		float w, h;
		bool bActive;
	};

	class API_3DF Canvas : public Sprocket
	{
	public:
		Canvas();
		Canvas(Canvas const & cInThat);
		virtual ~Canvas();

		void Set(Canvas const & cInThat);
		Canvas const & operator = (Canvas const & cInThat);

		void AttachViewAsLayout(View const & cInView);
		HWND GetHwnd();

		void SetDelivery(Signal::Delivery & cDelivery, int nViewId);
		void FileOpen(CString strFilePathName, CADModel & cInCADModel);
		static void ThreadFileOpen(Canvas * pcCanvas, CString strFilePathName, CADModel & cInCADModel);

		//== View 관련 함수 ==========================================================================
		H3DF::View & GetFrontView() const;
		H3DF::View & GetFrontView();

		H3DF::WindowKey & GetWindowKey() const;
		H3DF::WindowKey & GetWindowKey();

		Model & GetModel() const;

		void Update() const;

		void Update(Json::Object & cInObject) const;
		void Update(Json::Object & cInObject, Window::UpdateType eInType, H3DF::Time dInTimeLimit = -1.0) const;

		SegmentKey GetConstructionKey();
		SegmentKey const GetConstructionKey() const;

		SegmentKey GetSceneKey();
		SegmentKey const GetSceneKey() const;

		SegmentKey GetOverwriteKey();
		SegmentKey const GetOverwriteKey() const;

		void InvalidateSceneBounding();

		void SuppressUpdate(bool bSuppress);

		bool GetSuppressUpdate();
		bool GetSuppressUpdateTick();

		void Resize(int cx, int cy);

		//== Keyboard 관련 함수 ======================================================================
		bool Char(UINT nChar, UINT nRepCnt, UINT nFlags);

	public:
		// KEN - 20230607
		bool KeyboardInput(Json::Object & input);

		//== 환경 변수 관련 함수 =======================================================================
	protected:
		Facility::KernelOption m_cPreference;

	protected:
		int m_nViewId = -1;
	};
}