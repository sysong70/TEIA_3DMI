#pragma once

#include "../../3DF/3DF.h"

#include "../../3DF/Window.h"
#include "../../3DF/ApplicationWindowOption.h"

#include <HDB.h>
#include <HBaseModel.h>
#include <HTools.h>
#include <HStream.h>
#include <HUtility.h>
#include <HIOManager.h>

#include "../3DF.View.h"

#include <chrono>

namespace H3DF
{
	class MultiSelectManager;

	class CanvasImpl : public Impl
	{
	public:
		CanvasImpl();
		virtual ~CanvasImpl();

		void Copy(const CanvasImpl * pcInThat);

		Signal::Delivery & Delivery();
		const Signal::Delivery & Delivery() const;
		void SetDelivery(const Signal::Delivery * pcInDelivery, int nViewId);

		// 초기 입력 변수값
		H3DF::WindowHandle m_nInWindowHandle = 0;
		char * m_pchName = NULL;
		H3DF::ApplicationWindowOptionsKit m_cApplicationWindowOptionsKit;

		std::vector<H3DF::View> m_vcViewArray;

		WindowKey m_cWindowKey;

		H3DF::Model * m_pcModel = nullptr;

		int m_nViewId = -1;
		const Signal::Delivery * m_pcDelivery = nullptr;

		bool m_bPointColudData = false;
		bool m_bInitUpdate = false;

		std::chrono::system_clock::time_point m_cTimes[3];

		//== Callback 관련 함수 ======================================================================
		void SetFinishPictureCallback();
		static void FinishPictureCallback(HIC_Rendition const * pcRendition, bool bSwapBuffers);
	};
}