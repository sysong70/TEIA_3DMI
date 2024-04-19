#include "StdAfx.h"

#include "3DF.ComponentMarkupView.h"
#include "Impl/ComponentMarkupViewImpl.h"

#include "Common_Define.h"

#include "../3DF/Segment.h"
#include "../3DF/3DF.Utility.h"
#include "../3DF/KeyPath.h"
#include "../3DF/Selection.h"
#include "../3DF/Impl/SelectionImpl.h"

#include <ranges>

using namespace H3DF;

//== ComponentMarkupView Class ===============================================================================
H3DF::ComponentMarkupView::ComponentMarkupView()
{
	m_pcImpl = new ComponentMarkupViewImpl();
	DEBUG_VALID(m_pcImpl);
}

H3DF::ComponentMarkupView::ComponentMarkupView(ComponentMarkupView const & cInThat)
{
	ComponentImpl * pcImpl = new ComponentMarkupViewImpl();
	DEBUG_VALID(pcImpl);

	m_pcImpl = pcImpl;

	Set(cInThat);
}

void H3DF::ComponentMarkupView::Set(ComponentMarkupView const & cInThat)
{
	

	ComponentImpl * pcImpl = (ComponentImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	ComponentImpl * pcInThatImpl = (ComponentImpl *)cInThat.m_pcImpl;
	DEBUG_VALID(pcInThatImpl);

	pcImpl->Copy(pcInThatImpl);

	// ComponentMarkupView의 소유자를 설정한다.
	if (nullptr != pcImpl->m_pvSubComponents) {
		for (auto * pcComponent : *pcImpl->m_pvSubComponents) {
			ComponentImpl * pcSubImpl = (ComponentImpl *)pcComponent->GetImpl();
			DEBUG_VALID(pcImpl);
			pcSubImpl->m_pcOwner = this;
		}
	}
}

ComponentMarkupView & H3DF::ComponentMarkupView::operator = (ComponentMarkupView const & cInThat)
{
	Set(cInThat);
	return *this;
}

bool H3DF::ComponentMarkupView::Equals(ComponentMarkupView const & cInThat) const
{
	ComponentImpl * pcImpl = (ComponentImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	
	ComponentImpl * pcInThatImpl = (ComponentImpl *)cInThat.m_pcImpl;
	DEBUG_VALID(pcInThatImpl);

	if (pcImpl->m_nSegmentKey != pcInThatImpl->m_nSegmentKey) {
		return false;
	}

	if (pcImpl->m_nIncludeKey != pcInThatImpl->m_nIncludeKey) {
		return false;
	}

	return true;
}