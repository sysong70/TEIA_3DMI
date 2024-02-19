#include "StdAfx.h"

#include "CanvasImpl.h"

#include "Common_Define.h"

#include "../3DF.Model.h"
#include "ModelImpl.h"

#include "../3DF/3DF.Utility.h"

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
}

void H3DF::CanvasImpl::Copy(const CanvasImpl * pcInThat)
{
	if (nullptr != pcInThat->m_pchName) {
		Utility::CopyString(pcInThat->m_pchName, m_pchName);
	}

	m_nInWindowHandle = pcInThat->m_nInWindowHandle;
	m_cApplicationWindowOptionsKit = pcInThat->m_cApplicationWindowOptionsKit;

	for (auto pcView : pcInThat->m_vpcViewArray) {
		m_vpcViewArray.push_back(pcView);
	}
}
