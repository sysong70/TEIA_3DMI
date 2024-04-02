#pragma once

#include <Json.h>

#include "Kernel.h"
#include "Object.h"

#include "../Dmi3dfInterface/Sprocket/3DF.View.h"

namespace KERNEL
{
	// Kernel View는 CDocument의 CView의 신호를 처리하는 역활을 한다.
	// CDoucment 대응하는 함수는 없기 때문에, Kernel View에서 처리하도록 한다.
	class API_KERNEL DocView : public Object
	{
	public:
		DocView();
		KERNEL::Type ObjectType() const { return KERNEL::Type::View; };

		void SetDelivery(Signal::Delivery & cDelivery);

		void Initialize(Json::Object & cInObject);
		void FileOpenTimer();

		void Paint(Json::Object & cInObject);
		void Resize(Json::Object & cInObject);
		void CancelCommands();

		void ViewId(int nViewId);
		int ViewId();

		HWND GetHwnd();

		H3DF::Canvas & Canvas() const;
		H3DF::CADModel & CADModel() const;

		bool Save(WCHAR * pstrInFilePathName);

		//== Operator 관련 함수 ======================================================================
		Operator::Attribute & Attribute();
		Operator::Camera & Camera();
		Operator::Select & Select();
		Operator::ModelPanel & ModelPanel();

		//== Mouse 관련 함수 =========================================================================
		void MouseSignal(Json::Object & cInObject);
		
		void MouseMove(int nFlag, int x, int y);

		void LButtonDown(int nFlag, int x, int y);
		void LButtonUp(int nFlag, int x, int y);

		void RButtonDown(int nFlag, int x, int y);
		void RButtonUp(int nFlag, int x, int y);

		void MouseWheel(int nFlag, int x, int y, Json::Object & cInObject);

		//== Keyboard 관련 함수 ======================================================================
		void KeyboardSignal(Json::Object & cInObject);

		//== View 관련 함수 ==========================================================================
		void SetViewControl(int nId);

		//== Object Snap 관련 함수 ===================================================================
		void SetObjectSnap(int nId);

		//== Selection Fiter 관련 함수 ===============================================================
		void SetSelectionFilter(int nId);

		//== Visibility 관련 함수 ====================================================================
		void SetVisibility(int nId);

		//== Measure 관련 함수 =======================================================================
		void SetMeasure(int nId);
		
		//== Style 관련 함수 =========================================================================
		void SetViewStyle(int nStyleId);
		void SetViewDirection(int nDirectionId);

		//== Command 관련 함수 =======================================================================
		void CommandRequest(Json::Object & cInObject);
		void CommandChange(Json::Object & cInObject);

		//== Panel 관련 함수 =========================================================================
		void ModelPanelSignal(Json::Object & cInObject);

		//== 임시 Test용 함수 ========================================================================
		void TestCommand(int nId);
	};
};