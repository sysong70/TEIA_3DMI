#include "StdAfx.h"

#include "Command.HighlightObjectSnap.h"
#include "Impl/Command.HighlightObjectSnapImpl.h"

#include <3DF/Window.h>

using namespace KERNEL;

//== ObjectSnap class ==============================================================================
KERNEL::Command::HighlightObjectSnap::HighlightObjectSnap(const DocView * pcInDocView)
{
	auto * pcImpl = new HighlightObjectSnapImpl(pcInDocView);
	DEBUG_VALID(pcImpl);

	m_pcImpl = pcImpl;

}

int KERNEL::Command::HighlightObjectSnap::MouseMove(HEventInfo & cInEvent)
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

void KERNEL::Command::HighlightObjectSnap::DrawSnapItems()
{
	auto * pcImpl = static_cast<HighlightObjectSnapImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	pcImpl->DrawSnapItems();
}

void KERNEL::Command::HighlightObjectSnap::SetObjectSnapMode(DWORD nInSnapMode)
{
	auto * pcImpl = static_cast<HighlightObjectSnapImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	pcImpl->SetObjectSnapMode(nInSnapMode);
}


void KERNEL::Command::HighlightObjectSnap::SetSelectionFilter(DWORD nInSelFilter)
{
	auto * pcImpl = static_cast<HighlightObjectSnapImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	pcImpl->SetSelectionFilter(nInSelFilter);
}

H3DF::SelectionItem & KERNEL::Command::HighlightObjectSnap::DynamicHighlightSelectionItem()
{
	auto * pcImpl = static_cast<HighlightObjectSnapImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->m_cDynamicHighlightSelItem;
}

void KERNEL::Command::HighlightObjectSnap::UnhighlightEverything()
{
	auto * pcImpl = static_cast<HighlightObjectSnapImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	pcImpl->m_cDynHighlightControl.UnhighlightEverything();
	pcImpl->m_cDynLineHighlightCtrl.UnhighlightEverything();
	pcImpl->m_cDynPmiHighlightCtrl.UnhighlightEverything();
}

void KERNEL::Command::HighlightObjectSnap::Reset(bool bUpdate)
{
	auto * pcImpl = static_cast<HighlightObjectSnapImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	pcImpl->Reset(bUpdate);
}