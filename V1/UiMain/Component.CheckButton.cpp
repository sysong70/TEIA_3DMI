#include "stdafx.h"
#include "Component.CheckButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



#define PRESET PresetCheckButton

namespace PresetCheckButton
{
	int Gap()
	{
		return globalUtils.ScaleByDPI(4);
	}

	CSize ImageSize()
	{
		return globalUtils.ScaleByDPI(CSize(24, 24));
	}
}



using namespace Component;

BEGIN_MESSAGE_MAP(CheckButton, CWnd)
	ON_WM_LBUTTONUP()
	ON_WM_SIZE()
END_MESSAGE_MAP()



Component::CheckButton::CheckButton()
{
}



Component::CheckButton::~CheckButton()
{
}



void Component::CheckButton::PostNcDestroy()
{
	__super::PostNcDestroy();
}



void Component::CheckButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO

	CWnd::OnLButtonUp(nFlags, point);
}



void Component::CheckButton::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);


}

#undef PRESET
