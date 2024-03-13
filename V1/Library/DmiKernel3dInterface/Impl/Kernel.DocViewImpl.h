//////////////#pragma once

#include <Json.h>

#include "../Kernel.h"
#include "../Object.h"

#include "../Operator.h"
#include "../Operator.HighlightObjectSnap.h"
#include "../Operator.Camera.h"
#include "../Operator.Select.h"
#include "../Operator.ModelPanel.h"
#include "../Operator.Attribute.h"

#include <Sprocket/3DF.Factory.h>
#include <Sprocket/3DF.Canvas.h>
#include <Sprocket/3DF.Model.h>
#include <Sprocket/3DF.View.h>
#include <3DF/Selection.h>
#include <3DF/Highlight.h>

#include <Sprocket/3DF.CADModel.h>

namespace KERNEL
{
	class DocViewImpl : public Impl
	{
	public:
		DocViewImpl();
		~DocViewImpl();

		void Copy(const DocViewImpl * pcInThat)
		{
			m_pcCanvas = pcInThat->m_pcCanvas;
			m_nViewId = pcInThat->m_nViewId;
			m_pcDelivery = pcInThat->m_pcDelivery;
		}

		int m_nViewId = -1;

		H3DF::Canvas & GetCanvas() { return *m_pcCanvas; }
		H3DF::Canvas * m_pcCanvas = nullptr;

		H3DF::Model & GetModel();
		H3DF::CADModel & GetCADModel();
	
		H3DF::BaseView * GetBaseView();

		Signal::Delivery & Delivery();
		const Signal::Delivery & Delivery() const;
		void SetDelivery(const Signal::Delivery * pcInDelivery);

		void CancelCommands();

		//== Operator 관련 함수 ======================================================================
		void AllocationOperator(const DocView * pcInDocView);
		Operator::OperatorBase * GetOperator(Operator::Type eInType);

		Operator::Attribute & Attribute();
		Operator::Camera & Camera();
		Operator::Select & Select();
		Operator::ModelPanel & ModelPanel();

		DWORD MouseMapFlags(DWORD nState);

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

		H3DF::CADModel m_cCADModel;

	public:
		// 현재 선택된 요소들이 저장되는 변수
		H3DF::SelectionResults m_cSelectionResult;

		H3DF::Point2D m_cLButtonDownPosition;

		DWORD m_nMouseWhellStartTick;
	};
}