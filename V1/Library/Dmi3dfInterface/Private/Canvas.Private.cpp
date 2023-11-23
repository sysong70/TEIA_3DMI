#include "StdAfx.h"

#include "Canvas.Private.h"

#include "Common_Define.h"

#include "../3DF.Model.h"
#include "Model.Private.h"

#include "../3DF/3DF.Utility.h"

using namespace H3DF;

//== CanvasPrivate Class ===========================================================================

H3DF::CanvasPrivate::CanvasPrivate()
{	//----- Model 생성 및 초기화 -----
	m_pcModel = new H3DF::Model();
	if (nullptr == m_pcModel) {
		assert(false);
	}

}

H3DF::CanvasPrivate::~CanvasPrivate()
{
	if (nullptr != m_pchName) {
		delete[] m_pchName;
	}
}

void H3DF::CanvasPrivate::Copy(const CanvasPrivate * pcInThat)
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
