#include "StdAfx.h"

#include "WindowImpl.h"

#include "../../Impl/ViewImpl.h"

using namespace H3DF;

H3DF::WindowKeyImpl::WindowKeyImpl()
{
	m_pnSelectBufferKey = new HC_KEY[m_nSelectBufferKeyCount];
}

H3DF::WindowKeyImpl::~WindowKeyImpl()
{
	if (nullptr != m_pnSelectBufferKey) {
		delete[] m_pnSelectBufferKey;
	}
}

void H3DF::WindowKeyImpl::Copy(WindowKeyImpl * pcInThat)
{
	m_pcBaseView = pcInThat->m_pcBaseView;
	m_nViewId = pcInThat->m_nViewId;

	m_pcSelection = pcInThat->m_pcSelection;
	m_pcHighlight = pcInThat->m_pcHighlight;
	m_pcSelectionOptions = pcInThat->m_pcSelectionOptions;

	m_cSelectionOptionsKit = pcInThat->m_cSelectionOptionsKit;
}

HC_KEY H3DF::WindowKeyImpl::GetSceneKey()
{
	return GetBaseView()->GetSceneKey();
}

const HC_KEY H3DF::WindowKeyImpl::GetSceneKey() const
{
	return ((BaseView *) m_pcBaseView)->GetSceneKey();
}

// m_nSelectBufferKeyCount의 값이 nCount보다 작으면 m_pnSelectBufferKey를 재할당한다.
// m_nSelectBufferKeyCount는 nCount가 됨.
HC_KEY * H3DF::WindowKeyImpl::GetSelectBufferKey(int nCount)
{
	if (m_nSelectBufferKeyCount < nCount) {
		delete[] m_pnSelectBufferKey;
		m_pnSelectBufferKey = new HC_KEY[nCount];
		m_nSelectBufferKeyCount = nCount;
	}

	return m_pnSelectBufferKey;
}

