#pragma once

#include <Json.h>

#include "../Kernel.h"
#include "../Object.h"

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
		Signal::Delivery * m_pcDelivery = nullptr;

		//== Visual Effects 관련 함수 ================================================================
		void SetVisualEffectsShadow();
		void SetVisualEffectsReflection();
		void SetVisualEffectsAmbientOcclusion();
		void SetVisualEffectsSilhouetteEdges();
		void SetVisualEffectsBloom();

		//== Object Snap 관련 함수 ===================================================================
		Operator::HighlightObjectSnap * m_pcObjectSnapOperator = nullptr;
		DWORD m_nOSnapMode = 0;

		void SetObjectSnap(OSnap::Type eInType);

		//== Selection Filter 관련 함수 ==============================================================
		DWORD m_nSelectionFilter = 0;
		void SetSelectionFilter(SelectionFilter::Type eInType);

		//== Command 관련 함수 =======================================================================
		void RequestVisualEffectsSetting(Json::Object & cInObject);
	private:
		Json::Object * m_pcVisualEffectsSetting = nullptr;



	};
}