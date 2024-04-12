#include "StdAfx.h"

#include "CanvasImpl.h"

#include "Common_Define.h"

#include "../3DF.Model.h"
#include "ModelImpl.h"

#include "ViewImpl.h"

#include "../../3DF/3DF.Utility.h"

#include <hic.h>

typedef void(HC_CDECL * CallbackFunc)(...);

using namespace H3DF;

//== CanvasPrivate Class ===========================================================================

H3DF::CanvasImpl::CanvasImpl()
{	
	//----- Model 생성 및 초기화 -----
	m_pcModel = new H3DF::Model();
	if (nullptr == m_pcModel) {
		assert(false);
	}

}

H3DF::CanvasImpl::~CanvasImpl()
{
	if (nullptr != m_pchName) {
		delete[] m_pchName;
	}

	// H3DF::ViewImpl::~ViewImpl()에서 m_pcBaseView가 삭제될때 model도 삭제되므로 여기서는 삭제하지 않는다.
// 	if (nullptr != m_pcModel) {
// 		delete m_pcModel;
// 		m_pcModel = nullptr;
// 	}

	for (auto pcView : m_vpcViewArray) {
		delete pcView;
	}

	HC_Relinquish_Memory();
}

void H3DF::CanvasImpl::Copy(const CanvasImpl * pcInThat)
{
	if (nullptr != pcInThat->m_pchName) {
		Utility::CopyString(pcInThat->m_pchName, m_pchName);
	}

	m_pcModel = pcInThat->m_pcModel;

	m_nViewId = pcInThat->m_nViewId;
	m_pcDelivery = pcInThat->m_pcDelivery;

	m_nInWindowHandle = pcInThat->m_nInWindowHandle;
	m_cApplicationWindowOptionsKit = pcInThat->m_cApplicationWindowOptionsKit;

	for (auto pcView : pcInThat->m_vpcViewArray) {
		m_vpcViewArray.push_back(pcView);
	}
}

Signal::Delivery & H3DF::CanvasImpl::Delivery()
{
	((Signal::Delivery *)m_pcDelivery)->ViewId = m_nViewId;
	return *(Signal::Delivery *)m_pcDelivery;
}

const Signal::Delivery & H3DF::CanvasImpl::Delivery() const
{
	((Signal::Delivery *)m_pcDelivery)->ViewId = m_nViewId;
	return *m_pcDelivery;
}

void H3DF::CanvasImpl::SetDelivery(const Signal::Delivery * pcInDelivery, int nViewId)
{
	m_nViewId = nViewId;
	m_pcDelivery = pcInDelivery;
}

//== Callback 관련 함수 ==============================================================================
void H3DF::CanvasImpl::SetFinishPictureCallback()
{
	View * pcView = (View *)(m_pcFrontView);
	DEBUG_VALID(pcView);

	ViewImpl * pcViewImpl = (ViewImpl *)pcView->GetImpl();
	DEBUG_VALID(pcViewImpl);

	HC_KEY nViewKey = pcViewImpl->GetBaseView()->GetViewKey();

	HC_Open_Segment_By_Key(pcViewImpl->GetBaseView()->GetViewKey()); {
		HC_Define_Callback_Name("FinishPictureCallback", (CallbackFunc)CanvasImpl::FinishPictureCallback);
		HC_Set_Callback_With_Data("finish picture = FinishPictureCallback", this);
	} HC_Close_Segment();
}

void H3DF::CanvasImpl::FinishPictureCallback(HIC_Rendition const * pcRendition, bool bSwapBuffers)
{
	void const * pcData = HIC_Show_Callback_Data(pcRendition);
	if (nullptr == pcData) {
		DEBUG_STOP;
		return;
	}

	CanvasImpl * pcImpl = (CanvasImpl *)pcData;
	DEBUG_VALID(pcImpl);

	if (true == pcImpl->m_bInitUpdate) {
		return;
	}

	pcImpl->m_bInitUpdate = true;

/*
	View * pcView = (View *)(pcImpl->m_pcFrontView);
	DEBUG_VALID(pcView);

	ViewImpl * pcViewImpl = (ViewImpl *)pcView->GetImpl();
	DEBUG_VALID(pcViewImpl);

	HC_KEY nViewKey = pcViewImpl->GetBaseView()->GetViewKey();

	HC_Open_Segment_By_Key(pcViewImpl->GetBaseView()->GetViewKey()); {
		HC_UnDefine_Callback_Name("FinishPictureCallback");
	} HC_Close_Segment();
*/

	CString strMessage;
	strMessage.Format(L"Callback Function End");
	pcImpl->Delivery().progress.AddLog(Signal::Progress::Status::Succeed, strMessage);

	pcImpl->Delivery().mainFrame.HideProgress();

	HIC_Finish_Picture(pcRendition, bSwapBuffers);

	//pcImpl->Delivery().view.SetValidation();
}