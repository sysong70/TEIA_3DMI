#include "StdAfx.h"

#include "Window.h"
#include "Private/3DF.WindowPrivate.h"

#include "Private/3DF.SelectionPrivate.h"
#include "Private/3DF.HighlightPrivate.h"

#include "BaseView.h"

#include <hc.h>
#include <HBaseOperator.h>
#include <HSelectionSet.h>

USING_3DF_NAMESPACE

// public HBaseView, public HAnimationListener

H3DF::WindowKey::WindowKey(H3DF::BaseView * pcBaseView)
{
	WindowKeyPrivate * pcImpl = new WindowKeyPrivate();
	m_pcImpl = pcImpl;

	pcImpl->m_pcBaseView = pcBaseView;

	// SelectionSet 초기화, 3DF에서는 Hightliht, Selection을 구분하지 않고 사용한다.

	pcImpl->m_pcSelectionSet = new HSelectionSet((HBaseView *)pcBaseView);

	pcImpl->m_pcSelection = new SelectionControl(*this);
	SelectionControlPrivate * pcSelectionImpl = static_cast<SelectionControlPrivate *>(pcImpl->m_pcSelection->GetImpl());
	pcSelectionImpl->m_pcSelectionSet = pcImpl->m_pcSelectionSet;

	pcImpl->m_pcHighlight = new HighlightControl(*this);
	HighlightControlPrivate * pcHighlightImpl = static_cast<HighlightControlPrivate *>(pcImpl->m_pcHighlight->GetImpl());
	pcHighlightImpl->m_pcSelectionSet = pcImpl->m_pcSelectionSet;

	pcImpl->m_pcSelectionOptions = new SelectionOptionsControl(*this);
	SelectionOptionsControlPrivate * pcSelectionOptionsImpl = static_cast<SelectionOptionsControlPrivate *>(pcImpl->m_pcSelectionOptions->GetImpl());
	pcSelectionOptionsImpl->m_pcSelectionSet = pcImpl->m_pcSelectionSet;

	// Initialize();
}

H3DF::WindowKey::WindowKey(WindowKey const & cInThat)
{
	WindowKeyPrivate * pcImpl = new WindowKeyPrivate();
	pcImpl->m_pcBaseView = ((WindowKeyPrivate *)cInThat.m_pcImpl)->m_pcBaseView;
	m_pcImpl = pcImpl;
}

H3DF::WindowKey::~WindowKey()
{
	WindowKeyPrivate * pcImpl = static_cast<WindowKeyPrivate *>(m_pcImpl);

	if (nullptr != pcImpl->m_pcSelectionSet)
	{
		delete pcImpl->m_pcSelectionSet;
		pcImpl->m_pcSelectionSet = nullptr;
	}

	if (nullptr != pcImpl->m_pcSelection)
	{
		delete pcImpl->m_pcSelection;
		pcImpl->m_pcSelection = nullptr;
	}

	if (nullptr != pcImpl->m_pcHighlight)
	{
		delete pcImpl->m_pcHighlight;
		pcImpl->m_pcHighlight = nullptr;
	}
}

const H3DF::BaseView * H3DF::WindowKey::GetBaseView() const
{
	WindowKeyPrivate * pcImpl = static_cast<WindowKeyPrivate *>(m_pcImpl);
	return pcImpl->m_pcBaseView;
}

H3DF::BaseView * H3DF::WindowKey::GetBaseView()
{
	WindowKeyPrivate * pcImpl = static_cast<WindowKeyPrivate *>(m_pcImpl);
	return pcImpl->GetBaseView();
}

HC_KEY H3DF::WindowKey::GetSceneKey()
{
	return GetBaseView()->GetSceneKey();
}

const HC_KEY H3DF::WindowKey::GetSceneKey() const
{
	WindowKeyPrivate * pcImpl = static_cast<WindowKeyPrivate *>(m_pcImpl);
	return pcImpl->GetBaseView()->GetSceneKey();
}

void H3DF::WindowKey::Update()
{
	GetBaseView()->Update();
}

int H3DF::WindowKey::ViewId()
{
	WindowKeyPrivate * pcImpl = static_cast<WindowKeyPrivate *>(m_pcImpl);
	return pcImpl->m_nViewId;
}

const int H3DF::WindowKey::ViewId() const
{ 
	WindowKeyPrivate * pcImpl = static_cast<WindowKeyPrivate *>(m_pcImpl);
	return pcImpl->m_nViewId;
}

void H3DF::WindowKey::SetViewId(int nViewId) 
{ 
	WindowKeyPrivate * pcImpl = static_cast<WindowKeyPrivate *>(m_pcImpl);
	pcImpl->m_nViewId = nViewId;
}

void H3DF::WindowKey::Initialize()
{
	WindowKeyPrivate * pcImpl = static_cast<WindowKeyPrivate *>(m_pcImpl);

//	HBaseView * pcBaseView = pcImpl->m_pcBaseView;
}

int H3DF::WindowKey::OnMouseMove(HEventInfo & cEvent)
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

SelectionOptionsControl & H3DF::WindowKey::GetSelectionOptionsControl()
{
	WindowKeyPrivate * pcImpl = static_cast<WindowKeyPrivate *>(m_pcImpl);
	return * pcImpl->m_pcSelectionOptions;
}

SelectionOptionsControl const & H3DF::WindowKey::GetSelectionOptionsControl() const
{
	WindowKeyPrivate * pcImpl = static_cast<WindowKeyPrivate *>(m_pcImpl);
	return *pcImpl->m_pcSelectionOptions;
}

SelectionControl & H3DF::WindowKey::GetSelectionControl()
{
	WindowKeyPrivate * pcImpl = static_cast<WindowKeyPrivate *>(m_pcImpl);
	return *pcImpl->m_pcSelection;
}

SelectionControl const & H3DF::WindowKey::GetSelectionControl() const
{
	WindowKeyPrivate * pcImpl = static_cast<WindowKeyPrivate *>(m_pcImpl);
	return *pcImpl->m_pcSelection;
}

HighlightControl & H3DF::WindowKey::GetHighlightControl()
{
	WindowKeyPrivate * pcImpl = static_cast<WindowKeyPrivate *>(m_pcImpl);
	return *pcImpl->m_pcHighlight;
}

HighlightControl const & H3DF::WindowKey::GetHighlightControl() const
{
	WindowKeyPrivate * pcImpl = static_cast<WindowKeyPrivate *>(m_pcImpl);
	return *pcImpl->m_pcHighlight;
}

