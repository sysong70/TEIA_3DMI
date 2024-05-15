//////////////#pragma once

#include <Json.h>

#include "../Kernel.h"
#include "../Object.h"

#include "../Command.h"
#include "../Command.HighlightObjectSnap.h"
#include "../Command.Camera.h"
#include "../Command.Select.h"
#include "../Command.ModelPanel.h"
#include "../Command.Attribute.h"

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
			m_strFilePathName = pcInThat->m_strFilePathName;
		}

		int m_nViewId = -1;

		H3DF::Canvas & GetCanvas() { return *m_pcCanvas; }
		H3DF::Canvas * m_pcCanvas = nullptr;

		H3DF::Model & GetModel();
		H3DF::CADModel & CADModel();
	
		H3DF::BaseView * GetBaseView();

		Signal::Delivery & Delivery();
		const Signal::Delivery & Delivery() const;
		void SetDelivery(const Signal::Delivery * pcInDelivery);

		void CancelCommands();

		//== Operator 관련 함수 ======================================================================
		void AllocationOperator(const DocView * pcInDocView);
		Command::CommandBase * GetOperator(Command::Type eInType);

		Command::Attribute & Attribute();
		Command::Camera & Camera();
		Command::Select & Select();
		Command::ModelPanel & ModelPanel();

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
		Command::CommandBase * m_apcOperator[(int)Command::Type::Count];
		const Signal::Delivery * m_pcDelivery = nullptr;

		H3DF::CADModel m_cCADModel;

	public:
		// 현재 선택된 요소들이 저장되는 변수
		H3DF::SelectionResults m_cSelectionResult;

		H3DF::Point2D m_cLButtonDownPosition;

		DWORD m_nMouseWhellStartTick;

		CString m_strFilePathName;
	};
}