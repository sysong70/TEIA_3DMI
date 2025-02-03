#include "StdAfx.h"

#include "Highlight.h"

#include "Impl/SelectionImpl.h"
#include "Impl/ControlImpl.h"

#include "Window.h"
#include "Impl/WindowImpl.h"
#include "../Sprocket/Impl/3DF.View.Impl.h"

#include "Line.h"
#include "LineAttribute.h"

#include "AttributeLock.h"

#include "Visibility.h"

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

//== HighlightOptionsKit Class =====================================================================
namespace H3DF
{
	class HighlightOptionsKitImpl : public Impl
	{
	public:
		HighlightOptionsKitImpl() { m_eType = H3DF::Type::HighlightOptionsKit; }

		void Copy(HighlightOptionsKitImpl * pcInThat) {
			m_strInStyleName = pcInThat->m_strInStyleName;
			m_strInSecondaryStyleName = pcInThat->m_strInSecondaryStyleName;
			m_nNotification = pcInThat->m_nNotification;
		}

		CStringA m_strInStyleName;
		CStringA m_strInSecondaryStyleName;
		int m_nNotification = -1;
	};
}

H3DF::HighlightOptionsKit::HighlightOptionsKit()
{
	m_pcImpl = new HighlightOptionsKitImpl();
}

H3DF::HighlightOptionsKit::HighlightOptionsKit(CStringA strInStyleName)
{
	HighlightOptionsKitImpl * pcImpl = new HighlightOptionsKitImpl();
	pcImpl->m_strInStyleName = strInStyleName;
	m_pcImpl = pcImpl;
}

H3DF::HighlightOptionsKit::HighlightOptionsKit(CStringA strInStyleName, CStringA strInSecondaryStyleName)
{
	HighlightOptionsKitImpl * pcImpl = new HighlightOptionsKitImpl();
	pcImpl->m_strInStyleName = strInStyleName;
	pcImpl->m_strInSecondaryStyleName = strInSecondaryStyleName;
	m_pcImpl = pcImpl;
}

H3DF::HighlightOptionsKit::HighlightOptionsKit(HighlightOptionsKit const & cInThat)
{
	m_pcImpl = new HighlightOptionsKitImpl();
	Set(cInThat);
}

void H3DF::HighlightOptionsKit::Set(HighlightOptionsKit const & cInThat)
{
	HighlightOptionsKitImpl * pcImpl = (HighlightOptionsKitImpl *)m_pcImpl;
	HighlightOptionsKitImpl * pcInThatImpl = (HighlightOptionsKitImpl *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

HighlightOptionsKit & H3DF::HighlightOptionsKit::operator=(HighlightOptionsKit const & cInThat)
{
	Set(cInThat);
	return *this;
}

HighlightOptionsKit & H3DF::HighlightOptionsKit::SetNotification(bool bInState)
{
	HighlightOptionsKitImpl * pcImpl = (HighlightOptionsKitImpl *)m_pcImpl;

	if (true == bInState) {
		pcImpl->m_nNotification = true;
	}
	else {
		pcImpl->m_nNotification = false;
	}

	return *this;
}

HighlightOptionsKit & H3DF::HighlightOptionsKit::UnsetNotification()
{
	HighlightOptionsKitImpl * pcImpl = (HighlightOptionsKitImpl *)m_pcImpl;
	pcImpl->m_nNotification = -1;
	return *this;
}

bool H3DF::HighlightOptionsKit::ShowNotification(bool & bOutState) const
{
	HighlightOptionsKitImpl * pcImpl = (HighlightOptionsKitImpl *)m_pcImpl;
	if (0 > pcImpl->m_nNotification) {
		return false;
	}

	bOutState = (0 != pcImpl->m_nNotification);
	return true;
}

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
namespace H3DF
{
	class HighlightControlImpl : public ControlImpl
	{
	public:
		HighlightControlImpl(WindowKey const & cInWindow);

		void Copy(HighlightControlImpl * pcInThat) {
			m_pcWindow = pcInThat->m_pcWindow;
		}

		// 	int SelectButtonDown(Point const & cInLocation, UINT const nFlags, SelectionResults & cOutResults);
		// 	void HandleSelection(UINT const nFlags, SelectionResults & cOutResults);

		const WindowKey & Window() { return *m_pcWindow; }

		BaseView * GetBaseView();
		BaseView * GetBaseView() const;

		HSelectionSet * SelectionSet();

		H3DF::SelectionResults m_cOldHighlightSelection;

	private:
		const WindowKey * m_pcWindow = nullptr;
		HSelectionSet * m_pcSelectionSet = nullptr;
	};
}

H3DF::HighlightControlImpl::HighlightControlImpl(WindowKey const & cInWindow)
{
	m_eType = H3DF::Type::HighlightControl;
	m_pcWindow = (WindowKey *)&m_pcWindow;

	m_pcSelectionSet = ((HBaseView *)cInWindow.GetBaseView())->GetHighlightSelection();
	m_pcSelectionSet->SetHighlightMode(HighlightQuickmoves);
	m_pcSelectionSet->SetReferenceSelectionType(RefSelSpriting);

	m_pcSelectionSet->UpdateHighlightStyle();

// 
 	return;

// 	if (false == bDynFlag) {
// 		m_pcSelectionSet = ((HBaseView *)cInWindow.GetBaseView())->GetSelection();
// 	}
// 	else
// 	{
// 		m_pcSelectionSet = ((HBaseView *)cInWindow.GetBaseView())->GetHighlightSelection();
// 	}

	// HSelectionSet은 각각 선언될때, Style을 생성하게 된다.
	//================================================================================================

	m_pcSelectionSet = new H3DF::HighlightSelectionSet((HBaseView *)cInWindow.GetBaseView());

// 	m_pcSelectionSet->SetHighlightMode(HighlightQuickmoves);
// 	m_pcSelectionSet->SetReferenceSelectionType(RefSelOff);
// 
// 	m_pcSelectionSet->SetSelectionLevel(HSelectEntity);
	//m_pcSelectionSet->SetSelectionEdgeWeight(1.0);

	//================================================================================================

	/*m_pcSelectionSet->SetAllowRegionSelection(true);

	m_pcSelectionSet->SetGrayScale(false);
	// Transparecy Segment를 선택했을 때, 투명하게 보이도록 설정하는 부분
	m_pcSelectionSet->SetUseDefinedHighlight(false);
	m_pcSelectionSet->SetAllowDisplacement(false);*/

// 	HPixelRGBA cHighlightSelectColor;
// 	cHighlightSelectColor.Set(255, 0, 0);
// 
// 	m_pcSelectionSet->SetSelectionFaceColor(cHighlightSelectColor);
// 	m_pcSelectionSet->SetSelectionEdgeColor(cHighlightSelectColor);
// 	m_pcSelectionSet->SetSelectionMarkerColor(cHighlightSelectColor);

	// 선택될때 Face의 Edge를 표시여부 처리
	//m_pcSelectionSet->HighlightRegionEdgesAutoVisibility(false);

	m_pcSelectionSet->UpdateHighlightStyle();
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

//== HighlightControl Class ========================================================================

H3DF::HighlightControl::HighlightControl(WindowKey const & cInWindow)
{
	HighlightControlImpl * pcImpl = new HighlightControlImpl(cInWindow);
	m_pcImpl = pcImpl;
}

/*
H3DF::HighlightControl::HighlightControl(HighlightControl const & cInThat)
{
	m_pcImpl = new HighlightControlPrivate();
	Set(cInThat);
}
*/

H3DF::HighlightControl::HighlightControl() {}

void H3DF::HighlightControl::Set(HighlightControl const & cInThat)
{
	HighlightControlImpl * pcImpl = (HighlightControlImpl *)m_pcImpl;
	HighlightControlImpl * pcInThatImpl = (HighlightControlImpl *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

HighlightControl & H3DF::HighlightControl::operator=(HighlightControl const & cInThat)
{
	Set(cInThat);
	return *this;
}

void H3DF::HighlightControl::SetMode(HighlightMode::Type eInMode)
{
	HighlightControlImpl * pcHighlightImpl = (HighlightControlImpl *)m_pcImpl;

	HSelectionHighlightMode eMode = HighlightDefault;

	switch (eInMode) {
		case HighlightMode::Type::DefaultConditional:
			eMode = HighlightDefault;
		break;

		case HighlightMode::Type::Quickmoves:
			eMode = HighlightQuickmoves;
		break;

		case HighlightMode::Type::InverseTransparency:
			eMode = InverseTransparency;
			break;

		case HighlightMode::Type::ColoredInverseTransparency:
			eMode = ColoredInverseTransparency;
			break;
	}

	pcHighlightImpl->SelectionSet()->SetHighlightMode(eMode);
}

void H3DF::HighlightControl::SetMode(HighlightMode::Type eInMode) const
{
	HighlightControlImpl * pcHighlightImpl = (HighlightControlImpl *)m_pcImpl;
	HSelectionHighlightMode eMode = HighlightDefault;

	switch (eInMode) {
	case HighlightMode::Type::DefaultConditional:
		eMode = HighlightDefault;
		break;

	case HighlightMode::Type::Quickmoves:
		eMode = HighlightQuickmoves;
		break;

	case HighlightMode::Type::InverseTransparency:
		eMode = InverseTransparency;
		break;

	case HighlightMode::Type::ColoredInverseTransparency:
		eMode = ColoredInverseTransparency;
		break;
	}

	pcHighlightImpl->SelectionSet()->SetHighlightMode(eMode);
}

//== Highlight 관련 함수 =============================================================================
HighlightControl & H3DF::HighlightControl::Highlight(SelectionResults const & cInItems, HighlightOptionsKit const & cInOptions, bool bInRemoveExisting)
{
	HighlightControlImpl * pcHighlightImpl = (HighlightControlImpl *)m_pcImpl;

	HSelectionSet * pcSelSet = pcHighlightImpl->SelectionSet();

	char chType[MVO_BUFFER_SIZE];

	bool bNeedDeselect = true;
	bool bNeedUpdate = true;

	if (true == bInRemoveExisting) {
		pcSelSet->DeSelectAll();
	}

	SelectionResultsImpl * pcSelectionResultsImpl = (SelectionResultsImpl *)cInItems.GetImpl();

	for (auto & cItem : pcSelectionResultsImpl->GetItems()) {
		SelectionItemImpl * pcItemImpl = (SelectionItemImpl *)cItem.GetImpl();

		HC_KEY nKey = pcItemImpl->m_cKey.KeyValue();

		// Region 선택 관련 처리 부분
		if (H3DF::Type::ShellKey == cItem.Type() && (pcItemImpl->m_nLowest != pcItemImpl->m_nHighest || pcItemImpl->m_nLowest > 0)) {
			bNeedDeselect = false;

			if (!pcSelSet->IsRegionSelected(nKey, pcItemImpl->m_nIncludeCount, pcItemImpl->m_pnIncludeKeys, pcItemImpl->m_nRegion))
			{
				if (true == bInRemoveExisting) {
					pcSelSet->DeSelectAll();
				}

				pcSelSet->SelectRegion(nKey, pcItemImpl->m_nIncludeCount, pcItemImpl->m_pnIncludeKeys, pcItemImpl->m_nRegion, false);
			}
			else {
				bNeedUpdate = false;
			}
		}
		else {
			bNeedDeselect = false;

			if (!pcSelSet->IsSelected(nKey, pcItemImpl->m_nIncludeCount, pcItemImpl->m_pnIncludeKeys)) {
				HSelectLevel eSelectLevel = pcSelSet->GetSelectionLevel();

				if (pcSelSet->GetSelectionLevel() != HSelectSegment) // never should fail for dynamic highlighting, but let's be nice and check
				{
					// the key is to a geometric entity.  If we are in segment selection mode,
					// then we need to get the key to its parent segment.
					HC_Show_Key_Type(nKey, chType);

					if (!streq("segment", chType))
					{
						char segname[MVO_BUFFER_SIZE];
						HC_KEY segkey;

						segkey = HC_KShow_Owner_Original_Key(nKey);
						HC_Show_Owner_By_Key(nKey, segname);

						// climb up one more level if this is the temporary highlight key
						if (pcSelSet->IsHighlightSegment(segkey))
						{
							segkey = HC_KShow_Owner_Original_Key(segkey);
							HC_Show_Owner_By_Key(segkey, segname);
						}
					}
				}

				pcSelSet->Select(nKey, pcItemImpl->m_nIncludeCount, pcItemImpl->m_pnIncludeKeys, false);
			}
			else {
				bNeedUpdate = false;
			}
		}
	}

	if (bNeedDeselect) {
		pcSelSet->DeSelectAll();
	}

	if (bNeedUpdate) {
		bool bShowNotification = false;
		cInOptions.ShowNotification(bShowNotification);

		if (true == bShowNotification) {
			H3DF::BaseView * pcView = pcHighlightImpl->GetBaseView();
			pcView->ForceUpdate();
		}
	}

	return *this;
}

HighlightControl & H3DF::HighlightControl::Highlight(SelectionItem const & cInItem, HighlightOptionsKit const & cInOptions, bool bInRemoveExisting)
{
	HighlightControlImpl * pcHighlightImpl = (HighlightControlImpl *)m_pcImpl;
	DEBUG_VALID(pcHighlightImpl);

	HSelectionSet * pcSelSet = pcHighlightImpl->SelectionSet(); // HSelectionSet에서 Select 및 Highlight를 다 처리함.
	DEBUG_VALID(pcSelSet);

	if (true == bInRemoveExisting) {
		pcSelSet->DeSelectAll();
	}

	SelectionItemImpl * pcItemImpl = (SelectionItemImpl *)cInItem.GetImpl();

	HC_KEY nKey = pcItemImpl->m_cKey.KeyValue();

	bool bNeedUpdate = true;

	H3DF::Type eType = cInItem.Type();

	// Region 선택 관련 처리 부분
	if (H3DF::Type::ShellKey == cInItem.Type() && (pcItemImpl->m_nLowest != pcItemImpl->m_nHighest || pcItemImpl->m_nLowest > 0)) {
		if (!pcSelSet->IsRegionSelected(nKey, pcItemImpl->m_nIncludeCount, pcItemImpl->m_pnIncludeKeys, pcItemImpl->m_nRegion)) {
			pcSelSet->SelectRegion(nKey, pcItemImpl->m_nIncludeCount, pcItemImpl->m_pnIncludeKeys, pcItemImpl->m_nRegion, false);
		}
		else {
			bNeedUpdate = false;
		}
	}
	else {
		if (!pcSelSet->IsSelected(nKey, pcItemImpl->m_nIncludeCount, pcItemImpl->m_pnIncludeKeys)) {
			HSelectLevel eSelectLevel = pcSelSet->GetSelectionLevel();

			if (pcSelSet->GetSelectionLevel() != HSelectSegment) // never should fail for dynamic highlighting, but let's be nice and check
			{
				char chType[MVO_BUFFER_SIZE];
				// the key is to a geometric entity.  If we are in segment selection mode,
				// then we need to get the key to its parent segment.
				HC_Show_Key_Type(nKey, chType);

				if (!streq("segment", chType))
				{
					char segname[MVO_BUFFER_SIZE];
					HC_KEY segkey;

					segkey = HC_KShow_Owner_Original_Key(nKey);
					HC_Show_Owner_By_Key(nKey, segname);

					// climb up one more level if this is the temporary highlight key
					if (true == pcSelSet->IsHighlightSegment(segkey))
					{
						segkey = HC_KShow_Owner_Original_Key(segkey);
						HC_Show_Owner_By_Key(segkey, segname);
					}
				}
			}

			pcSelSet->Select(nKey, pcItemImpl->m_nIncludeCount, pcItemImpl->m_pnIncludeKeys, false);
		}
		else {
			bNeedUpdate = false;
		}
	}

	if (bNeedUpdate) {
		bool bShowNotification = false;
		cInOptions.ShowNotification(bShowNotification);

		if (true == bShowNotification) {
			H3DF::BaseView * pcView = pcHighlightImpl->GetBaseView();
			pcView->ForceUpdate();
		}
	}

	return *this;
}

HighlightControl & H3DF::HighlightControl::Unhighlight(SelectionResults const & cInItems, HighlightOptionsKit const & cInOptions)
{
	if (0 == cInItems.GetCount()) {
		return *this;
	}

	HighlightControlImpl * pcHighlightImpl = (HighlightControlImpl *) m_pcImpl;

	HSelectionSet * pcSelection = pcHighlightImpl->SelectionSet(); // HSelectionSet에서 Select 및 Highlight를 다 처리함.

	SelectionResultsImpl * pcImpl = (SelectionResultsImpl *)cInItems.GetImpl();

	// cInItem를 순회하면서 Unhighlight를 수행한다.
	for (auto & pcItem : pcImpl->GetItems()) {
		SelectionItemImpl * pcItemImpl = (SelectionItemImpl *)pcItem.GetImpl();
		HC_KEY nKey = pcItemImpl->m_cKey.KeyValue();
		pcSelection->DeSelect(nKey, pcItemImpl->m_nIncludeCount, pcItemImpl->m_pnIncludeKeys, false);
	}

	bool bShowNotification = false;
	cInOptions.ShowNotification(bShowNotification);

	if (true == bShowNotification) {
		pcHighlightImpl->GetBaseView()->ForceUpdate();
	}
	return *this;
}

HighlightControl & H3DF::HighlightControl::Unhighlight(SelectionItem const & cInItem, HighlightOptionsKit const & cInOptions)
{
	HighlightControlImpl * pcHighlightImpl = (HighlightControlImpl *) m_pcImpl;

	// cInItem의 Impl을 가져와서 작업을 수행한다.
	SelectionItemImpl * pcItemImpl = (SelectionItemImpl *)cInItem.GetImpl();
	DEBUG_VALID(pcItemImpl);

	HC_KEY nKey = pcItemImpl->m_cKey.KeyValue();
	pcHighlightImpl->SelectionSet()->DeSelect(nKey, pcItemImpl->m_nIncludeCount, pcItemImpl->m_pnIncludeKeys, false);

	bool bShowNotification = false;
	cInOptions.ShowNotification(bShowNotification);

	if (true == bShowNotification) {
		pcHighlightImpl->GetBaseView()->ForceUpdate();
	}
	return *this;
}

HighlightControl & H3DF::HighlightControl::UnhighlightEverything()
{
	HighlightControlImpl * pcImpl = dynamic_cast<HighlightControlImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);
	pcImpl->SelectionSet()->DeSelectAll();
	return *this;
}

//== Material Mapping 관련 함수 ======================================================================
HighlightControl & H3DF::HighlightControl::SetMaterialMapping(MaterialMappingKit const & cInKit)
{
	HighlightControlImpl * pcHighlightControlImpl = (HighlightControlImpl *)m_pcImpl;
	HC_KEY nKey = pcHighlightControlImpl->SelectionSet()->GetHighlightStyle();
	
	SegmentKey cSegmentKey(nKey);
	cSegmentKey.SetMaterialMapping(cInKit);

	return *this;
}

MaterialMappingControl H3DF::HighlightControl::GetMaterialMappingControl()
{
	HighlightControlImpl * pcHighlightControlImpl = (HighlightControlImpl *)m_pcImpl;
	HC_KEY nKey = pcHighlightControlImpl->SelectionSet()->GetHighlightStyle();

	SegmentKey cSegmentKey(nKey);
	MaterialMappingControl cMaterialMappingControl(cSegmentKey);

	return cMaterialMappingControl;
}

MaterialMappingControl const H3DF::HighlightControl::GetMaterialMappingControl() const
{
	HighlightControlImpl * pcHighlightControlImpl = (HighlightControlImpl *)m_pcImpl;
	HC_KEY nKey = pcHighlightControlImpl->SelectionSet()->GetHighlightStyle();

	SegmentKey cSegmentKey(nKey);
	MaterialMappingControl cMaterialMappingControl(cSegmentKey);

	return cMaterialMappingControl;
}

//== Line Attribute 관련 함수 ====================================================================
/*
HighlightControl & H3DF::HighlightControl::SetLineAttribute(LineAttributeKit const & cInKit)
{
	HighlightControlPrivate * pcHighlightControlImpl = (HighlightControlPrivate *)m_pcImpl;
	HC_KEY nKey = pcHighlightControlImpl->m_pcSelectionSet->GetHighlightStyle();

	SegmentKey cSegmentKey(nKey);
	cSegmentKey.SetMaterialMapping(cInKit);

	return *this;
}
*/

VisibilityControl H3DF::HighlightControl::GetVisibilityControl()
{
	auto * pcImpl = (HighlightControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	HC_KEY nKey = pcImpl->SelectionSet()->GetHighlightStyle();

	SegmentKey cSegmentKey(nKey);
	VisibilityControl cControl(cSegmentKey);

	return cControl;
}

VisibilityControl const H3DF::HighlightControl::GetVisibilityControl() const
{
	auto * pcImpl = (HighlightControlImpl *)m_pcImpl;

	HC_KEY nKey = pcImpl->SelectionSet()->GetHighlightStyle();

	SegmentKey cSegmentKey(nKey);
	VisibilityControl cControl(cSegmentKey);

	return cControl;
}

AttributeLockControl H3DF::HighlightControl::GetAttributeLockControl()
{
	auto * pcImpl = (HighlightControlImpl *)m_pcImpl;

	HC_KEY nKey = pcImpl->SelectionSet()->GetHighlightStyle();

	SegmentKey cSegmentKey(nKey);
	AttributeLockControl cControl(cSegmentKey);

	return cControl;
}

AttributeLockControl const H3DF::HighlightControl::GetAttributeLockControl() const
{
	auto * pcImpl = (HighlightControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	HC_KEY nKey = pcImpl->SelectionSet()->GetHighlightStyle();

	SegmentKey cSegmentKey(nKey);
	AttributeLockControl cControl(cSegmentKey);

	return cControl;
}

LineAttributeControl H3DF::HighlightControl::GetLineAttributeControl()
{
	auto * pcImpl = (HighlightControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	HC_KEY nKey = pcImpl->SelectionSet()->GetHighlightStyle();

	SegmentKey cSegmentKey(nKey);
	LineAttributeControl cControl(cSegmentKey);

	return cControl;
}

LineAttributeControl const H3DF::HighlightControl::GetLineAttributeControl() const
{
	auto * pcImpl = (HighlightControlImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	HC_KEY nKey = pcImpl->SelectionSet()->GetHighlightStyle();

	SegmentKey cSegmentKey(nKey);
	LineAttributeControl cControl(cSegmentKey);

	return cControl;
}

HighlightControl & H3DF::HighlightControl::Highlight_ORG(SelectionResults const & cInItems, HighlightOptionsKit const & cInOptions, bool bInRemoveExisting)
{
	HighlightControlImpl * pcHighlightControlImpl = (HighlightControlImpl *)m_pcImpl;
	H3DF::BaseView * pcView = pcHighlightControlImpl->GetBaseView();

	char chType[MVO_BUFFER_SIZE];

	bool bNeedDeselect = true;
	bool bNeedUpdate = true;

	SelectionResultsImpl * pcImpl = (SelectionResultsImpl *) cInItems.GetImpl();

	for (auto cItem : pcImpl->GetItems()) {
		SelectionItemImpl * pcImpl = (SelectionItemImpl *)cItem.GetImpl();

		HC_KEY nKey = pcImpl->m_cKey.KeyValue();

		// Region 선택 관련 처리 부분
		if (H3DF::Type::ShellKey == cItem.Type() && (pcImpl->m_nLowest != pcImpl->m_nHighest || pcImpl->m_nLowest > 0)) {
			bNeedDeselect = false;

			if (!pcView->GetHighlightSelection()->IsRegionSelected(nKey, pcImpl->m_nIncludeCount, pcImpl->m_pnIncludeKeys, pcImpl->m_nRegion))
			{
				if (true == bInRemoveExisting) {
					pcView->GetHighlightSelection()->DeSelectAll();
				}

				pcView->GetHighlightSelection()->SelectRegion(nKey, pcImpl->m_nIncludeCount, pcImpl->m_pnIncludeKeys, pcImpl->m_nRegion, false);
			}
			else {
				bNeedUpdate = false;
			}
		}
		else {
			bNeedDeselect = false;

			if (!pcView->GetHighlightSelection()->IsSelected(nKey, pcImpl->m_nIncludeCount, pcImpl->m_pnIncludeKeys)) {
				if (pcView->GetHighlightSelection()->GetSelectionLevel() != HSelectSegment) // never should fail for dynamic highlighting, but let's be nice and check
				{
					// the key is to a geometric entity.  If we are in segment selection mode,
					// then we need to get the key to its parent segment.
					HC_Show_Key_Type(nKey, chType);

					if (!streq("segment", chType))
					{
						char segname[MVO_BUFFER_SIZE];
						HC_KEY segkey;

						segkey = HC_KShow_Owner_Original_Key(nKey);
						HC_Show_Owner_By_Key(nKey, segname);

						// climb up one more level if this is the temporary highlight key
						if (pcView->GetHighlightSelection()->IsHighlightSegment(segkey))
						{
							segkey = HC_KShow_Owner_Original_Key(segkey);
							HC_Show_Owner_By_Key(segkey, segname);
						}
					}
				}

				if (true == bInRemoveExisting) {
					pcView->GetHighlightSelection()->DeSelectAll();
				}
				
				pcView->GetHighlightSelection()->Select(nKey, pcImpl->m_nIncludeCount, pcImpl->m_pnIncludeKeys, false);
			}
			else {
				bNeedUpdate = false;
			}
		}
	}

	if (bNeedDeselect) {
		pcView->GetHighlightSelection()->DeSelectAll();
	}

	if (bNeedUpdate) {
		bool bShowNotification = false;
		cInOptions.ShowNotification(bShowNotification);

		if (true == bShowNotification) {
			pcView->ForceUpdate();
		}
	}

	return *this;
}
