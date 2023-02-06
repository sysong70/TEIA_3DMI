#include "stdafx.h"
#include "Component.TabWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



using namespace Component;

BEGIN_MESSAGE_MAP(TabWnd, CBCGPTabWnd)
END_MESSAGE_MAP()



Component::TabWnd::TabWnd()
	: CBCGPTabWnd()
{
	m_bTransparent = TRUE;
	m_bVisualManagerStyle = TRUE;
}



void Component::TabWnd::SetImageList(std::vector<UINT> ids, CSize imageSize)
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

	if (m_ImagesGray.GetSafeHandle() != NULL) {
		m_ImagesGray.DeleteImageList();
	}
}



void Component::TabWnd::SetTabHeight(int height)
{
	//:CHECK
	m_sizeImage.cx = height;
	m_sizeImage.cy = height;
}
