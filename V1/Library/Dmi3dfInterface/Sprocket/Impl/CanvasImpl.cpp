#include "StdAfx.h"

#include "CanvasImpl.h"

#include "Common_Define.h"

#include "../3DF.Model.h"
#include "ModelImpl.h"

#include "../../3DF/3DF.Utility.h"

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
