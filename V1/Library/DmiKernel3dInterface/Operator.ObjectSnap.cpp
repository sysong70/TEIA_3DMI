#include "StdAfx.h"

#include "Operator.ObjectSnap.h"
#include "Private/Operator.ObjectSnapPrivate.h"

#include <3DF/Window.h>

using namespace KERNEL;

//== ObjectSnap class ==============================================================================

KERNEL::Operator::ObjectSnap::ObjectSnap(H3DF::WindowKey * pcWindow)
{
	ObjectSnapPrivate * pcImpl = new ObjectSnapPrivate(pcWindow);
	DEBUG_VALID(pcImpl);

	m_pcImpl = pcImpl;
}

int KERNEL::Operator::ObjectSnap::NoButtonDownAndMove(int nFlags, int x, int y)
{
	ObjectSnapPrivate * pcImpl = static_cast<ObjectSnapPrivate *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->NoButtonDownAndMove(nFlags, x, y);
}

//== Object Snap Point를 그리는 함수 ==================================================================

void KERNEL::Operator::ObjectSnap::DrawSnapItems()
{
	ObjectSnapPrivate * pcImpl = static_cast<ObjectSnapPrivate *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	pcImpl->DrawSnapItems();
}

void KERNEL::Operator::ObjectSnap::SetObjectSnapMode(DWORD nInSnapMode)
{
	ObjectSnapPrivate * pcImpl = static_cast<ObjectSnapPrivate *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	pcImpl->SetObjectSnapMode(nInSnapMode);
}