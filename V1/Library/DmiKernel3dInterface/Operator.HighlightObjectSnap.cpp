#include "StdAfx.h"

#include "OPERATOR.HighlightObjectSnap.h"
#include "Private/OPERATOR.HighlightObjectSnapPrivate.h"

#include <3DF/Window.h>

using namespace KERNEL;

//== ObjectSnap class ==============================================================================

KERNEL::Operator::HighlightObjectSnap::HighlightObjectSnap()
{
	auto * pcImpl = new HighlightObjectSnapPrivate();
	DEBUG_VALID(pcImpl);

	m_pcImpl = pcImpl;
}

KERNEL::Operator::HighlightObjectSnap::HighlightObjectSnap(H3DF::WindowKey * pcWindow)
{
	auto * pcImpl = new HighlightObjectSnapPrivate(pcWindow);
	DEBUG_VALID(pcImpl);

	m_pcImpl = pcImpl;
}

int KERNEL::Operator::HighlightObjectSnap::LButtonDownAndMove(int nFlags, int x, int y)
{
	auto * pcImpl = static_cast<HighlightObjectSnapPrivate *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->LButtonDownAndMove(nFlags, x, y);
}

int KERNEL::Operator::HighlightObjectSnap::NoButtonDownAndMove(int nFlags, int x, int y)
{
	auto * pcImpl = static_cast<HighlightObjectSnapPrivate *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->NoButtonDownAndMove(nFlags, x, y);
}

//== Object Snap Point를 그리는 함수 ==================================================================

void KERNEL::Operator::HighlightObjectSnap::DrawSnapItems()
{
	auto * pcImpl = static_cast<HighlightObjectSnapPrivate *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	pcImpl->DrawSnapItems();
}

void KERNEL::Operator::HighlightObjectSnap::SetObjectSnapMode(DWORD nInSnapMode)
{
	auto * pcImpl = static_cast<HighlightObjectSnapPrivate *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	pcImpl->SetObjectSnapMode(nInSnapMode);
}