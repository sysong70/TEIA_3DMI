#pragma once

#include <Json.h>

#include "../Kernel.h"
#include "../Object.h"

#include "../../Dmi3dfInterface/3DF.Canvas.h"
#include "../../Dmi3dfInterface/3DF.Factory.h"
#include "../../Dmi3dfInterface/3DF.View.h"

namespace KERNEL
{
	class ViewPrivate : public PrivateImpl
	{
	public:
		void Copy(const ViewPrivate * pcInThat)
		{
			m_cCanvas = pcInThat->m_cCanvas;
			m_nViewId = pcInThat->m_nViewId;
		}

		H3DF::Canvas & GetCanvas() { return m_cCanvas; }

		H3DF::Canvas m_cCanvas;
		int m_nViewId = -1;

	public:
		//== Visual Effects 관련 함수 ================================================================
		void SetVisualEffectsShadow();
		void SetVisualEffectsReflection();
		void SetVisualEffectsAmbientOcclusion();
	};
}