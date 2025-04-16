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

H3DF::WindowKey::WindowKey() {
	WindowKeyImpl * pcImpl = new WindowKeyImpl();
	DEBUG_VALID(pcImpl);

	m_pcImpl = pcImpl;

	// 단순 선언은 Type을 None으로 설정한다.
	m_pcImpl->SetType(H3DF::Type::None);
}

H3DF::WindowKey::WindowKey(WindowKey const & cInThat)
{
	WindowKeyImpl * pcImpl = new WindowKeyImpl();
	m_pcImpl = pcImpl;

	Set(cInThat);
}

H3DF::WindowKey::~WindowKey()
{
/*
	WindowKeyImpl * pcImpl = dynamic_cast<WindowKeyImpl *>(m_pcImpl);

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

void H3DF::WindowKey::Set(WindowKey const & cInThat)
{
	WindowKeyImpl * pcImpl = (WindowKeyImpl *) m_pcImpl;
	WindowKeyImpl * pcInThatImpl = (WindowKeyImpl *) cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

WindowKey const & H3DF::WindowKey::operator = (WindowKey const & cInThat)
{
	Set(cInThat);
	return *this;
}

const H3DF::BaseView * H3DF::WindowKey::GetBaseView() const
{
	WindowKeyImpl * pcImpl = dynamic_cast<WindowKeyImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);
	return pcImpl->GetBaseView();
}

H3DF::BaseView * H3DF::WindowKey::GetBaseView()
{
	WindowKeyImpl * pcImpl = dynamic_cast<WindowKeyImpl *>(m_pcImpl);
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
	WindowKeyImpl * pcImpl = dynamic_cast<WindowKeyImpl *>(m_pcImpl);
	return pcImpl->m_nViewId;
}

const int H3DF::WindowKey::ViewId() const
{ 
	WindowKeyImpl * pcImpl = dynamic_cast<WindowKeyImpl *>(m_pcImpl);
	return pcImpl->m_nViewId;
}

void H3DF::WindowKey::SetViewId(int nViewId) 
{ 
	WindowKeyImpl * pcImpl = dynamic_cast<WindowKeyImpl *>(m_pcImpl);
	pcImpl->m_nViewId = nViewId;
}

WindowKey & H3DF::WindowKey::SetSelectionOptions(SelectionOptionsKit const & cInKit)
{
	WindowKeyImpl * pcImpl = dynamic_cast<WindowKeyImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	//SegmentKey cViewSegment(pcImpl->GetBaseView()->GetViewKey());

	return *this;
}

bool H3DF::WindowKey::ShowSelectionOptions(SelectionOptionsKit & cOutKit) const
{
	WindowKeyImpl * pcImpl = dynamic_cast<WindowKeyImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return true;
}

SelectionOptionsControl & H3DF::WindowKey::GetSelectionOptionsControl()
{
	WindowKeyImpl * pcImpl = dynamic_cast<WindowKeyImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);
	return * pcImpl->m_pcSelectionOptions;
}

SelectionOptionsControl const & H3DF::WindowKey::GetSelectionOptionsControl() const
{
	WindowKeyImpl * pcImpl = dynamic_cast<WindowKeyImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);
	return *pcImpl->m_pcSelectionOptions;
}

SelectionControl & H3DF::WindowKey::GetSelectionControl()
{
	WindowKeyImpl * pcImpl = dynamic_cast<WindowKeyImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);
	return *pcImpl->m_pcSelection;
}

SelectionControl const & H3DF::WindowKey::GetSelectionControl() const
{
	WindowKeyImpl * pcImpl = dynamic_cast<WindowKeyImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);
	return *pcImpl->m_pcSelection;
}

HighlightControl & H3DF::WindowKey::GetHighlightControl()
{
	WindowKeyImpl * pcImpl = dynamic_cast<WindowKeyImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);
	return *pcImpl->m_pcHighlight;
}

HighlightControl const & H3DF::WindowKey::GetHighlightControl() const
{
	WindowKeyImpl * pcImpl = dynamic_cast<WindowKeyImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);
	return *pcImpl->m_pcHighlight;
}


