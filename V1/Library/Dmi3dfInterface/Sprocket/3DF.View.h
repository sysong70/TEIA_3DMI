#pragma once

#include <Json.h>

#include <map>
#include <chrono>

#include "../3DF/3DF.h"

#include "Sprocket.h"

#include "../3DF.Signal.h"
#include "../../Signal/Signal.h"

namespace H3DF
{
	class BaseView;
	class Canvas;
	class NavigationCube;

	class API_3DF View : public Sprocket
	{
	public:
		View();
		View(View const & cInThat);
		virtual ~View();

		void Set(View const & cInThat);
		View const & operator = (View const & cInThat);

		Model & GetAttachedModel() const;

		SegmentKey GetSegmentKey();
		SegmentKey const GetSegmentKey() const;

		SegmentKey GetModelOverrideSegmentKey();
		SegmentKey const GetModelOverrideSegmentKey() const;

		PortfolioKey const GetPortfolioKey() const;
		PortfolioKey GetPortfolioKey();

		//== Keyboard 관련 함수 ======================================================================
		bool Char(UINT nChar, UINT nRepCnt, UINT nFlags);
		// KEN - 20230607
		bool KeyboardInput(Json::Object & input);

		// KEN - 20230607
		bool ExecuteKeyboardSignal(int nAction, Json::Object & cInObject);

		//== Select 관련 함수 ========================================================================
		void SetSubentitySelectLevel();
	
		//== View Style 관련 함수 ====================================================================
		void SetRenderingMode(Rendering::Mode eInMode);
		Rendering::Mode GetRenderingMode() const;

		void SetViewDirection(ViewDirection::Mode eInMode);

		// 그림자 관련 함수
		void SetSimpleShadow(bool bInState, float fInPercentOffset = 5.0f);
		bool GetSimpleShadow();

		// 반사 관련 함수
		void SetSimpleReflection(bool bInState, float fInPercentOffset = 5.0f);
		bool GetSimpleReflection();

		void SmoothTransition(H3DF::CameraKit const & cInCamera);

	public:
		void SaveHsfFile(CString strFilePathName, H3DF::Canvas * pcHoopsView);
		void LoadPointCloudFile(CString strFilePathName);

		bool DoDynamicHighlighting(H3DF::HighlightControl & cHighlightControl,WindowPoint cInWindowPoint);
	};
};