#pragma once

#include <Json.h>

#include <map>
#include <chrono>

#include "3DF/3DF.h"

#include "Sprocket.h"

#include "3DF/Window.h"

#include "3DF.Signal.h"
#include "../Signal/Signal.h"

namespace H3DF
{
	class BaseView;
	class Canvas;

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
		void Update(Window::UpdateType eInType, H3DF::Time dInTimeLimit = -1.0) const;

		//==========================================================================================

		void Initialize(Json::Object & cInObject, Signal::Delivery & cInstance);
		void Initialize_OLD(Json::Object & cInObject, Signal::Delivery & cInstance);
		void Destruct();
		void Destruct_OLD();
		void Paint(Json::Object & cInObject);
		void Resize(int x, int y);

		void SaveHsfFile(CString strFilePathName, H3DF::Canvas * pcHoopsView);

		void LoadPointCloudFile(CString strFilePathName, H3DF::Canvas * pcHoopsView);

		//== Command 관련 함수 =======================================================================
		void CancelCommands();

		//== Mouse 관련 함수 =========================================================================
		//bool ExecuteMouseSignal(int nAction, Json::Object & cInObject);

		bool LButtonUp(int nFlags, int x, int y);
		bool LButtonDown(int nFlags, int x, int y);

		bool MButtonUp(int nFlags, int x, int y);
		bool MButtonDown(int nFlags, int x, int y);

		bool RButtonUp(int nFlags, int x, int y);
		bool RButtonDown(int nFlags, int x, int y);

		bool MouseMove(int nFlags, int x, int y);

		bool MouseWheel(int nFlags, int zDelta, int x, int y, int nLeft, int nTop);

	public:
		//:Ken - 20230607
		bool ExecuteKeyboardSignal(int nAction, Json::Object & cInObject);

	private:
		H3DF::Canvas * m_pcCanvas = nullptr;
		H3DF::Model * m_pcModel = nullptr;

		H3DF::BaseView * m_pcBaseView = nullptr;
		H3DF::WindowKey * m_pcWindow = nullptr;
	};
};