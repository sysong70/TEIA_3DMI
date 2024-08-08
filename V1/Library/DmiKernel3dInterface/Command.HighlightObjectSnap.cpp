#include "StdAfx.h"

#include "Command.HighlightObjectSnap.h"
#include "Impl/Command.HighlightObjectSnap.Impl.h"

#include <3DF/Window.h>

using namespace KERNEL;

//== ObjectSnap class ==============================================================================
KERNEL::Command::HighlightObjectSnap::HighlightObjectSnap(const Session * pcInSession)
	: Set(pcInSession)
{
	auto * pcImpl = new HighlightObjectSnapImpl(pcInSession);
	DEBUG_VALID(pcImpl);

	m_pcImpl = pcImpl;

}

Command::Result::Type KERNEL::Command::HighlightObjectSnap::MouseMove(Event & cInEvent)
{
	auto * pcImpl = static_cast<HighlightObjectSnapImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	if (false == cInEvent.LButton() && false == cInEvent.MButton() && false == cInEvent.RButton()) {
		pcImpl->NoButtonDownAndMove(cInEvent);
	}
	else if (true == cInEvent.LButton()) {
		pcImpl->LButtonDownAndMove(cInEvent);
	}

	return Command::Result::Type::Pass;
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