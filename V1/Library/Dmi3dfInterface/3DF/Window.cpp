#include "StdAfx.h"

#include "Window.h"
#include "Impl/WindowImpl.h"

#include "Selection.h"
#include "Impl/SelectionImpl.h"

#include "Highlight.h"
#include "Impl/HighlightImpl.h"

#include "../Impl/ViewImpl.h"

#include <hc.h>
#include <HBaseOperator.h>
#include <HSelectionSet.h>

USING_3DF_NAMESPACE

// public HBaseView, public HAnimationListener

H3DF::WindowKey::WindowKey(H3DF::BaseView * pcBaseView)
{
	WindowKeyImpl * pcImpl = new WindowKeyImpl();
	m_pcImpl = pcImpl;

	pcImpl->m_pcBaseView = pcBaseView;

	//SetKeyValue(pcBaseView->GetViewKey());

	// SelectionSet 초기화, 3DF에서는 Hightliht, Selection을 구분하지 않고 사용한다.

	pcImpl->m_pcSelectionSet = new HSelectionSet((HBaseView *)pcBaseView);

	pcImpl->m_pcSelection = new SelectionControl(*this);
	SelectionControlImpl * pcSelectionImpl = static_cast<SelectionControlImpl *>(pcImpl->m_pcSelection->GetImpl());
	pcSelectionImpl->m_pcSelectionSet = GetBaseView()->GetSelection();

	pcImpl->m_pcHighlight = new HighlightControl(*this);
	HighlightControlImpl * pcHighlightImpl = static_cast<HighlightControlImpl *>(pcImpl->m_pcHighlight->GetImpl());

	pcImpl->m_pcSelectionOptions = new SelectionOptionsControl(*this);
	SelectionOptionsControlImpl * pcSelectionOptionsImpl = static_cast<SelectionOptionsControlImpl *>(pcImpl->m_pcSelectionOptions->GetImpl());
	pcSelectionOptionsImpl->m_pcSelectionSet = pcImpl->m_pcSelectionSet;

	Initialize();
}

H3DF::WindowKey::WindowKey(WindowKey const & cInThat)
{
	WindowKeyImpl * pcImpl = new WindowKeyImpl();
	pcImpl->m_pcBaseView = ((WindowKeyImpl *)cInThat.m_pcImpl)->m_pcBaseView;
	m_pcImpl = pcImpl;
}

H3DF::WindowKey::~WindowKey()
{
	WindowKeyImpl * pcImpl = static_cast<WindowKeyImpl *>(m_pcImpl);

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
	WindowKeyImpl * pcImpl = static_cast<WindowKeyImpl *>(m_pcImpl);
	return pcImpl->m_pcBaseView;
}

H3DF::BaseView * H3DF::WindowKey::GetBaseView()
{
	WindowKeyImpl * pcImpl = static_cast<WindowKeyImpl *>(m_pcImpl);
	return pcImpl->GetBaseView();
}

void H3DF::WindowKey::Update()
{
	GetBaseView()->Update();
}

int H3DF::WindowKey::ViewId()
{
	WindowKeyImpl * pcImpl = static_cast<WindowKeyImpl *>(m_pcImpl);
	return pcImpl->m_nViewId;
}

const int H3DF::WindowKey::ViewId() const
{ 
	WindowKeyImpl * pcImpl = static_cast<WindowKeyImpl *>(m_pcImpl);
	return pcImpl->m_nViewId;
}

void H3DF::WindowKey::SetViewId(int nViewId) 
{ 
	WindowKeyImpl * pcImpl = static_cast<WindowKeyImpl *>(m_pcImpl);
	pcImpl->m_nViewId = nViewId;
}

void H3DF::WindowKey::Initialize()
{
	WindowKeyImpl * pcImpl = static_cast<WindowKeyImpl *>(m_pcImpl);

//	HBaseView * pcBaseView = pcImpl->m_pcBaseView;
}

WindowKey & H3DF::WindowKey::SetSelectionOptions(SelectionOptionsKit const & cInKit)
{
	WindowKeyImpl * pcImpl = static_cast<WindowKeyImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	//SegmentKey cViewSegment(pcImpl->GetBaseView()->GetViewKey());

	return *this;
}

bool H3DF::WindowKey::ShowSelectionOptions(SelectionOptionsKit & cOutKit) const
{
	WindowKeyImpl * pcImpl = static_cast<WindowKeyImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return true;
}

SelectionOptionsControl & H3DF::WindowKey::GetSelectionOptionsControl()
{
	WindowKeyImpl * pcImpl = static_cast<WindowKeyImpl *>(m_pcImpl);
	return * pcImpl->m_pcSelectionOptions;
}

SelectionOptionsControl const & H3DF::WindowKey::GetSelectionOptionsControl() const
{
	WindowKeyImpl * pcImpl = static_cast<WindowKeyImpl *>(m_pcImpl);
	return *pcImpl->m_pcSelectionOptions;
}

SelectionControl & H3DF::WindowKey::GetSelectionControl()
{
	WindowKeyImpl * pcImpl = static_cast<WindowKeyImpl *>(m_pcImpl);
	return *pcImpl->m_pcSelection;
}

SelectionControl const & H3DF::WindowKey::GetSelectionControl() const
{
	WindowKeyImpl * pcImpl = static_cast<WindowKeyImpl *>(m_pcImpl);
	return *pcImpl->m_pcSelection;
}

HighlightControl & H3DF::WindowKey::GetHighlightControl()
{
	WindowKeyImpl * pcImpl = static_cast<WindowKeyImpl *>(m_pcImpl);
	return *pcImpl->m_pcHighlight;
}

HighlightControl const & H3DF::WindowKey::GetHighlightControl() const
{
	WindowKeyImpl * pcImpl = static_cast<WindowKeyImpl *>(m_pcImpl);
	return *pcImpl->m_pcHighlight;
}


