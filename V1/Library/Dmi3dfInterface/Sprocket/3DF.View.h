#pragma once

#include <Json.h>

#include <map>
#include <chrono>

#include "../3DF/3DF.h"

#include "Sprocket.h"

#include "../3DF/Window.h"


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

		H3DF::Type ObjectType() const { return H3DF::Type::View; };

		void Update() const;
		void Update(Json::Object & cInObject) const;
		void Update(Json::Object & cInObject, Window::UpdateType eInType, H3DF::Time dInTimeLimit = -1.0) const;

		void SuppressUpdate(bool bSuppress);

		void Destruct() const;
		void Resize(int x, int y);

		WindowKey & GetWindowKey() const;

		Model & GetAttachedModel() const;

		SegmentKey GetSegmentKey();
		SegmentKey const GetSegmentKey() const;

		SegmentKey GetModelOverrideSegmentKey();
		SegmentKey const GetModelOverrideSegmentKey() const;

		PortfolioKey const GetPortfolioKey() const;
		PortfolioKey GetPortfolioKey();

		NavigationCube & GetNavigationCube() const;

		void SetSuppressUpdate(bool bInState);

		//== Command 관련 함수 =======================================================================
		void CancelCommands();
		void CancelCommands() const;

		//== Keyboard 관련 함수 ======================================================================
		bool Char(UINT nChar, UINT nRepCnt, UINT nFlags);
		//:Ken - 20230607
		bool KeyboardInput(Json::Object & input);

		//:Ken - 20230607
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

	public:
		void SaveHsfFile(CString strFilePathName, H3DF::Canvas * pcHoopsView);
		void LoadPointCloudFile(CString strFilePathName);
	};
};