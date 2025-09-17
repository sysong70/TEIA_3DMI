#include "StdAfx.h"

#include "Highlight.h"
#include "Impl/HighlightImpl.h"

#include "Impl/Selection.Impl.h"
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
H3DF::HighlightOptionsKit::HighlightOptionsKit()
{
	m_pcImpl = std::make_unique<HighlightOptionsKitImpl>();
	DEBUG_VALID(m_pcImpl);
}

H3DF::HighlightOptionsKit::HighlightOptionsKit(CStringA strInStyleName)
{
	m_pcImpl = std::make_unique<HighlightOptionsKitImpl>();
	static_cast<HighlightOptionsKitImpl *>(m_pcImpl.get())->m_strInStyleName = strInStyleName;
}

H3DF::HighlightOptionsKit::HighlightOptionsKit(CStringA strInStyleName, CStringA strInSecondaryStyleName)
{
	m_pcImpl = std::make_unique<HighlightOptionsKitImpl>();
	auto pcImpl = static_cast<HighlightOptionsKitImpl *>(m_pcImpl.get());

	pcImpl->m_strInStyleName = strInStyleName;
	pcImpl->m_strInSecondaryStyleName = strInSecondaryStyleName;
}

H3DF::HighlightOptionsKit::HighlightOptionsKit(HighlightOptionsKit const & cInThat)
{
	m_pcImpl = (nullptr != cInThat.m_pcImpl) ? cInThat.m_pcImpl->Clone() : nullptr;
}

HighlightOptionsKit & H3DF::HighlightOptionsKit::operator=(HighlightOptionsKit const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

HighlightOptionsKit & H3DF::HighlightOptionsKit::SetNotification(bool bInState)
{
	auto pcImpl = static_cast<HighlightOptionsKitImpl *>(m_pcImpl.get());

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
	auto pcImpl = static_cast<HighlightOptionsKitImpl *>(m_pcImpl.get());
	pcImpl->m_nNotification = -1;
	return *this;
}

bool H3DF::HighlightOptionsKit::ShowNotification(bool & bOutState) const
{
	auto pcImpl = static_cast<HighlightOptionsKitImpl *>(m_pcImpl.get());
	if (0 > pcImpl->m_nNotification) {
		return false;
	}

	bOutState = (0 != pcImpl->m_nNotification);
	return true;
}

//== HighlightControl Class ========================================================================

H3DF::HighlightControl::HighlightControl(WindowKey const & cInWindow)
{
	m_pcImpl = std::make_unique<HighlightControlImpl>(cInWindow);
}

/*
H3DF::HighlightControl::HighlightControl(HighlightControl const & cInThat)
{
	m_pcImpl = new HighlightControlPrivate();
	Set(cInThat);
}
*/

H3DF::HighlightControl::HighlightControl() {}


HighlightControl & H3DF::HighlightControl::operator=(HighlightControl const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

void H3DF::HighlightControl::SetMode(HighlightMode::Type eInMode)
{
	auto pcHighlightImpl = static_cast<HighlightControlImpl *>(m_pcImpl.get());

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
	auto pcHighlightImpl = static_cast<HighlightControlImpl *>(m_pcImpl.get());
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
	auto pcHighlightImpl = static_cast<HighlightControlImpl *>(m_pcImpl.get());

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

		HC_KEY nKey = pcItemImpl->Keys().front();

		std::vector<HC_KEY> vcIncludeKeys;
		pcItemImpl->GetIncludeKeys(vcIncludeKeys);

		// Region 선택 관련 처리 부분
		if (H3DF::Type::ShellKey == cItem.Type() && (pcItemImpl->m_nLowest != pcItemImpl->m_nHighest || pcItemImpl->m_nLowest > 0)) {
			bNeedDeselect = false;

			if (!pcSelSet->IsRegionSelected(nKey, (int) vcIncludeKeys.size(), vcIncludeKeys.data(), pcItemImpl->m_nRegion))
			{
				if (true == bInRemoveExisting) {
					pcSelSet->DeSelectAll();
				}

				pcSelSet->SelectRegion(nKey, (int) vcIncludeKeys.size(), vcIncludeKeys.data(), pcItemImpl->m_nRegion, false);
			}
			else {
				bNeedUpdate = false;
			}
		}
		else {
			bNeedDeselect = false;

			if (!pcSelSet->IsSelected(nKey, (int) vcIncludeKeys.size(), vcIncludeKeys.data())) {
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

				pcSelSet->Select(nKey, (int) vcIncludeKeys.size(), vcIncludeKeys.data(), false);
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
	auto pcHighlightImpl = static_cast<HighlightControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcHighlightImpl);

	HSelectionSet * pcSelSet = pcHighlightImpl->SelectionSet(); // HSelectionSet에서 Select 및 Highlight를 다 처리함.
	DEBUG_VALID(pcSelSet);

	if (true == bInRemoveExisting) {
		pcSelSet->DeSelectAll();
	}

	SelectionItemImpl * pcItemImpl = (SelectionItemImpl *)cInItem.GetImpl();

	HC_KEY nKey = pcItemImpl->Keys().front();
	std::vector<HC_KEY> vcIncludeKeys;
	pcItemImpl->GetIncludeKeys(vcIncludeKeys);

	bool bNeedUpdate = true;

	H3DF::Type eType = cInItem.Type();

	// Region 선택 관련 처리 부분
	if (H3DF::Type::ShellKey == cInItem.Type() && (pcItemImpl->m_nLowest != pcItemImpl->m_nHighest || pcItemImpl->m_nLowest > 0)) {
		if (!pcSelSet->IsRegionSelected(nKey, (int) vcIncludeKeys.size(), vcIncludeKeys.data(), pcItemImpl->m_nRegion)) {
			pcSelSet->SelectRegion(nKey, (int) vcIncludeKeys.size(), vcIncludeKeys.data(), pcItemImpl->m_nRegion, false);
		}
		else {
			bNeedUpdate = false;
		}
	}
	else {
		if (!pcSelSet->IsSelected(nKey, (int) vcIncludeKeys.size(), vcIncludeKeys.data())) {
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

			pcSelSet->Select(nKey, (int) vcIncludeKeys.size(), vcIncludeKeys.data(), false);
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

	auto pcHighlightImpl = static_cast<HighlightControlImpl *>(m_pcImpl.get());

	HSelectionSet * pcSelection = pcHighlightImpl->SelectionSet(); // HSelectionSet에서 Select 및 Highlight를 다 처리함.

	SelectionResultsImpl * pcItemImpl = (SelectionResultsImpl *)cInItems.GetImpl();

	// cInItem를 순회하면서 Unhighlight를 수행한다.
	for (auto & pcItem : pcItemImpl->GetItems()) {
		SelectionItemImpl * pcItemImpl = (SelectionItemImpl *)pcItem.GetImpl();
		HC_KEY nKey = pcItemImpl->Keys().front();
		std::vector<HC_KEY> vcIncludeKeys;
		pcItemImpl->GetIncludeKeys(vcIncludeKeys);

		pcSelection->DeSelect(nKey, (int) vcIncludeKeys.size(), vcIncludeKeys.data(), false);
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
	auto pcHighlightImpl = static_cast<HighlightControlImpl *>(m_pcImpl.get());

	// cInItem의 Impl을 가져와서 작업을 수행한다.
	SelectionItemImpl * pcItemImpl = (SelectionItemImpl *)cInItem.GetImpl();
	DEBUG_VALID(pcItemImpl);

	HC_KEY nKey = pcItemImpl->Keys().front();
	std::vector<HC_KEY> vcIncludeKeys;
	pcItemImpl->GetIncludeKeys(vcIncludeKeys);

	pcHighlightImpl->SelectionSet()->DeSelect(nKey, (int) vcIncludeKeys.size(), vcIncludeKeys.data(), false);

	bool bShowNotification = false;
	cInOptions.ShowNotification(bShowNotification);

	if (true == bShowNotification) {
		pcHighlightImpl->GetBaseView()->ForceUpdate();
	}
	return *this;
}

HighlightControl & H3DF::HighlightControl::UnhighlightEverything()
{
	auto pcHighlightImpl = static_cast<HighlightControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcHighlightImpl);
	pcHighlightImpl->SelectionSet()->DeSelectAll();
	return *this;
}

//== Material Mapping 관련 함수 ======================================================================
HighlightControl & H3DF::HighlightControl::SetMaterialMapping(MaterialMappingKit const & cInKit)
{
	auto pcHighlightControlImpl = static_cast<HighlightControlImpl *>(m_pcImpl.get());
	HC_KEY nKey = pcHighlightControlImpl->SelectionSet()->GetHighlightStyle();
	
	SegmentKey cSegment(nKey);
	cSegment.SetMaterialMapping(cInKit);

	return *this;
}

MaterialMappingControl H3DF::HighlightControl::GetMaterialMappingControl()
{
	auto pcHighlightControlImpl = static_cast<HighlightControlImpl *>(m_pcImpl.get());
	HC_KEY nKey = pcHighlightControlImpl->SelectionSet()->GetHighlightStyle();

	SegmentKey cSegment(nKey);
	MaterialMappingControl cMaterialMappingControl(cSegment);

	return cMaterialMappingControl;
}

MaterialMappingControl const H3DF::HighlightControl::GetMaterialMappingControl() const
{
	auto pcHighlightControlImpl = static_cast<HighlightControlImpl *>(m_pcImpl.get());
	HC_KEY nKey = pcHighlightControlImpl->SelectionSet()->GetHighlightStyle();

	SegmentKey cSegment(nKey);
	MaterialMappingControl cMaterialMappingControl(cSegment);

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
	auto pcImpl = static_cast<HighlightControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	HC_KEY nKey = pcImpl->SelectionSet()->GetHighlightStyle();

	SegmentKey cSegment(nKey);
	VisibilityControl cControl(cSegment);

	return cControl;
}

VisibilityControl const H3DF::HighlightControl::GetVisibilityControl() const
{
	auto pcImpl = static_cast<HighlightControlImpl *>(m_pcImpl.get());

	HC_KEY nKey = pcImpl->SelectionSet()->GetHighlightStyle();

	SegmentKey cSegment(nKey);
	VisibilityControl cControl(cSegment);

	return cControl;
}

AttributeLockControl H3DF::HighlightControl::GetAttributeLockControl()
{
	auto pcImpl = static_cast<HighlightControlImpl *>(m_pcImpl.get());

	HC_KEY nKey = pcImpl->SelectionSet()->GetHighlightStyle();

	SegmentKey cSegment(nKey);
	AttributeLockControl cControl(cSegment);

	return cControl;
}

AttributeLockControl const H3DF::HighlightControl::GetAttributeLockControl() const
{
	auto pcImpl = static_cast<HighlightControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	HC_KEY nKey = pcImpl->SelectionSet()->GetHighlightStyle();

	SegmentKey cSegment(nKey);
	AttributeLockControl cControl(cSegment);

	return cControl;
}

LineAttributeControl H3DF::HighlightControl::GetLineAttributeControl()
{
	auto pcImpl = static_cast<HighlightControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	HC_KEY nKey = pcImpl->SelectionSet()->GetHighlightStyle();

	SegmentKey cSegment(nKey);
	LineAttributeControl cControl(cSegment);

	return cControl;
}

LineAttributeControl const H3DF::HighlightControl::GetLineAttributeControl() const
{
	auto pcImpl = static_cast<HighlightControlImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	HC_KEY nKey = pcImpl->SelectionSet()->GetHighlightStyle();

	SegmentKey cSegment(nKey);
	LineAttributeControl cControl(cSegment);

	return cControl;
}

HighlightControl & H3DF::HighlightControl::Highlight_ORG(SelectionResults const & cInItems, HighlightOptionsKit const & cInOptions, bool bInRemoveExisting)
{
	auto pcHighlightControlImpl = static_cast<HighlightControlImpl *>(m_pcImpl.get());
	H3DF::BaseView * pcView = pcHighlightControlImpl->GetBaseView();

	char chType[MVO_BUFFER_SIZE];

	bool bNeedDeselect = true;
	bool bNeedUpdate = true;

	SelectionResultsImpl * pcItemImpl = (SelectionResultsImpl *) cInItems.GetImpl();

	for (auto cItem : pcItemImpl->GetItems()) {
		SelectionItemImpl * pcItemImpl = (SelectionItemImpl *)cItem.GetImpl();

		HC_KEY nKey = pcItemImpl->Keys().front();
		std::vector<HC_KEY> vcIncludeKeys;
		pcItemImpl->GetIncludeKeys(vcIncludeKeys);

		// Region 선택 관련 처리 부분
		if (H3DF::Type::ShellKey == cItem.Type() && (pcItemImpl->m_nLowest != pcItemImpl->m_nHighest || pcItemImpl->m_nLowest > 0)) {
			bNeedDeselect = false;

			if (!pcView->GetHighlightSelection()->IsRegionSelected(nKey, (int) vcIncludeKeys.size(), vcIncludeKeys.data(), pcItemImpl->m_nRegion))
			{
				if (true == bInRemoveExisting) {
					pcView->GetHighlightSelection()->DeSelectAll();
				}

				pcView->GetHighlightSelection()->SelectRegion(nKey, (int) vcIncludeKeys.size(), vcIncludeKeys.data(), pcItemImpl->m_nRegion, false);
			}
			else {
				bNeedUpdate = false;
			}
		}
		else {
			bNeedDeselect = false;

			if (!pcView->GetHighlightSelection()->IsSelected(nKey, (int) vcIncludeKeys.size(), vcIncludeKeys.data())) {
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
				
				pcView->GetHighlightSelection()->Select(nKey, (int) vcIncludeKeys.size(), vcIncludeKeys.data(), false);
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
