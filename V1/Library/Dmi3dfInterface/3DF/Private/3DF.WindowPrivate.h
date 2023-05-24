#pragma once

#include "../3DF.h"
#include "../3DF.Object.h"

#include <hc.h>

OPEN_3DF_NAMESPACE

class BaseView;

class WindowKeyPrivate : public PrivateImpl
{
public:
	WindowKeyPrivate();
	virtual ~WindowKeyPrivate();

	void Copy(WindowKeyPrivate * pcInThat);

	TDF::BaseView * GetBaseView() { return (TDF::BaseView *) m_pcBaseView; }
	const TDF::BaseView * m_pcBaseView = nullptr;

	int m_nViewId = -1;

	HC_KEY * GetSelectBufferKey(int nCount);

private:
	int m_nSelectBufferKeyCount = 64;
	HC_KEY * m_pnSelectBufferKey = nullptr;
};

CLOSE_3DF_NAMESPACE