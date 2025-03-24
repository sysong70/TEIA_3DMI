#include "stdafx.h"

#include "Ctl.TabWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

BEGIN_MESSAGE_MAP(CtlTabWnd, CBCGPTabWnd)
END_MESSAGE_MAP()



CtlTabWnd::CtlTabWnd()
	: CBCGPTabWnd()
{
	m_bTransparent = TRUE;
	m_bVisualManagerStyle = TRUE;
}



void CtlTabWnd::SetImageList(const Ctl::ResourceIds& ids, CSize imageSize)
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



void CtlTabWnd::SetTabHeight(int height)
{
	//:CHECK
	m_sizeImage.cx = height;
	m_sizeImage.cy = height;
}
