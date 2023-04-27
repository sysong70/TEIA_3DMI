#include "StdAfx.h"

#include "3DF.WindowPrivate.h"

USING_3DF_NAMESPACE

WindowKeyPrivate::WindowKeyPrivate()
{
	m_pnSelectBufferKey = new HC_KEY[m_nSelectBufferKeyCount];
}

WindowKeyPrivate::~WindowKeyPrivate()
{
	if (nullptr != m_pnSelectBufferKey) {
		delete[] m_pnSelectBufferKey;
	}
}

void WindowKeyPrivate::Copy(WindowKeyPrivate * pcInThat)
{
	m_pcBaseView = pcInThat->m_pcBaseView;
	m_nViewId = pcInThat->m_nViewId;
}

// m_nSelectBufferKeyCount의 값이 nCount보다 작으면 m_pnSelectBufferKey를 재할당한다.
// m_nSelectBufferKeyCount는 nCount가 됨.
HC_KEY * WindowKeyPrivate::GetSelectBufferKey(int nCount)
{
	if (m_nSelectBufferKeyCount < nCount) {
		delete[] m_pnSelectBufferKey;
		m_pnSelectBufferKey = new HC_KEY[nCount];
		m_nSelectBufferKeyCount = nCount;
	}

	return m_pnSelectBufferKey;
}

