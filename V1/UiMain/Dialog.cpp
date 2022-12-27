#include "stdafx.h"
#include "Dialog.h"



CSize Dialog::ControlGap()
{
	return globalUtils.ScaleByDPI(CSize(6, 6));
}



int Dialog::FooterPadding()
{
	return globalUtils.ScaleByDPI(6);
}



CSize Dialog::GetControlSize(CWnd* pControl)
{
	DEBUG_VALID(pControl);

	CRect rect;
	if (pControl != nullptr && pControl->GetSafeHwnd() != nullptr) {
		pControl->GetClientRect(&rect);
	}
	else {
		DEBUG_STOP;
	}

	return { rect.Width(), rect.Height() };
}
