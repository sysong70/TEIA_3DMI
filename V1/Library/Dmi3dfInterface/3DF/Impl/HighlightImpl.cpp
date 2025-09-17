#include "StdAfx.h"

#include "HighlightImpl.h"

#include "Selection.Impl.h"

#include "../Window.h"
#include "WindowImpl.h"

#include "../../Sprocket/Impl/3DF.View.Impl.h"

#include "../AttributeLock.h"

#include <vhash.h>
#include <vlist.h>

#include <atlcoll.h>

#include <HBaseOperator.h>
#include <HMarkupManager.h>
#include <HConstantFrameRate.h>

#define		SEGMENT_TYPE		1
#define		ENTITY_TYPE			2
#define		SUBENTITY_TYPE		3
#define		REGION_TYPE			4

using namespace H3DF;


//== SelectionSet Class =============================================================================
namespace H3DF
{
	class HighlightSelectionSet : public HSelectionSet
	{
	public:
		HighlightSelectionSet(HBaseView * pcInView, bool bInReferenceSelection = false);

		void UpdateHighlightStyle1();
	};
}

H3DF::HighlightSelectionSet::HighlightSelectionSet(HBaseView * pcInView, bool bInReferenceSelection)
	: HSelectionSet(pcInView, bInReferenceSelection)
{
}

void H3DF::HighlightSelectionSet::UpdateHighlightStyle1()
{
	UpdateHighlightStyle();

	SegmentKey cHighlightStyleSegment = GetHighlightStyle();
	cHighlightStyleSegment.GetAttributeLockControl().SetLock(AttributeLock::Type::LineAttributeWeight).SetLock(AttributeLock::Type::EdgeAttributeWeight);
}

//== HighlightControlImpl Class ====================================================================

// #Highlight 관련 설정 (Style, Color등을 설정)
H3DF::HighlightControlImpl::HighlightControlImpl(WindowKey const & cInWindow)
{
	m_pcWindow = (WindowKey *)&cInWindow;

	// HSelectionSet은 각각 선언될때, Style을 생성하게 된다.
	//================================================================================================

	m_pcSelectionSet = new H3DF::HighlightSelectionSet((HBaseView *) cInWindow.GetBaseView());

	m_pcSelectionSet->SetHighlightMode(HighlightQuickmoves);

	m_pcSelectionSet->SetReferenceSelectionType(RefSelOff);

	m_pcSelectionSet->SetSelectionLevel(HSelectEntity);

	m_pcSelectionSet->SetReferenceSelectionType(RefSelSpriting);

	m_pcSelectionSet->SetSelectionEdgeWeight(1.0);

	// 선택될때 Face의 Edge를 표시여부 처리
	m_pcSelectionSet->HighlightRegionEdgesAutoVisibility(false);

	m_pcSelectionSet->SetAllowRegionSelection(false);

	m_pcSelectionSet->SetGrayScale(false);
	m_pcSelectionSet->SetUseDefinedHighlight(false);
	m_pcSelectionSet->SetAllowDisplacement(false);

	m_pcSelectionSet->UpdateHighlightStyle();

#if 0
	HC_Open_Segment_By_Key(m_pcSelectionSet->GetHighlightStyle()); {
		char chRenderOptions[MVO_BUFFER_SIZE];
		HC_Show_Rendering_Options(chRenderOptions);
		int i = 0;
	} HC_Close_Segment();
#endif
}

BaseView * H3DF::HighlightControlImpl::GetBaseView()
{
	return (BaseView *)m_pcWindow->GetBaseView();
}

BaseView * H3DF::HighlightControlImpl::GetBaseView() const
{
	DEBUG_VALID(m_pcWindow);
	return (BaseView *)m_pcWindow->GetBaseView();
}

HSelectionSet * H3DF::HighlightControlImpl::SelectionSet()
{
	//return ((HBaseView *)m_pcWindow->GetBaseView())->GetHighlightSelection();

	DEBUG_VALID(m_pcSelectionSet);
	return m_pcSelectionSet;
}

