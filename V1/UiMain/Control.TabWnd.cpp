#include "stdafx.h"
#include "Control.TabWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

using namespace Control;

BEGIN_MESSAGE_MAP(TabWnd, CBCGPTabWnd)
END_MESSAGE_MAP()



Control::TabWnd::TabWnd()
	: CBCGPTabWnd()
{
	m_bTransparent = TRUE;
	m_bVisualManagerStyle = TRUE;
}



void Control::TabWnd::SetImageList(std::vector<UINT> ids, CSize imageSize)
{
	m_sizeImage = imageSize;

	CBCGPToolBarImages images;
	images.SetImageSize(m_sizeImage);

	for (auto id : ids) {
		CBCGPSVGImage* pImage = new CBCGPSVGImage();
		BOOL success = pImage->Load(id);
		ASSERT(success);
		images.AddSVG(pImage);
	}

	images.ExportToImageList(m_Images);

	if (m_ImagesGray.GetSafeHandle() != nullptr) {
		m_ImagesGray.DeleteImageList();
	}
}



void Control::TabWnd::SetTabHeight(int height)
{
	//:CHECK
	m_sizeImage.cx = height;
	m_sizeImage.cy = height;
}
