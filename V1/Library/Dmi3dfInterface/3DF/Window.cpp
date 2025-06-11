#include "StdAfx.h"

#include "Window.h"
#include "Impl/WindowImpl.h"

#include "Selection.h"
#include "Impl/Selection.Impl.h"

#include "Highlight.h"

#include "../Sprocket/Impl/3DF.View.Impl.h"

#include <hc.h>
#include <HBaseOperator.h>
#include <HSelectionSet.h>

USING_3DF_NAMESPACE

// public HBaseView, public HAnimationListener

H3DF::WindowKey::WindowKey() 
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = std::make_unique<WindowKeyImpl>();
	DEBUG_VALID(m_pcImpl);
}

H3DF::WindowKey::WindowKey(WindowKey const & cInThat)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = (nullptr == cInThat.GetImpl()) ? cInThat.GetImpl()->Clone() : nullptr;
}

H3DF::WindowKey::~WindowKey()
{
/*
	auto pcImpl = static_cast<WindowKeyImpl *>(m_pcImpl.get());

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

	if (nullptr != pcImpl->m_pcSelectionOptions)
	{
		delete pcImpl->m_pcSelectionOptions;
		pcImpl->m_pcSelectionOptions = nullptr;
	}*/
}

WindowKey const & H3DF::WindowKey::operator = (WindowKey const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

const H3DF::BaseView * H3DF::WindowKey::GetBaseView() const
{
	auto pcImpl = static_cast<WindowKeyImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	return pcImpl->GetBaseView();
}

H3DF::BaseView * H3DF::WindowKey::GetBaseView()
{
	auto pcImpl = static_cast<WindowKeyImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	return pcImpl->GetBaseView();
}

void H3DF::WindowKey::Update()
{
	GetBaseView()->Update();
}

void H3DF::WindowKey::ForceUpdate()
{
	GetBaseView()->ForceUpdate();
}

int H3DF::WindowKey::ViewId()
{
	auto pcImpl = static_cast<WindowKeyImpl *>(m_pcImpl.get());
	return pcImpl->m_nViewId;
}

const int H3DF::WindowKey::ViewId() const
{ 
	auto pcImpl = static_cast<WindowKeyImpl *>(m_pcImpl.get());
	return pcImpl->m_nViewId;
}

void H3DF::WindowKey::SetViewId(int nViewId) 
{ 
	auto pcImpl = static_cast<WindowKeyImpl *>(m_pcImpl.get());
	pcImpl->m_nViewId = nViewId;
}

WindowKey & H3DF::WindowKey::SetSelectionOptions(SelectionOptionsKit const & cInKit)
{
	auto pcImpl = static_cast<WindowKeyImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	//SegmentKey cViewSegment(pcImpl->GetBaseView()->GetViewKey());

	return *this;
}

bool H3DF::WindowKey::ShowSelectionOptions(SelectionOptionsKit & cOutKit) const
{
	auto pcImpl = static_cast<WindowKeyImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	return true;
}

SelectionOptionsControl & H3DF::WindowKey::GetSelectionOptionsControl()
{
	auto pcImpl = static_cast<WindowKeyImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	return * pcImpl->m_pcSelectionOptions;
}

SelectionOptionsControl const & H3DF::WindowKey::GetSelectionOptionsControl() const
{
	auto pcImpl = static_cast<WindowKeyImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	return *pcImpl->m_pcSelectionOptions;
}

SelectionControl & H3DF::WindowKey::GetSelectionControl()
{
	auto pcImpl = static_cast<WindowKeyImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	return *pcImpl->m_pcSelection;
}

SelectionControl const & H3DF::WindowKey::GetSelectionControl() const
{
	auto pcImpl = static_cast<WindowKeyImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	return *pcImpl->m_pcSelection;
}

HighlightControl & H3DF::WindowKey::GetHighlightControl()
{
	auto pcImpl = static_cast<WindowKeyImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	return *pcImpl->m_pcHighlight;
}

HighlightControl const & H3DF::WindowKey::GetHighlightControl() const
{
	auto pcImpl = static_cast<WindowKeyImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	return *pcImpl->m_pcHighlight;
}

NavigationCube & H3DF::WindowKey::GetNavigationCube()
{
	auto pcImpl = static_cast<WindowKeyImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	return pcImpl->GetNavigationCube();
}

