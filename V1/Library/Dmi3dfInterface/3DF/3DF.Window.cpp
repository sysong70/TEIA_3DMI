#include "StdAfx.h"

#include "3DF.Window.h"
#include "Private/3DF.WindowPrivate.h"

#include <hc.h>
#include <HBaseOperator.h>

USING_3DF_NAMESPACE

// public HBaseView, public HAnimationListener


WindowKey::WindowKey(HBaseView * pcBaseView)
{
	WindowKeyPrivate * pcImpl = new WindowKeyPrivate();
	pcImpl->m_pcBaseView = static_cast<HBaseView *>(pcBaseView);
	m_pcImpl = pcImpl;

	// Initialize();
}

WindowKey::WindowKey(WindowKey const & cInThat)
{
	WindowKeyPrivate * pcImpl = new WindowKeyPrivate();
	pcImpl->m_pcBaseView = ((WindowKeyPrivate *)cInThat.m_pcImpl)->m_pcBaseView;
	m_pcImpl = pcImpl;
}

const HBaseView * WindowKey::GetBaseView() const
{
	WindowKeyPrivate * pcImpl = static_cast<WindowKeyPrivate *>(m_pcImpl);
	return pcImpl->m_pcBaseView;
}

HBaseView * WindowKey::GetBaseView()
{
	WindowKeyPrivate * pcImpl = static_cast<WindowKeyPrivate *>(m_pcImpl);
	return pcImpl->GetBaseView();
}

HC_KEY WindowKey::GetSceneKey()
{
	return GetBaseView()->GetSceneKey();
}

const HC_KEY WindowKey::GetSceneKey() const
{
	WindowKeyPrivate * pcImpl = static_cast<WindowKeyPrivate *>(m_pcImpl);
	return pcImpl->GetBaseView()->GetSceneKey();
}

void WindowKey::Update()
{
	GetBaseView()->Update();
}

int WindowKey::ViewId()
{
	WindowKeyPrivate * pcImpl = static_cast<WindowKeyPrivate *>(m_pcImpl);
	return pcImpl->m_nViewId;
}

const int WindowKey::ViewId() const
{ 
	WindowKeyPrivate * pcImpl = static_cast<WindowKeyPrivate *>(m_pcImpl);
	return pcImpl->m_nViewId;
}

void WindowKey::SetViewId(int nViewId) 
{ 
	WindowKeyPrivate * pcImpl = static_cast<WindowKeyPrivate *>(m_pcImpl);
	pcImpl->m_nViewId = nViewId;
}

void WindowKey::Initialize()
{
	WindowKeyPrivate * pcImpl = static_cast<WindowKeyPrivate *>(m_pcImpl);

//	HBaseView * pcBaseView = pcImpl->m_pcBaseView;
}

int WindowKey::OnMouseMove(HEventInfo & cEvent)
{
	WindowKeyPrivate * pcImpl = static_cast<WindowKeyPrivate *>(m_pcImpl);

	int nResult = HOP_OK;

	if (cEvent.LButton() && !cEvent.RButton()) {
		nResult = pcImpl->GetBaseView()->OnLButtonDownAndMove(cEvent);
	}
	else if (cEvent.RButton() && !cEvent.LButton()) {
		nResult = pcImpl->GetBaseView()->OnRButtonDownAndMove(cEvent);
	}
	else if (cEvent.MButton() && !cEvent.LButton() && !cEvent.RButton()) {
		nResult = pcImpl->GetBaseView()->OnMButtonDownAndMove(cEvent);
	}
	else if (cEvent.LButton() && cEvent.RButton()) {
		nResult = pcImpl->GetBaseView()->OnLRButtonDownAndMove(cEvent);
	}
	else if (!cEvent.LButton() && !cEvent.RButton()) {
// 		HPoint  new_pos;
// 		new_pos = event.GetMouseWindowPos();
// 
// 		DoDynamicHighlighting(new_pos);
// 		return HLISTENER_PASS_EVENT;

		nResult = pcImpl->GetBaseView()->OnNoButtonDownAndMove(cEvent);
	}

	return nResult;
}

SelectionOptionsControl WindowKey::GetSelectionOptionsControl()
{
	SelectionOptionsControl cSelectionOptions(*this);
	return cSelectionOptions;
}

SelectionOptionsControl const WindowKey::GetSelectionOptionsControl() const
{
	SelectionOptionsControl cSelectionOptions(*this);
	return cSelectionOptions;
}

SelectionControl WindowKey::GetSelectionControl()
{
	SelectionControl cSelection(*this);
	return cSelection;
}

SelectionControl const WindowKey::GetSelectionControl() const
{
	SelectionControl cSelection(*this);
	return cSelection;
}

HighlightControl WindowKey::GetHighlightControl()
{
	HighlightControl cHighlight(*this);
	return cHighlight;
}

HighlightControl const WindowKey::GetHighlightControl() const
{
	HighlightControl ccHighlight(*this);
	return ccHighlight;
}

