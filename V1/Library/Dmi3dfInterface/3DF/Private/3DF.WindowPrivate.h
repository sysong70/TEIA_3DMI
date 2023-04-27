#pragma once

#include "../3DF.h"
#include "../3DF.Object.h"

#include <hc.h>

class HBaseView;

OPEN_3DF_NAMESPACE

class WindowKeyPrivate : public PrivateImpl
{
public:
	WindowKeyPrivate();
	virtual ~WindowKeyPrivate();

	void Copy(WindowKeyPrivate * pcInThat);

	HBaseView * GetBaseView() { return (HBaseView *)m_pcBaseView; }
	const HBaseView * m_pcBaseView = nullptr;

	int m_nViewId = -1;

	HC_KEY * GetSelectBufferKey(int nCount);

private:
	int m_nSelectBufferKeyCount = 64;
	HC_KEY * m_pnSelectBufferKey = nullptr;
};

CLOSE_3DF_NAMESPACE