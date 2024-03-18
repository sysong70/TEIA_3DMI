#include "stdafx.h"
#include "Graphic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

//:TEST
/*
CBCGPGraphicsManager* m_pGraphics = nullptr;

void DrawCircleOnView(const CPoint center, const CSize radiusSize, COLORREF color)
{
	if (m_pGraphics == nullptr) {
		m_pGraphics = CBCGPGraphicsManager::CreateInstance(CBCGPGraphicsManager::BCGP_GRAPHICS_MANAGER_D2D);
		DEBUG_VALID(m_pGraphics);
	}

	m_pGraphics->BindDC(GetDC());

	if (m_pGraphics->BeginDraw() == FALSE) {
		DEBUG_RETURN;
	}

	//:CHECK
	//CBCGPPoint from(center.x + radiusSize.cx, center.y + radiusSize.cy);
	//m_pGraphics->DrawArc(from, from, radiusSize, TRUE, TRUE, CBCGPBrush(color));
	//m_pGraphics->DrawRectangle({ center.x - 20.0, center.y - 20.0, center.x + 20.0, center.y + 20.0 }, CBCGPBrush(color));

	//:TEST- random
	for (int i = 0; i < 100; i++) {
		double x = rand() % 4000;
		double y = rand() % 1500;
		m_pGraphics->DrawRectangle({ x - 20.0, y - 20.0, x + 20.0, y + 20.0 }, CBCGPBrush(color));
	}

	m_pGraphics->EndDraw();
}
*/