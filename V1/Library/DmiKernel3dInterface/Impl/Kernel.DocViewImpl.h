//////////////#pragma once

#include <Json.h>

#include "../Kernel.h"
#include "../Object.h"

#include "../Operator.h"
#include "../Operator.HighlightObjectSnap.h"
#include "../Operator.Camera.h"
#include "../Operator.Select.h"

#include <3DF.Factory.h>
#include <3DF.Canvas.h>
#include <3DF.Model.h>
#include <3DF.View.h>
#include <3DF/Selection.h>
#include <3DF/Highlight.h>

namespace KERNEL
{
	class DocViewImpl : public Impl
	{
	public:
		DocViewImpl();

		void Copy(const DocViewImpl * pcInThat)
		{
			m_cCanvas = pcInThat->m_cCanvas;
			m_nViewId = pcInThat->m_nViewId;
			m_pcDelivery = pcInThat->m_pcDelivery;
		}

		int m_nViewId = -1;

		H3DF::Canvas & GetCanvas() { return m_cCanvas; }
		H3DF::Canvas m_cCanvas;
		H3DF::Model m_cModel;
	
		H3DF::BaseView * GetBaseView();

		Signal::Delivery & Delivery();
		const Signal::Delivery & Delivery() const;
		void SetDelivery(const Signal::Delivery * pcInDelivery);

		void CancelCommands();

		//== Operator 관련 함수 ======================================================================
		void AllocationOperator(H3DF::View * pcInView, Signal::Delivery & cDelivery);
		Operator::OperatorBase * GetOperator(Operator::Type eInType);
		Operator::HighlightObjectSnap & HighlightOSnapOperator();
		Operator::Camera & Camera();
		Operator::Select & Select();

		DWORD MouseMapFlags(DWORD nState);

		//== Object Snap 관련 함수 ===================================================================
		void SetObjectSnap(OSnap::Type eInType);

		//== Select 관련 함수 ========================================================================
		H3DF::HighlightControl & HighlightControl() { return *m_pcHighlightControl; }

		//== Selection Filter 관련 함수 ==============================================================
		void SetSelectionFilter(SelectionFilter::Type eInType);

		//== Visibility 관련 함수 ====================================================================
		void SetVisibility(int nId);

		//== Command 관련 함수 =======================================================================
		void CommandRequest(Json::Object & cInObject);
		void CommandChange(Json::Object & cInObject);

	protected:
		void RequestVisualEffects(Json::Object & cInObject);
		void ChangeVisualEffects(Json::Object & cInObject);

	private:
		Operator::OperatorBase * m_apcOperator[(int)Operator::Type::Count];
		const Signal::Delivery * m_pcDelivery = nullptr;

	public:
		DWORD m_nOSnapMode = 0;
		// 화면에 선택된 상태를 표시하기 위한 Highlight control
		H3DF::HighlightControl * m_pcHighlightControl = nullptr;
		DWORD m_nSelFilter = 0;

		// 현재 선택된 요소들이 저장되는 변수
		H3DF::SelectionResults m_cSelectionResult;

		DWORD m_nMouseWhellStartTick;
	};
}