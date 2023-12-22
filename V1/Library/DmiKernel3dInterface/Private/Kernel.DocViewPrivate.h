#pragma once

#include <Json.h>

#include "../Kernel.h"
#include "../Object.h"

#include "../Operator.h"
#include "../Operator.HighlightObjectSnap.h"

#include <3DF.Factory.h>
#include <3DF.Canvas.h>
#include <3DF.Model.h>
#include <3DF.View.h>

namespace KERNEL
{
	class DocViewPrivate : public PrivateImpl
	{
	public:
		DocViewPrivate();

		void Copy(const DocViewPrivate * pcInThat)
		{
			m_cCanvas = pcInThat->m_cCanvas;
			m_nViewId = pcInThat->m_nViewId;
			m_pcDelivery = pcInThat->m_pcDelivery;
		}

		H3DF::Canvas & GetCanvas() { return m_cCanvas; }
		H3DF::Canvas m_cCanvas;
		H3DF::Model m_cModel;
		int m_nViewId = -1;

		Signal::Delivery & Delivery();
		const Signal::Delivery & Delivery() const;
		void SetDelivery(const Signal::Delivery * pcInDelivery);

	private:
		const Signal::Delivery * m_pcDelivery = nullptr;

		//== Operator 관련 함수 ======================================================================
	public:
		void AllocationOperator(H3DF::View * pcInView, Signal::Delivery & cDelivery);
		Operator::OperatorBase * GetOperator(Operator::Type eInType);
		Operator::HighlightObjectSnap & HighlightOSnapOperator();

	private:
		Operator::OperatorBase * m_apcOperator[(int)Operator::Type::Count];

		//== Object Snap 관련 함수 ===================================================================
	public:
		DWORD m_nOSnapMode = 0;

	public:
		void SetObjectSnap(OSnap::Type eInType);

		//== Selection Filter 관련 함수 ==============================================================
	public:
		DWORD m_nSelFilter = 0;
		void SetSelectionFilter(SelectionFilter::Type eInType);

		//== Command 관련 함수 =======================================================================
		void CommandRequest(Json::Object & cInObject);
		void CommandChange(Json::Object & cInObject);

	protected:
		void RequestVisualEffects(Json::Object & cInObject);
		void ChangeVisualEffects(Json::Object & cInObject);
	};
}