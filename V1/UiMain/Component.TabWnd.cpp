#include "stdafx.h"
#include "Component.TabWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



Component::TabWnd::TabWnd()
	: CBCGPTabWnd()
{
}



void Component::TabWnd::SetImageSize(CSize size)
{
	m_sizeImage = size;
}



void Component::TabWnd::AddImages(std::vector<UINT> ids)
{
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
