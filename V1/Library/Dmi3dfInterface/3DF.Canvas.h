#pragma once

#include <Json.h>

#include "3DF/3DF.h"

#include "3DF/Facility.AppOptions.h"
#include "3DF/Window.h"

#include "Sprocket.h"

#include <HSelectionSet.h>

class HBaseModel;
class HBaseOperator;

#include "3DF.Signal.h"
#include "../Signal/Signal.h"

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

		void FileOpen(Json::Object & cInObject, Signal::Delivery & cDelivery);
		static void ThreadFileOpen(const Canvas & cCanvas, Json::Object & cInObject, Signal::Delivery & cDelivery);
		void FileOpen_ORG(Json::Object & cInObject, Signal::Delivery & cDelivery);

		H3DF::View & GetFrontView() const;

		Model & GetModel() const;

		void Update() const;

		void Update(Json::Object & cInObject) const;
		void Update(Json::Object & cInObject, Window::UpdateType eInType, H3DF::Time dInTimeLimit = -1.0) const;

		void Resize(int cx, int cy);

		//== Keyboard 관련 함수 ======================================================================
		bool Char(UINT nChar, UINT nRepCnt, UINT nFlags);


		//==========================================================================================

	protected:
		void SetSceneFont(CString csFontName, CString csFontSize, CString csFontUnits);

		//== Command 관련 함수 ===========================================================================
	public:
		void CancelCommands();

	public:
		//:Ken - 20230607
		bool KeyboardInput(Json::Object & input);

		//== 환경 변수 관련 함수 ==========================================================================
	protected:
		Facility::KernelOption m_cPreference;

	protected:
		int m_nViewId = -1;
	};
}