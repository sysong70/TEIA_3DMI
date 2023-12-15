#include "StdAfx.h"

#include "OPERATOR.SelectionFilter.h"
#include "Private/OPERATOR.HighlightObjectSnapPrivate.h"

#include <3DF/Window.h>

using namespace KERNEL;

//== Selection Filter class ========================================================================

KERNEL::Operator::SelectionFilter::SelectionFilter(H3DF::WindowKey * pcWindow)
{
	HighlightObjectSnapPrivate * pcImpl = new HighlightObjectSnapPrivate(pcWindow);
	DEBUG_VALID(pcImpl);

	m_pcImpl = pcImpl;
}

void KERNEL::Operator::SelectionFilter::SetObjectSnapMode(DWORD nInSnapMode)
{
	HighlightObjectSnapPrivate * pcImpl = static_cast<HighlightObjectSnapPrivate *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	pcImpl->SetObjectSnapMode(nInSnapMode);
}