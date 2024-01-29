#include "StdAfx.h"

#include "Operator.HighlightObjectSnap.h"
#include "Impl/Operator.HighlightObjectSnapImpl.h"

#include <3DF/Window.h>

using namespace KERNEL;

//== ObjectSnap class ==============================================================================
KERNEL::Operator::HighlightObjectSnap::HighlightObjectSnap(const H3DF::View * pcInView, const Signal::Delivery * pcInDelivery)
{
	auto * pcImpl = new HighlightObjectSnapImpl(pcInView, pcInDelivery);
	DEBUG_VALID(pcImpl);

	m_pcImpl = pcImpl;

}

int KERNEL::Operator::HighlightObjectSnap::MouseMove(HEventInfo & cInEvent)
{
	auto * pcImpl = static_cast<HighlightObjectSnapImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	UINT nFlags = cInEvent.GetFlags();

	if (!(MVO_LBUTTON & nFlags) && !(MVO_MBUTTON & nFlags) && !(MVO_RBUTTON & nFlags)) {
		pcImpl->NoButtonDownAndMove(cInEvent);
	}
	else if (MVO_LBUTTON & nFlags) {
		pcImpl->LButtonDownAndMove(cInEvent);
	}

	return HLISTENER_PASS_EVENT;
}

//== Object Snap Point를 그리는 함수 ==================================================================

void KERNEL::Operator::HighlightObjectSnap::DrawSnapItems()
{
	auto * pcImpl = static_cast<HighlightObjectSnapImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	pcImpl->DrawSnapItems();
}

void KERNEL::Operator::HighlightObjectSnap::SetObjectSnapMode(DWORD nInSnapMode)
{
	auto * pcImpl = static_cast<HighlightObjectSnapImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	pcImpl->SetObjectSnapMode(nInSnapMode);
}


void KERNEL::Operator::HighlightObjectSnap::SetSelectionFilter(DWORD nInSelFilter)
{
	auto * pcImpl = static_cast<HighlightObjectSnapImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	pcImpl->SetSelectionFilter(nInSelFilter);
}

H3DF::SelectionItem & KERNEL::Operator::HighlightObjectSnap::DynamicHighlightSelectionItem()
{
	auto * pcImpl = static_cast<HighlightObjectSnapImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->m_cDynamicHighlightSelItem;
}

void KERNEL::Operator::HighlightObjectSnap::UnhighlightEverything()
{
	auto * pcImpl = static_cast<HighlightObjectSnapImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	pcImpl->m_cDynamicHighlightControl.UnhighlightEverything();
	pcImpl->m_cDynLineHighlightCtrl.UnhighlightEverything();
}